//
//  LJDownLoadFileTool.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface LJDownLoadFileTool : NSObject

/**
 文件地址是否存在对应文件

 @param path 文件地址
 @return 是否存在
 */
+ (BOOL)isFileExists:(NSString *)path;

/**
 获取指定路径的文件大小

 @param path 文件路径
 @return 文件代销
 */
+ (long long)fileSizeWithPath:(NSString *)path;

/**
 将文件移动到指定位置

 @param fromPath 需要移动的地址
 @param toPath 目的地址
 */
+ (void)moveFile:(NSString *)fromPath toPath:(NSString *)toPath;

/**
 移除指定位置的文件

 @param path 文件地址
 */
+ (void)removeFileAtPath:(NSString *)path;

@end
