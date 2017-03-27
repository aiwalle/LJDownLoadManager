//
//  UIView+Animation.m
//  LJDownloadManager
//
//  Created by liang on 17/3/22.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "UIView+Animation.h"
#import "SDWebImage/UIImageView+WebCache.h"
@implementation UIView (CrossAnimation)

- (void)crossDissolveAnimationForWebImage {
    [UIView transitionWithView:self
                      duration:5.3
                       options:UIViewAnimationOptionTransitionCrossDissolve
                    animations:^{
                        self.alpha = 1.0f;
                    } completion:NULL];
}

- (void)crossDissolveAnimationForWebImageWithType:(SDImageCacheType)type image:(UIImage *)image {
    if ([self isKindOfClass:[UIImageView class]]) {
        if (type == SDImageCacheTypeNone) {
            [self crossDissolveAnimationForWebImage];
        }
        UIImageView *imageView = (UIImageView *)self;
        imageView.image = image;
        [self setNeedsLayout];
    } else {
        [self crossDissolveAnimationForWebImage];
    }
}

- (void)lj_setImageWithURL:(nullable NSURL *)url placeholderImage:(nullable UIImage *)placeholder {
    if ([self isKindOfClass:[UIImageView class]]) {
        UIImageView *imageView = (UIImageView *)self;
        [imageView sd_setImageWithURL:url placeholderImage:placeholder options:SDWebImageAvoidAutoSetImage completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
            [imageView crossDissolveAnimationForWebImageWithType:cacheType image:image];
        }];
    }
}


@end
