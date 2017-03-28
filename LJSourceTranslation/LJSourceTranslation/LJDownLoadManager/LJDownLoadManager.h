//
//  LJDownLoadManager.h
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
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
typedef void(^LJDownLoadSucessBlock)(NSString *filePath);
typedef void(^LJDownLoadFailBlock)();

@interface LJDownLoadManager : NSObject
@property (nonatomic, copy) LJDownLoadInfoBlock info;
@property (nonatomic, copy) LJDownLoadSucessBlock success;
@property (nonatomic, copy) LJDownLoadFailBlock fail;
@property (nonatomic, assign, readonly) LJDownLoadStatus downLoadStatus;

- (void)downLoadWithURL:(NSURL *)url;

- (void)downLoadWithURL:(NSURL *)url downLoadInfo:(LJDownLoadInfoBlock)Info downLoadSuccess:(LJDownLoadSucessBlock)success downLoadFail:(LJDownLoadFailBlock)fail;
// 恢复
- (void)resume;
// 暂停
- (void)pause;
// 取消
- (void)cancel;
// 取消并清除缓存
- (void)cancelAndClearCache;

@end
