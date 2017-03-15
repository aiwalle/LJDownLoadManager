/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"
#import "SDWebImageOperation.h"

typedef NS_OPTIONS(NSUInteger, SDWebImageDownloaderOptions) {
    // 低优先级的下载
    SDWebImageDownloaderLowPriority = 1 << 0,
    // 持续渐进的下载
    SDWebImageDownloaderProgressiveDownload = 1 << 1,

    /**
     * By default, request prevent the use of NSURLCache. With this flag, NSURLCache
     * is used with default policies.
     * 默认情况下，请求阻止使用NSURLCache。 使用此标志，NSURLCache将与默认策略一起使用
     */
    SDWebImageDownloaderUseNSURLCache = 1 << 2,

    /**
     * Call completion block with nil image/imageData if the image was read from NSURLCache
     * (to be combined with `SDWebImageDownloaderUseNSURLCache`).
     * I think this option should be renamed to 'SDWebImageDownloaderUsingCachedResponseDontLoad'
     * 如果图像从NSURLCache读取（与“SDWebImageDownloaderUseNSURLCache”组合），回调里的image和imageData为nil
     * 我认为这个选项应该重命名为'SDWebImageDownloaderUsingCachedResponseDontLoad'
     */
    SDWebImageDownloaderIgnoreCachedResponse = 1 << 3,
    
    /**
     * In iOS 4+, continue the download of the image if the app goes to background. This is achieved by asking the system for
     * extra time in background to let the request finish. If the background task expires the operation will be cancelled.
     * 在iOS4以上，如果app进入后台，也保持下载图像，这个需要取得用户权限
     * 如果后台任务过期，操作将被取消
     */
    SDWebImageDownloaderContinueInBackground = 1 << 4,

    /**
     * Handles cookies stored in NSHTTPCookieStore by setting 
     * NSMutableURLRequest.HTTPShouldHandleCookies = YES;
     * 操作cookies存储在NSHTTPCookieStore通过设置NSMutableURLRequest.HTTPShouldHandleCookies = YES
     */
    SDWebImageDownloaderHandleCookies = 1 << 5,

    /**
     * Enable to allow untrusted SSL certificates.
     * Useful for testing purposes. Use with caution in production.
     * 允许使用无效的SSL证书
     * 用户测试，生成情况下小心使用
     */
    SDWebImageDownloaderAllowInvalidSSLCertificates = 1 << 6,

    /**
     * Put the image in the high priority queue.
     * 将图像放在高优先级队列中
     */
    SDWebImageDownloaderHighPriority = 1 << 7,
    
    /**
     * Scale down the image
     * 缩小图像
     */
    SDWebImageDownloaderScaleDownLargeImages = 1 << 8,
};

typedef NS_ENUM(NSInteger, SDWebImageDownloaderExecutionOrder) {
    /**
     * Default value. All download operations will execute in queue style (first-in-first-out).
     * 默认值，先进先出.所有的下载的操作在队列中的执行情况
     */
    SDWebImageDownloaderFIFOExecutionOrder,

    /**
     * All download operations will execute in stack style (last-in-first-out).
     * 后进先出，
     */
    SDWebImageDownloaderLIFOExecutionOrder
};

extern NSString * _Nonnull const SDWebImageDownloadStartNotification;
extern NSString * _Nonnull const SDWebImageDownloadStopNotification;

typedef void(^SDWebImageDownloaderProgressBlock)(NSInteger receivedSize, NSInteger expectedSize, NSURL * _Nullable targetURL);

typedef void(^SDWebImageDownloaderCompletedBlock)(UIImage * _Nullable image, NSData * _Nullable data, NSError * _Nullable error, BOOL finished);

typedef NSDictionary<NSString *, NSString *> SDHTTPHeadersDictionary;
typedef NSMutableDictionary<NSString *, NSString *> SDHTTPHeadersMutableDictionary;

typedef SDHTTPHeadersDictionary * _Nullable (^SDWebImageDownloaderHeadersFilterBlock)(NSURL * _Nullable url, SDHTTPHeadersDictionary * _Nullable headers);

/**
 *  A token associated with each download. Can be used to cancel a download
 *  一个与每个下载关联的Token，可以用来取消下载
 */
@interface SDWebImageDownloadToken : NSObject

@property (nonatomic, strong, nullable) NSURL *url;
@property (nonatomic, strong, nullable) id downloadOperationCancelToken;

@end


/**
 * Asynchronous downloader dedicated and optimized for image loading.
 * 异步的下载者专注用于图像加载
 */
@interface SDWebImageDownloader : NSObject

/**
 * Decompressing images that are downloaded and cached can improve performance but can consume lot of memory.
 * Defaults to YES. Set this to NO if you are experiencing a crash due to excessive memory consumption.
 * 解压缩下载和缓存的映像可以提高性能，但可能会消耗大量内存
 * 默认为YES。 如果由于过多内存消耗而遇到崩溃，请将此选项设置为NO
 */
@property (assign, nonatomic) BOOL shouldDecompressImages;

/**
 *  The maximum number of concurrent downloads
 *  最大的下载数
 */
@property (assign, nonatomic) NSInteger maxConcurrentDownloads;

/**
 * Shows the current amount of downloads that still need to be downloaded
 * 显示仍需下载的当前下载量
 */
@property (readonly, nonatomic) NSUInteger currentDownloadCount;


/**
 *  The timeout value (in seconds) for the download operation. Default: 15.0.
 * 下载任务的超时时间，默认是15s
 */
@property (assign, nonatomic) NSTimeInterval downloadTimeout;


/**
 * Changes download operations execution order. Default value is `SDWebImageDownloaderFIFOExecutionOrder`.
 * 改变下载操作执行顺序，默认是先进先出
 */
@property (assign, nonatomic) SDWebImageDownloaderExecutionOrder executionOrder;

/**
 *  Singleton method, returns the shared instance
 *
 *  @return global shared instance of downloader class
 *  返回单例
 */
+ (nonnull instancetype)sharedDownloader;

/**
 *  Set the default URL credential to be set for request operations.
 *  为请求操作设置默认的URL信任
 */
@property (strong, nonatomic, nullable) NSURLCredential *urlCredential;

/**
 * Set username 用户名
 */
@property (strong, nonatomic, nullable) NSString *username;

/**
 * Set password 密码
 */
@property (strong, nonatomic, nullable) NSString *password;

/**
 * Set filter to pick headers for downloading image HTTP request.
 * 为下载图片的http请求设置过滤器
 * This block will be invoked for each downloading image request, returned
 * NSDictionary will be used as headers in corresponding HTTP request.
 * 这个回调会被调用在每一次下载图片的请求
 * 返回的NSDictionary将被用作相应的HTTP请求中的头
 */
@property (nonatomic, copy, nullable) SDWebImageDownloaderHeadersFilterBlock headersFilter;

/**
 * Creates an instance of a downloader with specified session configuration.
 * 通过特定的session配置来创建一个下载的实例
 * *Note*: `timeoutIntervalForRequest` is going to be overwritten.
 * 请求超时时间很可能会被重写
 * @return new instance of downloader class
 * 返回一个新的下载实例
 */
- (nonnull instancetype)initWithSessionConfiguration:(nullable NSURLSessionConfiguration *)sessionConfiguration NS_DESIGNATED_INITIALIZER;

/**
 * Set a value for a HTTP header to be appended to each download HTTP request.
 * 设置每一次http请求下载的请求头
 * @param value The value for the header field. Use `nil` value to remove the header.
 * 头文件的值，如果设为nil,就会移除header
 * @param field The name of the header field to set.
 * 头文件的名称
 */
- (void)setValue:(nullable NSString *)value forHTTPHeaderField:(nullable NSString *)field;

/**
 * Returns the value of the specified HTTP header field.
 * 返回特定的http头文件的值
 * @return The value associated with the header field field, or `nil` if there is no corresponding header field.
 * 头文件相关的文件，如果没有相应的头字段那么为nil
 */
- (nullable NSString *)valueForHTTPHeaderField:(nullable NSString *)field;

/**
 * Sets a subclass of `SDWebImageDownloaderOperation` as the default
 * `NSOperation` to be used each time SDWebImage constructs a request
 * operation to download an image.
 * 设置`SDWebImageDownloaderOperation`的子类作为默认的`NSOperation`，每次SDWebImage构造一个下载图像的请求操作时使用。
 * @param operationClass The subclass of `SDWebImageDownloaderOperation` to set 
 *        as default. Passing `nil` will revert to `SDWebImageDownloaderOperation`.
 * operationClass  `SDWebImageDownloaderOperation`的子类设置为默认值。 传递`nil`将恢复为`SDWebImageDownloaderOperation`。
 */
- (void)setOperationClass:(nullable Class)operationClass;

/**
 * Creates a SDWebImageDownloader async downloader instance with a given URL
 * 通过给定的URL来创建一个异步的SDWebImageDownloader下载实例
 * The delegate will be informed when the image is finish downloaded or an error has happen.
 * 当图片下载完成或者发生错误的时候，会通知代理
 * @see SDWebImageDownloaderDelegate
 *
 * @param url            The URL to the image to download
 * @param options        The options to be used for this download
 * @param progressBlock  A block called repeatedly while the image is downloading
 *                       @note the progress block is executed on a background queue
 * @param completedBlock A block called once the download is completed.
 *                       If the download succeeded, the image parameter is set, in case of error,
 *                       error parameter is set with the error. The last parameter is always YES
 *                       if SDWebImageDownloaderProgressiveDownload isn't use. With the
 *                       SDWebImageDownloaderProgressiveDownload option, this block is called
 *                       repeatedly with the partial image object and the finished argument set to NO
 *                       before to be called a last time with the full image and finished argument
 *                       set to YES. In case of error, the finished argument is always YES.
 * 当下载完成时调用的回调。如果下载成功，图像参数会被设置，万一发生了错误，错误参数会被设置，如果SDWebImageDownloaderProgressiveDownload没有使用，最后的参数总是YES。通过SDWebImageDownloaderProgressiveDownload选项，当呈现部分图像的时候会多次调用这个回调，当图像完全下载完毕以后，这个回调会最后一次调用，返回全部的图像，这个参数最后被设定为YES
 *
 * @return A token (SDWebImageDownloadToken) that can be passed to -cancel: to cancel this operation
 */
- (nullable SDWebImageDownloadToken *)downloadImageWithURL:(nullable NSURL *)url
                                                   options:(SDWebImageDownloaderOptions)options
                                                  progress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                                                 completed:(nullable SDWebImageDownloaderCompletedBlock)completedBlock;

/**
 * Cancels a download that was previously queued using -downloadImageWithURL:options:progress:completed:
 * 取消一个下载，之前使用-downloadImageWithURL:options:progress:completed:
 * @param token The token received from -downloadImageWithURL:options:progress:completed: that should be canceled.
 * 上面方法返回的token
 */
- (void)cancel:(nullable SDWebImageDownloadToken *)token;

/**
 * Sets the download queue suspension state
 * 设置下载的队列状态为暂停
 */
- (void)setSuspended:(BOOL)suspended;

/**
 * Cancels all download operations in the queue
 * 取消队列中的所有下载operations
 */
- (void)cancelAllDownloads;

@end
