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

@interface UIView (WebCacheOperation)

/**
 *  Set the image load operation (storage in a UIView based dictionary)
 *  设置图片的读取操作（通过字典保存在UIView）
 *  @param operation the operation
 *  @param key       key for storing the operation
 */
- (void)sd_setImageLoadOperation:(nullable id)operation forKey:(nullable NSString *)key;

/**
 *  Cancel all operations for the current UIView and key
 *  给当前的view通过key取消所有的操作
 *  @param key key for identifying the operations
 */
- (void)sd_cancelImageLoadOperationWithKey:(nullable NSString *)key;

/**
 *  Just remove the operations corresponding to the current UIView and key without cancelling them
 *  只需删除对应于当前UIView和键的操作，而不取消它们
 *  @param key key for identifying the operations
 */
- (void)sd_removeImageLoadOperationWithKey:(nullable NSString *)key;

@end

#endif
