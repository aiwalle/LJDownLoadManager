//
//  LJDownLoadManager.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "LJDownLoader.h"
@interface LJDownLoadManager : NSObject
/** 创建单例*/
/**
 创建单例

 @return 返回LJDownLoadManager对象
 */
+ (instancetype)shareInstance;

/**
 从指定url下载文件

 @param url url地址
 */
- (void)downLoadWithURL:(NSURL *)url;

/**
 从指定url下载文件

 @param url url地址
 @param success 成功回调
 @param fail 失败回调
 */
- (void)downLoadWithURL:(NSURL *)url success:(LJDownLoadSucessBlock)success fail:(LJDownLoadFailBlock)fail;

/**
 从指定url下载文件

 @param url url地址
 @param success 成功回调
 @param progress 进程回调
 @param fail 失败回调
 */
- (void)downLoadWithURL:(NSURL *)url success:(LJDownLoadSucessBlock)success progress:(LJDownLoadProgressBlock)progress fail:(LJDownLoadFailBlock)fail;

/**
 暂停对应url的下载

 @param url url地址
 */
- (void)pauseWithURL:(NSURL *)url;


/**
 取消对应url的下载

 @param url url地址
 */
- (void)cancelWithURL:(NSURL *)url;

/**
 暂停所有下载
 */
- (void)pauseAll;
@end
