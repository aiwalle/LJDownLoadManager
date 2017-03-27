/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "UIImage+MultiFormat.h"
#import "UIImage+GIF.h"
#import "NSData+ImageContentType.h"
#import <ImageIO/ImageIO.h>

#ifdef SD_WEBP
#import "UIImage+WebP.h"
#endif

@implementation UIImage (MultiFormat)

+ (nullable UIImage *)sd_imageWithData:(nullable NSData *)data {
    if (!data) {
        return nil;
    }
    
    UIImage *image;
    // 根据data的前面几个字节，判断出图片类型，是jepg，png，gif还是...
    SDImageFormat imageFormat = [NSData sd_imageFormatForImageData:data];
    // 如果是gif图片
    if (imageFormat == SDImageFormatGIF) {
        image = [UIImage sd_animatedGIFWithData:data];
    }
#ifdef SD_WEBP
    // webp图片
    else if (imageFormat == SDImageFormatWebP)
    {
        image = [UIImage sd_imageWithWebPData:data];
    }
#endif
    else {
        image = [[UIImage alloc] initWithData:data];
#if SD_UIKIT || SD_WATCH
        // 获取朝向信息
        UIImageOrientation orientation = [self sd_imageOrientationFromImageData:data];
        // 默认朝向就是向上的，所以如果不是向上的图片，才进行调整，省时间，优化
        if (orientation != UIImageOrientationUp) {
            image = [UIImage imageWithCGImage:image.CGImage
                                        scale:image.scale
                                  orientation:orientation];
        }
#endif
    }


    return image;
}

#if SD_UIKIT || SD_WATCH
+(UIImageOrientation)sd_imageOrientationFromImageData:(nonnull NSData *)imageData {
    // 保证如果imageData中获取不到朝向信息，就默认UIImageOrientationUp
    UIImageOrientation result = UIImageOrientationUp;
    CGImageSourceRef imageSource = CGImageSourceCreateWithData((__bridge CFDataRef)imageData, NULL);
    if (imageSource) {
        CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(imageSource, 0, NULL);
        if (properties) {
            CFTypeRef val;
            int exifOrientation;
            val = CFDictionaryGetValue(properties, kCGImagePropertyOrientation);
            if (val) {
                // 这个kCGImagePropertyOrientation先转化为int值
                // 然后用一个switch case语句将int转化为朝向的enum值（sd_exifOrientationToiOSOrientation）
                CFNumberGetValue(val, kCFNumberIntType, &exifOrientation);
                result = [self sd_exifOrientationToiOSOrientation:exifOrientation];
            } // else - if it's not set it remains at up
            CFRelease((CFTypeRef) properties);
        } else {
            //NSLog(@"NO PROPERTIES, FAIL");
        }
        CFRelease(imageSource);
    }
    return result;
}

#pragma mark EXIF orientation tag converter
// Convert an EXIF image orientation to an iOS one.
// reference see here: http://sylvana.net/jpegcrop/exif_orientation.html
+ (UIImageOrientation) sd_exifOrientationToiOSOrientation:(int)exifOrientation {
    UIImageOrientation orientation = UIImageOrientationUp;
    switch (exifOrientation) {
        case 1:
            orientation = UIImageOrientationUp;
            break;

        case 3:
            orientation = UIImageOrientationDown;
            break;

        case 8:
            orientation = UIImageOrientationLeft;
            break;

        case 6:
            orientation = UIImageOrientationRight;
            break;

        case 2:
            orientation = UIImageOrientationUpMirrored;
            break;

        case 4:
            orientation = UIImageOrientationDownMirrored;
            break;

        case 5:
            orientation = UIImageOrientationLeftMirrored;
            break;

        case 7:
            orientation = UIImageOrientationRightMirrored;
            break;
        default:
            break;
    }
    return orientation;
}
#endif

- (nullable NSData *)sd_imageData {
    return [self sd_imageDataAsFormat:SDImageFormatUndefined];
}

- (nullable NSData *)sd_imageDataAsFormat:(SDImageFormat)imageFormat {
    NSData *imageData = nil;
    if (self) {
#if SD_UIKIT || SD_WATCH
        // 我们需要判断image是PNG还是JPEG
        // PNG的图片很容易检测出来，因为它们有一个特定的标示 (http://www.w3.org/TR/PNG-Structure.html)
        // PNG图片的前8个字节不许符合下面这些值(十进制表示)
        // 137 80 78 71 13 10 26 10
        
        // 如果imageData为空 (举个例子，比如image在下载后需要transform，那么就imageData就会为空)
        // 并且image有一个alpha通道, 我们将该image看做PNG以避免透明度(alpha)的丢失（因为JPEG没有透明色）
        int alphaInfo = CGImageGetAlphaInfo(self.CGImage);
        // 该image中确实有透明信息，就认为image为PNG
        BOOL hasAlpha = !(alphaInfo == kCGImageAlphaNone ||
                          alphaInfo == kCGImageAlphaNoneSkipFirst ||
                          alphaInfo == kCGImageAlphaNoneSkipLast);
        
        BOOL usePNG = hasAlpha;
        // 但是如果我们已经有了imageData，我们就可以直接根据data中前几个字节判断是不是PNG
        // the imageFormat param has priority here. But if the format is undefined, we relly on the alpha channel
        // 图片的格式参数有优先级，但是如果格式是未定义的，我们我有透明通道
        if (imageFormat != SDImageFormatUndefined) {
            usePNG = (imageFormat == SDImageFormatPNG);
        }
        // 如果image是PNG格式，就是用UIImagePNGRepresentation将其转化为NSData，否则按照JPEG格式转化，并且压缩质量为1，即无压缩
        if (usePNG) {
            imageData = UIImagePNGRepresentation(self);
        } else {
            imageData = UIImageJPEGRepresentation(self, (CGFloat)1.0);
        }
#else
        NSBitmapImageFileType imageFileType = NSJPEGFileType;
        if (imageFormat == SDImageFormatGIF) {
            imageFileType = NSGIFFileType;
        } else if (imageFormat == SDImageFormatPNG) {
            imageFileType = NSPNGFileType;
        }
        
        imageData = [NSBitmapImageRep representationOfImageRepsInArray:self.representations
                                                             usingType:imageFileType
                                                            properties:@{}];
#endif
    }
    return imageData;
}


@end
