/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "SDWebImageDownloaderOperation.h"
#import "SDWebImageDecoder.h"
#import "UIImage+MultiFormat.h"
#import <ImageIO/ImageIO.h>
#import "SDWebImageManager.h"
#import "NSImage+WebCache.h"

NSString *const SDWebImageDownloadStartNotification = @"SDWebImageDownloadStartNotification";
NSString *const SDWebImageDownloadReceiveResponseNotification = @"SDWebImageDownloadReceiveResponseNotification";
NSString *const SDWebImageDownloadStopNotification = @"SDWebImageDownloadStopNotification";
NSString *const SDWebImageDownloadFinishNotification = @"SDWebImageDownloadFinishNotification";

static NSString *const kProgressCallbackKey = @"progress";
static NSString *const kCompletedCallbackKey = @"completed";

typedef NSMutableDictionary<NSString *, id> SDCallbacksDictionary;

@interface SDWebImageDownloaderOperation ()

@property (strong, nonatomic, nonnull) NSMutableArray<SDCallbacksDictionary *> *callbackBlocks;

@property (assign, nonatomic, getter = isExecuting) BOOL executing;
@property (assign, nonatomic, getter = isFinished) BOOL finished;
@property (strong, nonatomic, nullable) NSMutableData *imageData;

// This is weak because it is injected by whoever manages this session. If this gets nil-ed out, we won't be able to run
// the task associated with this operation
// 谁管理这个会话，他是弱引用且被注入的对象，如果获取到为nil,那么就不能执行
// 这个task是关联这个operation的
@property (weak, nonatomic, nullable) NSURLSession *unownedSession;
// This is set if we're using not using an injected NSURLSession. We're responsible of invalidating this one
// 如果我们使用不使用注入的NSURLSession，这是设置的。 我们负责使这一个无效
@property (strong, nonatomic, nullable) NSURLSession *ownedSession;

@property (strong, nonatomic, readwrite, nullable) NSURLSessionTask *dataTask;

@property (SDDispatchQueueSetterSementics, nonatomic, nullable) dispatch_queue_t barrierQueue;

#if SD_UIKIT
@property (assign, nonatomic) UIBackgroundTaskIdentifier backgroundTaskId;
#endif

@end

@implementation SDWebImageDownloaderOperation {
    size_t width, height;
#if SD_UIKIT || SD_WATCH
    UIImageOrientation orientation;
#endif
}

@synthesize executing = _executing;
@synthesize finished = _finished;

- (nonnull instancetype)init {
    return [self initWithRequest:nil inSession:nil options:0];
}

// 通过对应的request和session来生成对应的SDWebImageDownloaderOperation实例
- (nonnull instancetype)initWithRequest:(nullable NSURLRequest *)request
                              inSession:(nullable NSURLSession *)session
                                options:(SDWebImageDownloaderOptions)options {
    if ((self = [super init])) {
        _request = [request copy];
        _shouldDecompressImages = YES;
        _options = options;
        _callbackBlocks = [NSMutableArray new];
        _executing = NO;
        _finished = NO;
        _expectedSize = 0;
        _unownedSession = session;
        _barrierQueue = dispatch_queue_create("com.hackemist.SDWebImageDownloaderOperationBarrierQueue", DISPATCH_QUEUE_CONCURRENT);
    }
    return self;
}

- (void)dealloc {
    SDDispatchQueueRelease(_barrierQueue);
}

// 添加处理程序进度和完成的回调。 返回可传递到-cancel：以取消此回调集合的令牌。
- (nullable id)addHandlersForProgress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                            completed:(nullable SDWebImageDownloaderCompletedBlock)completedBlock {
    SDCallbacksDictionary *callbacks = [NSMutableDictionary new];
    if (progressBlock) callbacks[kProgressCallbackKey] = [progressBlock copy];
    if (completedBlock) callbacks[kCompletedCallbackKey] = [completedBlock copy];
    dispatch_barrier_async(self.barrierQueue, ^{
        [self.callbackBlocks addObject:callbacks];
    });
    return callbacks;
}

// 传入一个key返回对应的callbacks
- (nullable NSArray<id> *)callbacksForKey:(NSString *)key {
    __block NSMutableArray<id> *callbacks = nil;
    dispatch_sync(self.barrierQueue, ^{
        // We need to remove [NSNull null] because there might not always be a progress block for each callback
        callbacks = [[self.callbackBlocks valueForKey:key] mutableCopy];
        [callbacks removeObjectIdenticalTo:[NSNull null]];
    });
    return [callbacks copy];    // strip mutability here
}

- (BOOL)cancel:(nullable id)token {
    __block BOOL shouldCancel = NO;
    dispatch_barrier_sync(self.barrierQueue, ^{
        [self.callbackBlocks removeObjectIdenticalTo:token];
        if (self.callbackBlocks.count == 0) {
            shouldCancel = YES;
        }
    });
    if (shouldCancel) {
        [self cancel];
    }
    return shouldCancel;
}

// 实现了NSOperation的子类，那么要让其运行起来，要么实现main(),要么实现start()。
// 如果你只是想弄一个同步的方法，那很简单，你只要重写main这个函数，在里面添加你要的操作。如果想定义异步的方法的话就重写start方法
- (void)start {
    @synchronized (self) {
        // 判断当前这个SDWebImageDownloaderOperation是否取消了，如果取消了，即认为该任务已经完成，并且及时回收资源（即reset）
        if (self.isCancelled) {
            self.finished = YES;
            [self reset];
            return;
        }

#if SD_UIKIT
        Class UIApplicationClass = NSClassFromString(@"UIApplication");
        BOOL hasApplication = UIApplicationClass && [UIApplicationClass respondsToSelector:@selector(sharedApplication)];
        // 这里需要提及的就是shouldContinueWhenAppEntersBackground，也就是说下载选项中需要设置SDWebImageDownloaderContinueInBackground
        if (hasApplication && [self shouldContinueWhenAppEntersBackground]) {
            __weak __typeof__ (self) wself = self;
            UIApplication * app = [UIApplicationClass performSelector:@selector(sharedApplication)];
            // ⚠️注意beginBackgroundTaskWithExpirationHandler并不是意味着立即执行后台任务，它只是相当于注册了一个后台任务，函数后面的handler block表示程序在后台运行时间到了后，要运行的代码
            self.backgroundTaskId = [app beginBackgroundTaskWithExpirationHandler:^{
                __strong __typeof (wself) sself = wself;
                
                // 后台时间结束时，如果下载任务还在进行，就取消该任务，并且调用endBackgroundTask，以及置backgroundTaskId为UIBackgroundTaskInvalid
                if (sself) {
                    [sself cancel];

                    [app endBackgroundTask:sself.backgroundTaskId];
                    sself.backgroundTaskId = UIBackgroundTaskInvalid;
                }
            }];
        }
#endif
        NSURLSession *session = self.unownedSession;
        if (!self.unownedSession) {
            NSURLSessionConfiguration *sessionConfig = [NSURLSessionConfiguration defaultSessionConfiguration];
            sessionConfig.timeoutIntervalForRequest = 15;
            
            /**
             *  Create the session for this task
             *  We send nil as delegate queue so that the session creates a serial operation queue for performing all delegate
             *  method calls and completion handler calls.
             * 为这个task创建session
             * 我们发送nil作为委托队列，以便会话创建一个串行操作队列，用于执行所有委托方法调用和完成处理程序调用
             * 第三个参数：队列，如果该参数传递nil 那么默认在子线程中执行
             */
            self.ownedSession = [NSURLSession sessionWithConfiguration:sessionConfig
                                                              delegate:self
                                                         delegateQueue:nil];
            session = self.ownedSession;
        }
        
        self.dataTask = [session dataTaskWithRequest:self.request];
        self.executing = YES;
    }
    //发送请求
    [self.dataTask resume];

    if (self.dataTask) {
        // 将对应的下载信息回传到block中
        for (SDWebImageDownloaderProgressBlock progressBlock in [self callbacksForKey:kProgressCallbackKey]) {
            progressBlock(0, NSURLResponseUnknownLength, self.request.URL);
        }
        // 发送通知，下载已经开始
        dispatch_async(dispatch_get_main_queue(), ^{
            [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadStartNotification object:self];
        });
    } else {
        // dataTask无法被初始化，抛出错误
        [self callCompletionBlocksWithError:[NSError errorWithDomain:NSURLErrorDomain code:0 userInfo:@{NSLocalizedDescriptionKey : @"Connection can't be initialized"}]];
    }

#if SD_UIKIT
    // LJMARK：下面代码的作用?
    Class UIApplicationClass = NSClassFromString(@"UIApplication");
    if(!UIApplicationClass || ![UIApplicationClass respondsToSelector:@selector(sharedApplication)]) {
        return;
    }
    if (self.backgroundTaskId != UIBackgroundTaskInvalid) {
        UIApplication * app = [UIApplication performSelector:@selector(sharedApplication)];
        [app endBackgroundTask:self.backgroundTaskId];
        self.backgroundTaskId = UIBackgroundTaskInvalid;
    }
#endif
}

- (void)cancel {
    @synchronized (self) {
        [self cancelInternal];
    }
}

- (void)cancelInternal {
    if (self.isFinished) return;
    [super cancel];

    if (self.dataTask) {
        [self.dataTask cancel];
        dispatch_async(dispatch_get_main_queue(), ^{
            // 主线程发送下载停止的通知
            [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadStopNotification object:self];
        });

        // As we cancelled the connection, its callback won't be called and thus won't
        // maintain the isFinished and isExecuting flags.
        // isExecuting 代表任务正在执行中
        // isFinished 代表任务已经执行完成
        // isCancelled 代表任务已经取消执行
        if (self.isExecuting) self.executing = NO;
        if (!self.isFinished) self.finished = YES;
    }

    [self reset];
}

- (void)done {
    self.finished = YES;
    self.executing = NO;
    [self reset];
}

- (void)reset {
    dispatch_barrier_async(self.barrierQueue, ^{
        [self.callbackBlocks removeAllObjects];
    });
    self.dataTask = nil;
    self.imageData = nil;
    if (self.ownedSession) {
        [self.ownedSession invalidateAndCancel];
        self.ownedSession = nil;
    }
}

- (void)setFinished:(BOOL)finished {
    [self willChangeValueForKey:@"isFinished"];
    _finished = finished;
    [self didChangeValueForKey:@"isFinished"];
}

- (void)setExecuting:(BOOL)executing {
    [self willChangeValueForKey:@"isExecuting"];
    _executing = executing;
    [self didChangeValueForKey:@"isExecuting"];
}

- (BOOL)isConcurrent {
    return YES;
}

#pragma mark NSURLSessionDataDelegate

- (void)URLSession:(NSURLSession *)session
          dataTask:(NSURLSessionDataTask *)dataTask
didReceiveResponse:(NSURLResponse *)response
 completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler {
    
    //'304 Not Modified' is an exceptional one
    // HTTP的statusCode小于400表示正常码。但是304码表示文档的内容（自上次访问以来或者根据请求的条件）并没有改变，这里我们在获取图片时考虑直接使用Cache，所以statusCode为304时会单独处理
    // 如果Response返回正常码，并且不为304
    if (![response respondsToSelector:@selector(statusCode)] || (((NSHTTPURLResponse *)response).statusCode < 400 && ((NSHTTPURLResponse *)response).statusCode != 304)) {
        // 根据response中的expectedContentLength来给self.expectedSize进行赋值
        // 而self.expectedSize此处表示响应的数据体（此处为imageData）期望大小
        // 注意expectedContentLength为-1时，expectedSize赋值为0
        NSInteger expected = response.expectedContentLength > 0 ? (NSInteger)response.expectedContentLength : 0;
        self.expectedSize = expected;
        // 将对应的数据传到进程回调中
        for (SDWebImageDownloaderProgressBlock progressBlock in [self callbacksForKey:kProgressCallbackKey]) {
            progressBlock(0, expected, self.request.URL);
        }
        
        self.imageData = [[NSMutableData alloc] initWithCapacity:expected];
        self.response = response;
        // 不过好像SDWebImage中并没有addObserver这个SDWebImageDownloadReceiveResponseNotification
        // 可能需要用户自己去使用addObserver
        dispatch_async(dispatch_get_main_queue(), ^{
            [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadReceiveResponseNotification object:self];
        });
    }
    else {
        // 如果response返回错误码
        NSUInteger code = ((NSHTTPURLResponse *)response).statusCode;
        
        //This is the case when server returns '304 Not Modified'. It means that remote image is not changed.
        //In case of 304 we need just cancel the operation and return cached image from the cache.
        // 当服务器端返回statusCode为“304 Not Modified”，意味着服务器端的image并没有改变，
        // 此时，我们只需取消operation，然后返回缓存中的image
        if (code == 304) {
            [self cancelInternal];
        } else {
            [self.dataTask cancel];
        }
        //发送通知，已停止下载
        dispatch_async(dispatch_get_main_queue(), ^{
            [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadStopNotification object:self];
        });
        // 抛出错误
        [self callCompletionBlocksWithError:[NSError errorWithDomain:NSURLErrorDomain code:((NSHTTPURLResponse *)response).statusCode userInfo:nil]];
        // 初始化，重置资源
        [self done];
    }
    
    if (completionHandler) {
        completionHandler(NSURLSessionResponseAllow);
    }
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data {
    // 如果image比较大的话，会多次调用didReceiveData，这样一个image就分成很多块了，所以每次receive到data，就串起来
    [self.imageData appendData:data];
    
    // 单独处理SDWebImageDownloaderProgressiveDownload
    // SDWebImageProgressiveDownload表示image的显示过程是随着下载的进度一点点进行的，而不是下载完成后，一次显示完成。这就可以理解了，因为要随着下载进度显示，所以每接收到新的data，就要显示一下。为什么还需要completedBlock呢？因为在didReceiveData中只是获取到了imageData，但是还需要显示在imageView上呢？那就得使用completedBlock来进行处理。所以SDWebImageProgressiveDownload默认的图片显示是交给用户进行处理的
    if ((self.options & SDWebImageDownloaderProgressiveDownload) && self.expectedSize > 0) {
        // The following code is from http://www.cocoaintheshell.com/2011/05/progressive-images-download-imageio/
        // Thanks to the author @Nyx0uf

        // Get the total bytes downloaded
        // 获取总下载的数据大小
        const NSInteger totalSize = self.imageData.length;
        
        // 使用最新下载后的图片数据来创建一个CGImageSourceRef变量imageSource
        // 注意创建使用的数据是CoreFoundation的data，而self.imageData是NSData，所以要做如下转化
        // (__bridge CFDataRef)self.imageData
        // Update the data source, we must pass ALL the data, not just the new bytes
        CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef)self.imageData, NULL);
        
        // width + height == 0在此处其实就是表示width==0&&height==0
        // 初始条件下，也就是第一次执行时，width和height均为0
        if (width + height == 0) {
            // 从imageSource中获取图片的一些属性，比如长宽等等，是一个dictionary变量
            // 这里获取imageSource属性，直接传入imageSource就行，为啥还要传入一个index？
            // 因为对于gif图片，一个imageSource对应的CGImage会有多个，需要使用index
            // 底下会使用CGImageSourceCreateImageAtIndex来根据imageSource创建一个带index的CGImageRef
            CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(imageSource, 0, NULL);
            if (properties) {
                NSInteger orientationValue = -1;
                // 获取到图片高度
                CFTypeRef val = CFDictionaryGetValue(properties, kCGImagePropertyPixelHeight);
                if (val) CFNumberGetValue(val, kCFNumberLongType, &height);
                // 获取到图片宽度
                val = CFDictionaryGetValue(properties, kCGImagePropertyPixelWidth);
                if (val) CFNumberGetValue(val, kCFNumberLongType, &width);
                // 获取到图片朝向
                val = CFDictionaryGetValue(properties, kCGImagePropertyOrientation);
                if (val) CFNumberGetValue(val, kCFNumberNSIntegerType, &orientationValue);
                CFRelease(properties);

                // When we draw to Core Graphics, we lose orientation information,
                // which means the image below born of initWithCGIImage will be
                // oriented incorrectly sometimes. (Unlike the image born of initWithData
                // in didCompleteWithError.) So save it here and pass it on later.
                // 当我们绘制到Core Graphics时，我们失去方向信息，这意味着下面由initWithCGIImage生成的图像有时会被不正确地定向。 （不同于initWithData在didCompleteWithError中诞生的图像）。因此，将其保存在此并稍后传递。
#if SD_UIKIT || SD_WATCH
                orientation = [[self class] orientationFromPropertyValue:(orientationValue == -1 ? 1 : orientationValue)];
#endif
            }
        }
        
        // width和height更新过了，并且还没有获取到完整的图片数据（totalSize < self.expectedSize）
        if (width + height > 0 && totalSize < self.expectedSize) {
            // Create the image
            CGImageRef partialImageRef = CGImageSourceCreateImageAtIndex(imageSource, 0, NULL);

#if SD_UIKIT || SD_WATCH
            // Workaround for iOS anamorphic image
            // 解决iOS平台图片失真问题
            // 因为如果下载的图片是非png格式，图片会出现失真
            // 为了解决这个问题，先将图片在bitmap的context下渲染
            // 然后在传回partialImageRef
            if (partialImageRef) {
                const size_t partialHeight = CGImageGetHeight(partialImageRef);
                CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
                CGContextRef bmContext = CGBitmapContextCreate(NULL, width, height, 8, width * 4, colorSpace, kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedFirst);
                CGColorSpaceRelease(colorSpace);
                if (bmContext) {
                    CGContextDrawImage(bmContext, (CGRect){.origin.x = 0.0f, .origin.y = 0.0f, .size.width = width, .size.height = partialHeight}, partialImageRef);
                    CGImageRelease(partialImageRef);
                    partialImageRef = CGBitmapContextCreateImage(bmContext);
                    CGContextRelease(bmContext);
                }
                else {
                    CGImageRelease(partialImageRef);
                    partialImageRef = nil;
                }
            }
#endif
            // partialImageRef是一个CGImageRef类型的值，本质还是self.imageData
            if (partialImageRef) {
#if SD_UIKIT || SD_WATCH
                // 从CGImageRef转化为UIImage，scale你可以理解为图片后缀为@1x,@2x,@3x需要放大的倍数
                // 至于orientation后面会讲，暂时理解图片的朝向
                UIImage *image = [UIImage imageWithCGImage:partialImageRef scale:1 orientation:orientation];
#elif SD_MAC
                UIImage *image = [[UIImage alloc] initWithCGImage:partialImageRef size:NSZeroSize];
#endif
                // 有时候你不想直接把图片的url作为cache的key，因为有可能图片的url是动态变化的
                // 所以你可以自定义一个cache key filter
                NSString *key = [[SDWebImageManager sharedManager] cacheKeyForURL:self.request.URL];
                // scaledImageForKey是SDWebImageCompat的一个函数，主要是根据image名称中
                // @2x,@3x来设置scale，并通过initWithCGImage来获得image
                UIImage *scaledImage = [self scaledImageForKey:key image:image];
                // 判断是否要压缩图片，初始化默认是要压缩图片的
                if (self.shouldDecompressImages) {
                    image = [UIImage decodedImageWithImage:scaledImage];
                }
                else {
                    image = scaledImage;
                }
                CGImageRelease(partialImageRef);
                
                [self callCompletionBlocksWithImage:image imageData:nil error:nil finished:NO];
            }
        }
        // 释放资源
        CFRelease(imageSource);
    }

    for (SDWebImageDownloaderProgressBlock progressBlock in [self callbacksForKey:kProgressCallbackKey]) {
        progressBlock(self.imageData.length, self.expectedSize, self.request.URL);
    }
}

// 如果我们需要对缓存做更精确的控制，我们可以实现一些代理方法来允许应用来确定请求是否应该缓存
// 如果不实现此方法，NSURLSession 就简单地使用本来要传入 -URLSession:dataTask:willCacheResponse:completionHandler: 的那个缓存对象，
// 所以除非你需要改变一些值或者阻止缓存，否则这个代理方法不必实现
- (void)URLSession:(NSURLSession *)session
          dataTask:(NSURLSessionDataTask *)dataTask
 willCacheResponse:(NSCachedURLResponse *)proposedResponse
 completionHandler:(void (^)(NSCachedURLResponse *cachedResponse))completionHandler {
    
    NSCachedURLResponse *cachedResponse = proposedResponse;

    if (self.request.cachePolicy == NSURLRequestReloadIgnoringLocalCacheData) {
        // Prevents caching of responses
        cachedResponse = nil;
    }
    if (completionHandler) {
        completionHandler(cachedResponse);
    }
}

#pragma mark NSURLSessionTaskDelegate

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error {
    @synchronized(self) {
        self.dataTask = nil;
        dispatch_async(dispatch_get_main_queue(), ^{
            [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadStopNotification object:self];
            if (!error) {
                [[NSNotificationCenter defaultCenter] postNotificationName:SDWebImageDownloadFinishNotification object:self];
            }
        });
    }
    
    if (error) {
        [self callCompletionBlocksWithError:error];
    } else {
        if ([self callbacksForKey:kCompletedCallbackKey].count > 0) {
            /**
             *  If you specified to use `NSURLCache`, then the response you get here is what you need.
             *  if you specified to only use cached data via `SDWebImageDownloaderIgnoreCachedResponse`,
             *  the response data will be nil.
             *  So we don't need to check the cache option here, since the system will obey the cache option
             *  如果你指定使用`NSURLCache`，那么你在这里得到的响应就是你所需要的。 如果你指定只使用缓存数据通过
             *  `SDWebImageDownloaderIgnoreCachedResponse`，响应数据将是nil。
             *  因此我们不需要检查缓存选项在这里，因为系统将服从缓存选项
             */
            if (self.imageData) {
                UIImage *image = [UIImage sd_imageWithData:self.imageData];
                NSString *key = [[SDWebImageManager sharedManager] cacheKeyForURL:self.request.URL];
                image = [self scaledImageForKey:key image:image];
                // 不解压gif
                // Do not force decoding animated GIFs
                if (!image.images) {
                    if (self.shouldDecompressImages) {
                        if (self.options & SDWebImageDownloaderScaleDownLargeImages) {
#if SD_UIKIT || SD_WATCH
                            image = [UIImage decodedAndScaledDownImageWithImage:image];
                            [self.imageData setData:UIImagePNGRepresentation(image)];
#endif
                        } else {
                            image = [UIImage decodedImageWithImage:image];
                        }
                    }
                }
                if (CGSizeEqualToSize(image.size, CGSizeZero)) {
                    // 图片大小为0，报错
                    [self callCompletionBlocksWithError:[NSError errorWithDomain:SDWebImageErrorDomain code:0 userInfo:@{NSLocalizedDescriptionKey : @"Downloaded image has 0 pixels"}]];
                } else {
                    [self callCompletionBlocksWithImage:image imageData:self.imageData error:nil finished:YES];
                }
            } else {
                // image为空，报错
                [self callCompletionBlocksWithError:[NSError errorWithDomain:SDWebImageErrorDomain code:0 userInfo:@{NSLocalizedDescriptionKey : @"Image data is nil"}]];
            }
        }
    }
    // 释放资源
    [self done];
}

// 当客户端向目标服务器发送请求时。服务器会使用401进行响应。客户端收到响应后便开始认证挑战(Authentication Challenge)，而且是通过didReceiveChallenge:函数进行的。
// didReceiveChallenge:函数中的challenge对象包含了protectionSpace（NSURLProtectionSpace）实例属性，在此进行protectionSpace的检查。当检查不通过时既取消认证，这里需要注意下的是取消是必要的，因为didReceiveChallenge:可能会被调用多次
- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition disposition, NSURLCredential *credential))completionHandler {
    
    NSURLSessionAuthChallengeDisposition disposition = NSURLSessionAuthChallengePerformDefaultHandling;
    __block NSURLCredential *credential = nil;
    // NSURLProtectionSpace主要有Host、port、protocol、realm、authenticationMethod等属性。
    // 为了进行认证，程序需要使用服务端期望的认证信息创建一个NSURLCredential对象。我们可以调用authenticationMethod来确定服务端的认证方法，这个认证方法是在提供的认证请求的保护空间（protectionSpace）中。
    // 服务端信任认证(NSURLAuthenticationMethodServerTrust)需要一个由认证请求的保护空间提供的信任。使用credentialForTrust:来创建一个NSURLCredential对象。
    if ([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]) {
        // SDWebImageDownloaderAllowInvalidSSLCertificates表示允许不受信任SSL认证
        // 注释中提示尽量作为test使用，不要在最终production使用。
        // 所以此处使用performDefaultHandlingForAuthenticationChallenge，即使用系统提供的默认行为
        if (!(self.options & SDWebImageDownloaderAllowInvalidSSLCertificates)) {
            disposition = NSURLSessionAuthChallengePerformDefaultHandling;
        } else {
            credential = [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust];
            disposition = NSURLSessionAuthChallengeUseCredential;
        }
    } else {
        // 每次认证失败，previousFailureCount就会加1
        // 第一次认证（previousFailureCount == 0）并且有Credential，使用Credential认证
        // 非第一次认证或者第一次认证没有Credential，对于认证挑战，不提供Credential就去download一个request，但是如果这里challenge是需要Credential的challenge，那么使用这个方法是徒劳的
        if (challenge.previousFailureCount == 0) {
            if (self.credential) {
                credential = self.credential;
                disposition = NSURLSessionAuthChallengeUseCredential;
            } else {
                disposition = NSURLSessionAuthChallengeCancelAuthenticationChallenge;
            }
        } else {
            disposition = NSURLSessionAuthChallengeCancelAuthenticationChallenge;
        }
    }
    
    if (completionHandler) {
        completionHandler(disposition, credential);
    }
}

#pragma mark Helper methods

#if SD_UIKIT || SD_WATCH
+ (UIImageOrientation)orientationFromPropertyValue:(NSInteger)value {
    switch (value) {
        case 1:
            return UIImageOrientationUp;
        case 3:
            return UIImageOrientationDown;
        case 8:
            return UIImageOrientationLeft;
        case 6:
            return UIImageOrientationRight;
        case 2:
            return UIImageOrientationUpMirrored;
        case 4:
            return UIImageOrientationDownMirrored;
        case 5:
            return UIImageOrientationLeftMirrored;
        case 7:
            return UIImageOrientationRightMirrored;
        default:
            return UIImageOrientationUp;
    }
}
#endif

- (nullable UIImage *)scaledImageForKey:(nullable NSString *)key image:(nullable UIImage *)image {
    return SDScaledImageForKey(key, image);
}

- (BOOL)shouldContinueWhenAppEntersBackground {
    return self.options & SDWebImageDownloaderContinueInBackground;
}

- (void)callCompletionBlocksWithError:(nullable NSError *)error {
    [self callCompletionBlocksWithImage:nil imageData:nil error:error finished:YES];
}

- (void)callCompletionBlocksWithImage:(nullable UIImage *)image
                            imageData:(nullable NSData *)imageData
                                error:(nullable NSError *)error
                             finished:(BOOL)finished {
    NSArray<id> *completionBlocks = [self callbacksForKey:kCompletedCallbackKey];
    dispatch_main_async_safe(^{
        for (SDWebImageDownloaderCompletedBlock completedBlock in completionBlocks) {
            completedBlock(image, imageData, error, finished);
        }
    });
}

@end
