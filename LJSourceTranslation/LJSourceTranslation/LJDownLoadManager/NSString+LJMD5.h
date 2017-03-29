//
//  NSString+LJMD5.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/28.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (LJMD5)

/**
 md5加密后的字符串
 
 @return 需要加密的字符串
 */
- (NSString *)md5Str;
@end
