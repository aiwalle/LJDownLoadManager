/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "SDWebImageCompat.h"
#import "SDWebImageOperation.h"
#import "SDWebImageDownloader.h"
#import "SDImageCache.h"

typedef NS_OPTIONS(NSUInteger, SDWebImageOptions) {
    /**
     * By default, when a URL fail to be downloaded, the URL is blacklisted so the library won't keep trying.
     * This flag disable this blacklisting.
     * 默认情况下，当一个URL下载失败的时候，这个URL在黑名单，且这个库不会尝试再次下载
     * 这个标记禁止加入黑名单(失败后重试)
     */
    SDWebImageRetryFailed = 1 << 0,

    /**
     * By default, image downloads are started during UI interactions, this flags disable this feature,
     * leading to delayed download on UIScrollView deceleration for instance.
     * UI交互期间下载
     * 导致延迟下载在UIScrollView减速的时候
     */
    SDWebImageLowPriority = 1 << 1,

    /**
     * This flag disables on-disk caching
     * 只进行内存缓存，不进行磁盘缓存
     */
    SDWebImageCacheMemoryOnly = 1 << 2,

    /**
     * This flag enables progressive download, the image is displayed progressively during download as a browser would do.
     * By default, the image is only displayed once completely downloaded.
     * 这个标志可以渐进式下载,显示的图像是逐步在下载
     */
    SDWebImageProgressiveDownload = 1 << 3,

    /**
     * Even if the image is cached, respect the HTTP response cache control, and refresh the image from remote location if needed.
     * 即使图像缓存，也要遵守HTTP响应缓存控制，如果需要，可以从远程位置刷新图像
     * The disk caching will be handled by NSURLCache instead of SDWebImage leading to slight performance degradation.
     * 磁盘缓存将由NSURLCache而不是SDWebImage处理，导致轻微的性能降低。
     * This option helps deal with images changing behind the same request URL, e.g. Facebook graph api profile pics.
     * If a cached image is refreshed, the completion block is called once with the cached image and again with the final image.
     * 这个选项帮助处理在同样的网络请求地址下图片的改变
     * 如果刷新缓存的图像，完成的block会在使用缓存图像的时候调用，还会在最后的图像被调用
     * Use this flag only if you can't make your URLs static with embedded cache busting parameter.
     * 当你不能使你的URL静态与嵌入式缓存
     */
    SDWebImageRefreshCached = 1 << 4,

    /**
     * In iOS 4+, continue the download of the image if the app goes to background. This is achieved by asking the system for
     * extra time in background to let the request finish. If the background task expires the operation will be cancelled.
     * 在iOS4以上，如果app进入后台，也保持下载图像，这个需要取得用户权限
     * 如果后台任务过期，操作将被取消
     */
    SDWebImageContinueInBackground = 1 << 5,

    /**
     * Handles cookies stored in NSHTTPCookieStore by setting
     * NSMutableURLRequest.HTTPShouldHandleCookies = YES;
     * 操作cookies存储在NSHTTPCookieStore通过设置NSMutableURLRequest.HTTPShouldHandleCookies = YES
     */
    SDWebImageHandleCookies = 1 << 6,

    /**
     * Enable to allow untrusted SSL certificates.
     * 允许使用无效的SSL证书
     * Useful for testing purposes. Use with caution in production.
     * 用户测试，生成情况下小心使用
     */
    SDWebImageAllowInvalidSSLCertificates = 1 << 7,

    /**
     * By default, images are loaded in the order in which they were queued. This flag moves them to
     * the front of the queue.
     * 优先下载
     */
    SDWebImageHighPriority = 1 << 8,
    
    /**
     * By default, placeholder images are loaded while the image is loading. This flag will delay the loading
     * of the placeholder image until after the image has finished loading.
     * 在加载图片时加载占位图。 此标志将延迟加载占位符图像，直到图像完成加载。
     */
    SDWebImageDelayPlaceholder = 1 << 9,

    /**
     * We usually don't call transformDownloadedImage delegate method on animated images,
     * as most transformation code would mangle it.
     * Use this flag to transform them anyway.
     * 我们通常不调用transformDownloadedImage代理方法在动画图像上，大多数情况下会对图像进行耗损
     * 无论什么情况下都使用
     */
    SDWebImageTransformAnimatedImage = 1 << 10,
    
    /**
     * By default, image is added to the imageView after download. But in some cases, we want to
     * have the hand before setting the image (apply a filter or add it with cross-fade animation for instance)
     * Use this flag if you want to manually set the image in the completion when success
     * 图片在下载后被加载到imageView。但是在一些情况下，我们想要设置一下图片（引用一个滤镜或者加入透入动画）
     * 使用这个来手动的设置图片在下载图片成功后
     */
    SDWebImageAvoidAutoSetImage = 1 << 11,
    
    /**
     * By default, images are decoded respecting their original size. On iOS, this flag will scale down the
     * images to a size compatible with the constrained memory of devices.
     * If `SDWebImageProgressiveDownload` flag is set the scale down is deactivated.
     * 图像将根据其原始大小进行解码。 在iOS上，此标记会将图片缩小到与设备的受限内存兼容的大小。
     */
    SDWebImageScaleDownLargeImages = 1 << 12
};

typedef void(^SDExternalCompletionBlock)(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL);

typedef void(^SDInternalCompletionBlock)(UIImage * _Nullable image, NSData * _Nullable data, NSError * _Nullable error, SDImageCacheType cacheType, BOOL finished, NSURL * _Nullable imageURL);

typedef NSString * _Nullable (^SDWebImageCacheKeyFilterBlock)(NSURL * _Nullable url);


@class SDWebImageManager;

@protocol SDWebImageManagerDelegate <NSObject>

@optional

/**
 * Controls which image should be downloaded when the image is not found in the cache.
 * 当图像没有在内存中找到的时候，控制是否下载图像
 * @param imageManager The current `SDWebImageManager`
 * @param imageURL     The url of the image to be downloaded
 *
 * @return Return NO to prevent the downloading of the image on cache misses. If not implemented, YES is implied.
 * 如果设置为NO,内存中没有找到也不会去下载图像，如果没有实现这个方法，默认是YES
 */
- (BOOL)imageManager:(nonnull SDWebImageManager *)imageManager shouldDownloadImageForURL:(nullable NSURL *)imageURL;

/**
 * Allows to transform the image immediately after it has been downloaded and just before to cache it on disk and memory.
 * NOTE: This method is called from a global queue in order to not to block the main thread.
 * 允许在图像立即下载之后立即对其进行转换，然后将其缓存到磁盘和内存中
 * 这个方法是在一个全局队列中执行，为了不阻塞主线程
 * @param imageManager The current `SDWebImageManager`
 * @param image        The image to transform
 * @param imageURL     The url of the image to transform
 *
 * @return The transformed image object.
 * 返回的是被转换的图像对象
 */
- (nullable UIImage *)imageManager:(nonnull SDWebImageManager *)imageManager transformDownloadedImage:(nullable UIImage *)image withURL:(nullable NSURL *)imageURL;

@end

/**
 * The SDWebImageManager is the class behind the UIImageView+WebCache category and likes.
 * It ties the asynchronous downloader (SDWebImageDownloader) with the image cache store (SDImageCache).
 * You can use this class directly to benefit from web image downloading with caching in another context than
 * a UIView.
 *
 * SDWebImageManager是在UIImageView+WebCache中的一个分类
 * 它将异步下载器（SDWebImageDownloader）与图像缓存存储（SDImageCache）绑定在一起。 您可以直接使用这个类从在UIView之外的另一个上下文中使用缓存获得Web图像下载。
 * Here is a simple example of how to use SDWebImageManager:
 *
 * @code

SDWebImageManager *manager = [SDWebImageManager sharedManager];
[manager loadImageWithURL:imageURL
                  options:0
                 progress:nil
                completed:^(UIImage *image, NSError *error, SDImageCacheType cacheType, BOOL finished, NSURL *imageURL) {
                    if (image) {
                        // do something with image
                    }
                }];

 * @endcode
 */
@interface SDWebImageManager : NSObject

@property (weak, nonatomic, nullable) id <SDWebImageManagerDelegate> delegate;

@property (strong, nonatomic, readonly, nullable) SDImageCache *imageCache;
@property (strong, nonatomic, readonly, nullable) SDWebImageDownloader *imageDownloader;

/**
 * The cache filter is a block used each time SDWebImageManager need to convert an URL into a cache key. This can
 * be used to remove dynamic part of an image URL.
 * 缓存过滤器是每次SDWebImageManager需要将URL转换为缓存密钥时使用的块。 这可用于删除图片网址的动态部分。
 * The following example sets a filter in the application delegate that will remove any query-string from the
 * URL before to use it as a cache key:
 * 下面的例子是设置一个在应用的代理中设置一个过滤器，该过滤器将在将其用作缓存键之前从URL中删除任何查询字符串
 * @code

[[SDWebImageManager sharedManager] setCacheKeyFilter:^(NSURL *url) {
    url = [[NSURL alloc] initWithScheme:url.scheme host:url.host path:url.path];
    return [url absoluteString];
}];

 * @endcode
 */
@property (nonatomic, copy, nullable) SDWebImageCacheKeyFilterBlock cacheKeyFilter;

/**
 * Returns global SDWebImageManager instance.
 * 返回全局SDWebImageManager实例对象
 * @return SDWebImageManager shared instance
 */
+ (nonnull instancetype)sharedManager;

/**
 * Allows to specify instance of cache and image downloader used with image manager.
 * @return new instance of `SDWebImageManager` with specified cache and downloader.
 * 允许指定实例缓存和用于图像管理者的图像下载器
 * 返回通过指定缓存和下载器创建的新的SDWebImageManager
 */
- (nonnull instancetype)initWithCache:(nonnull SDImageCache *)cache downloader:(nonnull SDWebImageDownloader *)downloader NS_DESIGNATED_INITIALIZER;

/**
 * Downloads the image at the given URL if not present in cache or return the cached version otherwise.
 * 如果不存在于缓存中，请下载给定URL的图像，否则返回缓存的版本。
 * @param url            The URL to the image
 * 图像的url
 * @param options        A mask to specify options to use for this request
 * 指定用于此请求的选项的掩码
 * @param progressBlock  A block called while image is downloading
 *                       @note the progress block is executed on a background queue
 * 当图像下载中时调用的block
 * 这个进程的回调是在一个后台队列执行
 * @param completedBlock A block called when operation has been completed.
 * 当操作完成的回调
 *   This parameter is required.
 
 *   这个参数是必须的
 
 *   This block has no return value and takes the requested UIImage as first parameter and the NSData representation as second parameter.
 *   In case of error the image parameter is nil and the third parameter may contain an NSError.
 
 *   这个回调无返回值，把请求到的图像作为第一个参数，将NSData表示作为第二个参数。
 *   为了防止图像的参数为空导致的错误，这里的第三个参数包含一个NSError对象
 
 *   The forth parameter is an `SDImageCacheType` enum indicating if the image was retrieved from the local cache
 *   or from the memory cache or from the network.

 *   第四个参数是一个SDImageCacheType枚举值，图像是从本地缓存还是内存缓存或者是从网络上获得
 
 *   The fith parameter is set to NO when the SDWebImageProgressiveDownload option is used and the image is
 *   downloading. This block is thus called repeatedly with a partial image. When image is fully downloaded, the
 *   block is called a last time with the full image and the last parameter set to YES.
 
 *   当使用SDWebImageProgressiveDownload，且图像正在下载的时候，这个值是NO.当呈现部分图像的时候会多次调用这个回调，当图像完全下载完毕以后，这个回调会最后一次调用，返回全部的图像，这个参数最后被设定为YES
 
 *   The last parameter is the original image URL
 *   最后一个参数是原始的图像url
 
 * @return Returns an NSObject conforming to SDWebImageOperation. Should be an instance of SDWebImageDownloaderOperation
 *   返回一个遵循SDWebImageOperation的对象，应该是一个SDWebImageDownloaderOperation对象的实例
 */
- (nullable id <SDWebImageOperation>)loadImageWithURL:(nullable NSURL *)url
                                              options:(SDWebImageOptions)options
                                             progress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                                            completed:(nullable SDInternalCompletionBlock)completedBlock;

/**
 * Saves image to cache for given URL
 *
 * @param image The image to cache
 * @param url   The URL to the image
 * 通过给定的URL存储图像到缓存
 */

- (void)saveImageToCache:(nullable UIImage *)image forURL:(nullable NSURL *)url;

/**
 * Cancel all current operations
 * 取消所有的当前operation操作
 */
- (void)cancelAll;

/**
 * Check one or more operations running
 * 检查一个或多个operation是否允许
 */
- (BOOL)isRunning;

/**
 *  Async check if image has already been cached
 *  异步检查如果图像已经被缓存
 *  @param url              image url
 *  @param completionBlock  the block to be executed when the check is finished
 *  当检查结束时调用的回调
 *  @note the completion block is always executed on the main queue
 *  这个回调总会在主线程执行
 */
- (void)cachedImageExistsForURL:(nullable NSURL *)url
                     completion:(nullable SDWebImageCheckCacheCompletionBlock)completionBlock;

/**
 *  Async check if image has already been cached on disk only
 *  异步检查如果图像已经仅仅被缓存到磁盘
 *  @param url              image url
 *  @param completionBlock  the block to be executed when the check is finished
 *
 *  @note the completion block is always executed on the main queue
 *  这个回调总会在主线程执行
 */
- (void)diskImageExistsForURL:(nullable NSURL *)url
                   completion:(nullable SDWebImageCheckCacheCompletionBlock)completionBlock;


/**
 *Return the cache key for a given URL
 * 通过给定的URL返回缓存的key
 */
- (nullable NSString *)cacheKeyForURL:(nullable NSURL *)url;

@end
