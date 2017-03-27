//
//  LJDownLoadFileTool.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LJDownLoadFileTool : NSObject

+ (BOOL)isFileExists:(NSString *)path;

+ (long long)fileSizeWithPath:(NSString *)path;

+ (void)moveFile:(NSString *)fromPath toPath:(NSString *)toPath;

+ (void)removeFileAtPath:(NSString *)path;

@end
