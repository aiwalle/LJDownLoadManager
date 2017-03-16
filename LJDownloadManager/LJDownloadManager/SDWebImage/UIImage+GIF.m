/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 * (c) Laurin Brandner
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "UIImage+GIF.h"
#import <ImageIO/ImageIO.h>
#import "objc/runtime.h"
#import "NSImage+WebCache.h"

@implementation UIImage (GIF)

+ (UIImage *)sd_animatedGIFWithData:(NSData *)data {
    if (!data) {
        return nil;
    }
    // 根据data创建一个CG下的imageSource
    CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)data, NULL);
    // 返回imageSource中的image数目，为后面创建CGImage提供index
    size_t count = CGImageSourceGetCount(source);

    UIImage *staticImage;
    // count<=1的时候，就当单张图片
    if (count <= 1) {
        staticImage = [[UIImage alloc] initWithData:data];
    } else {
        // we will only retrieve the 1st frame. the full GIF support is available via the FLAnimatedImageView category.
        // this here is only code to allow drawing animated images as static ones
#if SD_WATCH
        CGFloat scale = 1;
        scale = [WKInterfaceDevice currentDevice].screenScale;
#elif SD_UIKIT
        CGFloat scale = 1;
        scale = [UIScreen mainScreen].scale;
#endif
         // 根据指定的index创建CGImage
        CGImageRef CGImage = CGImageSourceCreateImageAtIndex(source, 0, NULL);
#if SD_UIKIT || SD_WATCH
        UIImage *frameImage = [UIImage imageWithCGImage:CGImage scale:scale orientation:UIImageOrientationUp];
        staticImage = [UIImage animatedImageWithImages:@[frameImage] duration:0.0f];
#elif SD_MAC
        staticImage = [[UIImage alloc] initWithCGImage:CGImage size:NSZeroSize];
#endif
        CGImageRelease(CGImage);
    }

    CFRelease(source);

    return staticImage;
}

- (BOOL)isGIF {
    return (self.images != nil);
}

@end
