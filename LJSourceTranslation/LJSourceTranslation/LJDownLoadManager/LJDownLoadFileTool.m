//
//  LJDownLoadFileTool.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "LJDownLoadFileTool.h"

@implementation LJDownLoadFileTool
+ (BOOL)isFileExists:(NSString *)path {
    return [[NSFileManager defaultManager] fileExistsAtPath:path];
}


+ (long long)fileSizeWithPath:(NSString *)path {
    if (![self isFileExists:path]) {
        return 0;
    }
    NSDictionary *fileInfo = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:nil];
    long long size = [fileInfo[NSFileSize] longLongValue];
    return size;
}

+ (void)moveFile:(NSString *)fromPath toPath:(NSString *)toPath {
    if (![self isFileExists:fromPath]) {
        return;
    }
    [[NSFileManager defaultManager] moveItemAtPath:fromPath toPath:toPath error:nil];
}

+ (void)removeFileAtPath:(NSString *)path {
    if (![self isFileExists:path]) {
        return;
    }
    [[NSFileManager defaultManager] removeItemAtPath:path error:nil];
}
@end
