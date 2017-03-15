/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"
#import "SDImageCacheConfig.h"

typedef NS_ENUM(NSInteger, SDImageCacheType) {
    /**
     * The image wasn't available the SDWebImage caches, but was downloaded from the web.
     * 不用缓存，直接从网上下载
     */
    SDImageCacheTypeNone,
    /**
     * The image was obtained from the disk cache.
     * 从磁盘获得
     */
    SDImageCacheTypeDisk,
    /**
     * The image was obtained from the memory cache.
     * 从内存获得
     */
    SDImageCacheTypeMemory
};

typedef void(^SDCacheQueryCompletedBlock)(UIImage * _Nullable image, NSData * _Nullable data, SDImageCacheType cacheType);

typedef void(^SDWebImageCheckCacheCompletionBlock)(BOOL isInCache);

typedef void(^SDWebImageCalculateSizeBlock)(NSUInteger fileCount, NSUInteger totalSize);


/**
 * SDImageCache maintains a memory cache and an optional disk cache. Disk cache write operations are performed
 * asynchronous so it doesn’t add unnecessary latency to the UI.
 * SDImageCache维护内存缓存和可选磁盘缓存。 磁盘高速缓存写入操作是异步执行的，因此不会对UI增加不必要的延迟
 */
@interface SDImageCache : NSObject

#pragma mark - Properties

/**
 *  Cache Config object - storing all kind of settings
 *  缓存配置对象-存储所有种类的设置
 */
@property (nonatomic, nonnull, readonly) SDImageCacheConfig *config;

/**
 * The maximum "total cost" of the in-memory image cache. The cost function is the number of pixels held in memory.
 * LJMARK:内存中图像缓存的最大“总成本”。 成本函数是存储器中保存的像素数。
 */
@property (assign, nonatomic) NSUInteger maxMemoryCost;

/**
 * The maximum number of objects the cache should hold.
 * 缓存保持的对象的最大个数
 */
@property (assign, nonatomic) NSUInteger maxMemoryCountLimit;

#pragma mark - Singleton and initialization

/**
 * Returns global shared cache instance
 * 返回单例
 * @return SDImageCache global instance
 */
+ (nonnull instancetype)sharedImageCache;

/**
 * Init a new cache store with a specific namespace
 * 用一个特定的命名空间来初始化一个新的缓存存储
 * @param ns The namespace to use for this cache store
 */
- (nonnull instancetype)initWithNamespace:(nonnull NSString *)ns;

/**
 * Init a new cache store with a specific namespace and directory
 * 用一个特定的命名空间和字典来初始化一个新的缓存存储
 * @param ns        The namespace to use for this cache store
 * @param directory Directory to cache disk images in
 */
- (nonnull instancetype)initWithNamespace:(nonnull NSString *)ns
                       diskCacheDirectory:(nonnull NSString *)directory NS_DESIGNATED_INITIALIZER;

#pragma mark - Cache paths

- (nullable NSString *)makeDiskCachePath:(nonnull NSString*)fullNamespace;

/**
 * Add a read-only cache path to search for images pre-cached by SDImageCache
 * Useful if you want to bundle pre-loaded images with your app
 * LJMARK:添加只读缓存路径以搜索由SDImageCache预缓存的图像，如果您希望将预加载的图像与应用程序捆绑在一起这很有用
 * @param path The path to use for this read-only cache path
 */
- (void)addReadOnlyCachePath:(nonnull NSString *)path;

#pragma mark - Store Ops

/**
 * Asynchronously store an image into memory and disk cache at the given key.
 * 使用一个给定的key异步存储一个图像到内存和磁盘缓存
 * @param image           The image to store
 *                        存储的图像
 * @param key             The unique image cache key, usually it's image absolute URL
 *                        唯一的图像缓存key，通常是URL
 * @param completionBlock A block executed after the operation is finished
 *                        一个在operation完成后执行的block
 */
- (void)storeImage:(nullable UIImage *)image
            forKey:(nullable NSString *)key
        completion:(nullable SDWebImageNoParamsBlock)completionBlock;

/**
 * Asynchronously store an image into memory and disk cache at the given key.
 * 使用一个给定的key异步存储一个图像到内存和磁盘缓存
 * @param image           The image to store
 *                        存储的图像
 * @param key             The unique image cache key, usually it's image absolute URL
 *                        唯一的图像缓存key，通常是URL
 * @param toDisk          Store the image to disk cache if YES
 *                        是否存储到磁盘
 * @param completionBlock A block executed after the operation is finished
 *                        一个在operation完成后执行的block
 */
- (void)storeImage:(nullable UIImage *)image
            forKey:(nullable NSString *)key
            toDisk:(BOOL)toDisk
        completion:(nullable SDWebImageNoParamsBlock)completionBlock;

/**
 * Asynchronously store an image into memory and disk cache at the given key.
 * 使用一个给定的key异步存储一个图像到内存和磁盘缓存
 * @param image           The image to store
 *                        存储的图像
 * @param imageData       The image data as returned by the server, this representation will be used for disk storage
 *                        instead of converting the given image object into a storable/compressed image format in order
 *                        to save quality and CPU
 *                        服务器返回的图片数据，将被用作磁盘存储，而不是将给定的图像对象转换成可存储/压缩的图像格式，以便节省质量和CPU
 * @param key             The unique image cache key, usually it's image absolute URL
 *                        唯一的图像缓存key，通常是URL
 * @param toDisk          Store the image to disk cache if YES
 *                        是否存储到磁盘
 * @param completionBlock A block executed after the operation is finished
 *                        一个在operation完成后执行的block
 */
- (void)storeImage:(nullable UIImage *)image
         imageData:(nullable NSData *)imageData
            forKey:(nullable NSString *)key
            toDisk:(BOOL)toDisk
        completion:(nullable SDWebImageNoParamsBlock)completionBlock;

/**
 * Synchronously store image NSData into disk cache at the given key.
 * 通过给定的key同步存储图像数据到磁盘缓存
 * @warning This method is synchronous, make sure to call it from the ioQueue
 * 警告，这个方法时同步的，确认调用在写入写出队列
 * @param imageData  The image data to store
 * @param key        The unique image cache key, usually it's image absolute URL
 */
- (void)storeImageDataToDisk:(nullable NSData *)imageData forKey:(nullable NSString *)key;

#pragma mark - Query and Retrieve Ops

/**
 *  Async check if image exists in disk cache already (does not load the image)
 *  异步的检查，如果图像已经存在磁盘中(不加载图像)
 *  @param key             the key describing the url
 *                         描述url的key
 *  @param completionBlock the block to be executed when the check is done.
 *                         当检查执行完毕后，这个block会被执行
 *  @note the completion block will be always executed on the main queue
 *                         这个完成的block会在主线程执行
 */
- (void)diskImageExistsWithKey:(nullable NSString *)key completion:(nullable SDWebImageCheckCacheCompletionBlock)completionBlock;

/**
 * Operation that queries the cache asynchronously and call the completion when done.
 * Operation，异步的查询缓存，查询完毕后调用回调的operation
 * @param key       The unique key used to store the wanted image
 *                  唯一的key用来存储所要的图像
 * @param doneBlock The completion block. Will not get called if the operation is cancelled
 *                  完成的回调，如果operation取消的话将不会调用
 * @return a NSOperation instance containing the cache op
 *                  返回一个NSOperation实例，包含对应的缓存操作
 */
- (nullable NSOperation *)queryCacheOperationForKey:(nullable NSString *)key done:(nullable SDCacheQueryCompletedBlock)doneBlock;

/**
 * Query the memory cache synchronously.
 * 同步的查询内存缓存
 * @param key The unique key used to store the image
 * 唯一的key用来存储所要的图像
 */
- (nullable UIImage *)imageFromMemoryCacheForKey:(nullable NSString *)key;

/**
 * Query the disk cache synchronously.
 * 同步的查询磁盘缓存
 * @param key The unique key used to store the image
 * 唯一的key用来存储所要的图像
 */
- (nullable UIImage *)imageFromDiskCacheForKey:(nullable NSString *)key;

/**
 * Query the cache (memory and or disk) synchronously after checking the memory cache.
 * 在检查完内存缓存以后，同步的查询内存和磁盘缓存
 * @param key The unique key used to store the image
 * 唯一的key用来存储所要的图像
 */
- (nullable UIImage *)imageFromCacheForKey:(nullable NSString *)key;

#pragma mark - Remove Ops

/**
 * Remove the image from memory and disk cache asynchronously
 * 异步的从内存和磁盘缓存中移除图像
 * @param key             The unique image cache key
 * 唯一的图片缓存key
 * @param completion      A block that should be executed after the image has been removed (optional)
 * 在对应的图片被移除后执行的回调（可选的）
 */
- (void)removeImageForKey:(nullable NSString *)key withCompletion:(nullable SDWebImageNoParamsBlock)completion;

/**
 * Remove the image from memory and optionally disk cache asynchronously
 * 异步的从内存中移除图像，从磁盘中移除是可选的
 * @param key             The unique image cache key
 * 唯一的图像缓存key
 * @param fromDisk        Also remove cache entry from disk if YES
 * 如果是YES，那么也从磁盘中移除
 * @param completion      A block that should be executed after the image has been removed (optional)
 * 在对应的图片被移除后执行的回调（可选的）
 */
- (void)removeImageForKey:(nullable NSString *)key fromDisk:(BOOL)fromDisk withCompletion:(nullable SDWebImageNoParamsBlock)completion;

#pragma mark - Cache clean Ops

/**
 * Clear all memory cached images
 * 清楚所有的内存缓存图像
 */
- (void)clearMemory;

/**
 * Async clear all disk cached images. Non-blocking method - returns immediately.
 * @param completion    A block that should be executed after cache expiration completes (optional)
 * 异步的清楚所有的磁盘缓存。阻塞方法 - 立即返回。
 * 缓存到期后执行的回调(可选)
 */
- (void)clearDiskOnCompletion:(nullable SDWebImageNoParamsBlock)completion;

/**
 * Async remove all expired cached image from disk. Non-blocking method - returns immediately.
 * @param completionBlock A block that should be executed after cache expiration completes (optional)
 * 从磁盘中异步的移除所有国企的缓存图片。阻塞方法 - 立即返回。
 * 缓存到期后执行的回调(可选)
 */
- (void)deleteOldFilesWithCompletionBlock:(nullable SDWebImageNoParamsBlock)completionBlock;

#pragma mark - Cache Info

/**
 * Get the size used by the disk cache
 * 获取到磁盘缓存的大小
 */
- (NSUInteger)getSize;

/**
 * Get the number of images in the disk cache
 * 获得磁盘缓存的图像数
 */
- (NSUInteger)getDiskCount;

/**
 * Asynchronously calculate the disk cache's size.
 * 异步的计算磁盘缓存的大小
 */
- (void)calculateSizeWithCompletionBlock:(nullable SDWebImageCalculateSizeBlock)completionBlock;

#pragma mark - Cache Paths

/**
 *  Get the cache path for a certain key (needs the cache path root folder)
 *  通过一个明确的key（需要对应的缓存路径根文件）
 *  @param key  the key (can be obtained from url using cacheKeyForURL)
 *  能够使用cacheKeyForURL从中获得的url
 *  @param path the cache path root folder
 *  缓存对应的根目录文件
 *  @return the cache path
 *  返回缓存路径
 */
- (nullable NSString *)cachePathForKey:(nullable NSString *)key inPath:(nonnull NSString *)path;

/**
 *  Get the default cache path for a certain key
 *  通过一个确定的key获得默认的缓存地址
 *  @param key the key (can be obtained from url using cacheKeyForURL)
 *  能够使用cacheKeyForURL从中获得的url
 *  @return the default cache path
 *  返回默认的缓存路径
 */
- (nullable NSString *)defaultCachePathForKey:(nullable NSString *)key;

@end
