//
//  LJDownLoader.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/29.
//  Copyright © 2017年 liang. All rights reserved.
//

#import <Foundation/Foundation.h>
typedef NS_ENUM(NSInteger, LJDownLoadStatus) {
    LJDownLoadStatusUnknown,
    /** 下载暂停 */
    LJDownLoadStatusPause,
    /** 正在下载 */
    LJDownLoadStatusDownLoading,
    /** 已经下载 */
    LJDownLoadStatusSuccess,
    /** 下载失败 */
    LJDownLoadStatusFailed
};

typedef void(^LJDownLoadInfoBlock)(long long fileSize);
typedef void(^LJDownLoadProgressBlock)(float progressFloat);
typedef void(^LJDownLoadSucessBlock)(NSString *filePath);
typedef void(^LJDownLoadFailBlock)(NSError *error);
@interface LJDownLoader : NSObject
@property (nonatomic, copy) LJDownLoadInfoBlock infoBlock;
@property (nonatomic, copy) LJDownLoadProgressBlock progressBlock;
@property (nonatomic, copy) LJDownLoadSucessBlock successBlock;
@property (nonatomic, copy) LJDownLoadFailBlock failBlock;

@property (nonatomic, assign, readonly) LJDownLoadStatus downLoadStatus;
@property (nonatomic, assign, readonly) float progress;

// 状态改变的block
@property (nonatomic, copy) void(^downLoadStateChange)(LJDownLoadStatus status);
// 文件下载进度
//@property (nonatomic, copy)  void(^downLoadProgress)(float progress);
- (void)downLoadWithURL:(NSURL *)url;

- (void)downLoadWithURL:(NSURL *)url downLoadInfo:(LJDownLoadInfoBlock)Info downLoadSuccess:(LJDownLoadSucessBlock)success downLoadFail:(LJDownLoadFailBlock)fail;
- (void)downLoadWithURL:(NSURL *)url downLoadInfo:(LJDownLoadInfoBlock)Info progress:(LJDownLoadProgressBlock)progress downLoadSuccess:(LJDownLoadSucessBlock)success downLoadFail:(LJDownLoadFailBlock)fail;

// 恢复
- (void)resume;
// 暂停
- (void)pause;
// 取消
- (void)cancel;
// 取消并清除缓存
- (void)cancelAndClearCache;
@end
