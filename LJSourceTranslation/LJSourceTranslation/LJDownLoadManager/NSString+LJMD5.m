//
//  NSString+LJMD5.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/28.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "NSString+LJMD5.h"
#import <CommonCrypto/CommonDigest.h>

@implementation NSString (LJMD5)
- (NSString *)md5Str {
    const char *data = self.UTF8String;
    unsigned char digest[CC_MD5_DIGEST_LENGTH];
    CC_MD5(data, (CC_LONG)strlen(data), digest);
    NSMutableString *result = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH];
    for (int i = 0; i < CC_MD5_DIGEST_LENGTH; i ++) {
        [result appendFormat:@"%02x", digest[i]];
    }
    return result;
}
@end
