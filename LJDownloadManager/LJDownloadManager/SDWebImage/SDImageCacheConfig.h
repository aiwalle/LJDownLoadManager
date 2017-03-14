/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageCompat.h"

@interface SDImageCacheConfig : NSObject

/**
 * Decompressing images that are downloaded and cached can improve performance but can consume lot of memory.
 * 解压缩下载和缓存的映像可以提高性能，但可能会消耗大量内存
 * Defaults to YES. Set this to NO if you are experiencing a crash due to excessive memory consumption.
 * 默认为YES。 如果由于过多内存消耗而遇到崩溃，请将此选项设置为NO
 */
@property (assign, nonatomic) BOOL shouldDecompressImages;

/**
 *  disable iCloud backup [defaults to YES]
 *  禁止iCloud备份，默认情况下是yes
 */
@property (assign, nonatomic) BOOL shouldDisableiCloud;

/**
 * use memory cache [defaults to YES]
 * 使用内存缓存，默认是YES
 */
@property (assign, nonatomic) BOOL shouldCacheImagesInMemory;

/**
 * The maximum length of time to keep an image in the cache, in seconds
 * 保持一个图像缓存的最大时间长度，单位是秒
 */
@property (assign, nonatomic) NSInteger maxCacheAge;

/**
 * The maximum size of the cache, in bytes.
 * 最大的图像缓存大小，单位是比特
 */
@property (assign, nonatomic) NSUInteger maxCacheSize;

@end
