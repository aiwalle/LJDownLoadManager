//
//  UIView+Animation.h
//  LJDownloadManager
//
//  Created by liang on 17/3/22.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SDImageCache.h"
@interface UIView (Animation)
/**
 *  网络图片显示 淡入淡出动画
 *
 */
- (void)crossDissolveAnimationForWebImage;

/**
 *  SDWebImage网络图片显示 淡入淡出动画
 *
 *  @param type SDWebImage缓存方式，只有当Type为SDImageCacheTypeNone 时才会生效
 *
 *  @param image 获取到的图片
 */
- (void)crossDissolveAnimationForWebImageWithType:(SDImageCacheType)type image:(UIImage *)image;

- (void)lj_setImageWithURL:(nullable NSURL *)url placeholderImage:(nullable UIImage *)placeholder;

@end
