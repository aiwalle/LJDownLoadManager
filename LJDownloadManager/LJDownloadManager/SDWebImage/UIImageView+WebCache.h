/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "SDWebImageCompat.h"

#if SD_UIKIT || SD_MAC

#import "SDWebImageManager.h"

/**
 * Integrates SDWebImage async downloading and caching of remote images with UIImageView.
 *
 * Usage with a UITableViewCell sub-class:
 *
 * @code
 
 给UIImageView集成SDWebImage 异步下载和获取远程远程服务器的图片

#import <SDWebImage/UIImageView+WebCache.h>

...

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *MyIdentifier = @"MyIdentifier";
 
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];
 
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:MyIdentifier]
                 autorelease];
    }
 
    // Here we use the provided sd_setImageWithURL: method to load the web image
    // Ensure you use a placeholder image otherwise cells will be initialized with no image
    // 这里我们使用提供的sd_setImageWithURL:来读取网络图片
    // 确保你使用了一个占位图片，否则初始化的cell会没有图像
    [cell.imageView sd_setImageWithURL:[NSURL URLWithString:@"http://example.com/image.jpg"]
                      placeholderImage:[UIImage imageNamed:@"placeholder"]];
 
    cell.textLabel.text = @"My Text";
    return cell;
}

 * @endcode
 */
@interface UIImageView (WebCache)

/**
 * Set the imageView `image` with an `url`.
 * 使用一个url来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url The url for the image.
   图像的url
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url;

/**
 * Set the imageView `image` with an `url` and a placeholder.
 * 使用一个url和占位图片来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url         The url for the image.
 * 图像的url
 * @param placeholder The image to be set initially, until the image request finishes.
 * 占位图片，初始化时被设置，在请求结束时消失
 * @see sd_setImageWithURL:placeholderImage:options:
 * 参看 sd_setImageWithURL:placeholderImage:options:
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder;

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 * 使用一个url，占位图片和自定义选项来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url         The url for the image.
 * 图像的url
 * @param placeholder The image to be set initially, until the image request finishes.
 * 占位图片，初始化时被设置，在请求结束时消失
 * @param options     The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * 选项，在下载图片的时候使用的选项，看SDWebImageOptions有哪些可能的值
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder
                   options:(SDWebImageOptions)options;

/**
 * Set the imageView `image` with an `url`.
 * 使用一个url来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url            The url for the image.
 * 图像的url
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 * completedBlock:当操作结束时调用的block,这个block没有返回值，吧请求到的图像作为第一个参数，如果发生错误的话，第一个参数为空，第二个参数会包含一个NSError对象，第三个参数是一个bool值，指是从本地缓存还是从网络来重新获取图像，第四个参数是图片原始的url
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
                 completed:(nullable SDExternalCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder.
 * 使用一个url和占位图片来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url            The url for the image.
 * 图像的url
 * @param placeholder    The image to be set initially, until the image request finishes.
 * 占位图片，初始化时被设置，在请求结束时消失
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 * completedBlock:当操作结束时调用的block,这个block没有返回值，吧请求到的图像作为第一个参数，如果发生错误的话，第一个参数为空，第二个参数会包含一个NSError对象，第三个参数是一个bool值，指是从本地缓存还是从网络来重新获取图像，第四个参数是图片原始的url
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder
                 completed:(nullable SDExternalCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 * 使用一个url，占位图片和自定义选项来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url            The url for the image.
 * 图像的url
 * @param placeholder    The image to be set initially, until the image request finishes.
 * 占位图片，初始化时被设置，在请求结束时消失
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * 选项，在下载图片的时候使用的选项，看SDWebImageOptions有哪些可能的值
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 * completedBlock:当操作结束时调用的block,这个block没有返回值，吧请求到的图像作为第一个参数，如果发生错误的话，第一个参数为空，第二个参数会包含一个NSError对象，第三个参数是一个bool值，指是从本地缓存还是从网络来重新获取图像，第四个参数是图片原始的url
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder
                   options:(SDWebImageOptions)options
                 completed:(nullable SDExternalCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url`, placeholder and custom options.
 * 使用一个url，占位图片和自定义选项来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url            The url for the image.
 * 图像的url
 * @param placeholder    The image to be set initially, until the image request finishes.
 * 占位图片，初始化时被设置，在请求结束时消失
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * 选项，在下载图片的时候使用的选项，看SDWebImageOptions有哪些可能的值
 * @param progressBlock  A block called while image is downloading
 *                       @note the progress block is executed on a background queue
 * 当图像下载时候调用的block,这个block在一个后台队列执行
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 * completedBlock:当操作结束时调用的block,这个block没有返回值，吧请求到的图像作为第一个参数，如果发生错误的话，第一个参数为空，第二个参数会包含一个NSError对象，第三个参数是一个bool值，指是从本地缓存还是从网络来重新获取图像，第四个参数是图片原始的url
 */
- (void)sd_setImageWithURL:(nullable NSURL *)url
          placeholderImage:(nullable UIImage *)placeholder
                   options:(SDWebImageOptions)options
                  progress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                 completed:(nullable SDExternalCompletionBlock)completedBlock;

/**
 * Set the imageView `image` with an `url` and optionally a placeholder image.
 * 使用一个url，占位图片和自定义选项来设置imageView
 * The download is asynchronous and cached.
 * 下载是异步且是可以获得的（指下载进度）
 * @param url            The url for the image.
 * 图像的url
 * @param placeholder    The image to be set initially, until the image request finishes.
 * 下载是异步且是可以获得的（指下载进度）
 * @param options        The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * 选项，在下载图片的时候使用的选项，看SDWebImageOptions有哪些可能的值
 * @param progressBlock  A block called while image is downloading
 *                       @note the progress block is executed on a background queue
 * 当图像下载时候调用的block,这个block在一个后台队列执行
 * @param completedBlock A block called when operation has been completed. This block has no return value
 *                       and takes the requested UIImage as first parameter. In case of error the image parameter
 *                       is nil and the second parameter may contain an NSError. The third parameter is a Boolean
 *                       indicating if the image was retrieved from the local cache or from the network.
 *                       The fourth parameter is the original image url.
 * completedBlock:当操作结束时调用的block,这个block没有返回值，吧请求到的图像作为第一个参数，如果发生错误的话，第一个参数为空，第二个参数会包含一个NSError对象，第三个参数是一个bool值，指是从本地缓存还是从网络来重新获取图像，第四个参数是图片原始的url
 */
- (void)sd_setImageWithPreviousCachedImageWithURL:(nullable NSURL *)url
                                 placeholderImage:(nullable UIImage *)placeholder
                                          options:(SDWebImageOptions)options
                                         progress:(nullable SDWebImageDownloaderProgressBlock)progressBlock
                                        completed:(nullable SDExternalCompletionBlock)completedBlock;

#if SD_UIKIT

#pragma mark - Animation of multiple images

/**
 * Download an array of images and starts them in an animation loop
 * 下载一组图像，在一个动画循环里开始它们
 * @param arrayOfURLs An array of NSURL
 * 一组NSURL对象
 */
- (void)sd_setAnimationImagesWithURLs:(nonnull NSArray<NSURL *> *)arrayOfURLs;

- (void)sd_cancelCurrentAnimationImagesLoad;

#endif

@end

#endif
