 //
//  LJDownLoadManager.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "LJDownLoadManager.h"
#import "LJDownLoader.h"
#import "NSString+LJMD5.h"
@interface LJDownLoadManager()
@property (nonatomic, strong) NSMutableDictionary <NSString *, LJDownLoader *>*downLoadInfoDic;
@end

@implementation LJDownLoadManager
static LJDownLoadManager *_shareInstance;
+ (instancetype)shareInstance {
    if (!_shareInstance) {
        _shareInstance = [[LJDownLoadManager alloc] init];
    }
    return _shareInstance;
}

+ (instancetype)allocWithZone:(struct _NSZone *)zone {
    if (!_shareInstance) {
        static dispatch_once_t onceToken;
        dispatch_once(&onceToken, ^{
            _shareInstance = [super allocWithZone:zone];
        });
    }
    return _shareInstance;
}

- (NSMutableDictionary *)downLoadInfoDic {
    if (!_downLoadInfoDic) {
        _downLoadInfoDic = [NSMutableDictionary dictionary];
    }
    return _downLoadInfoDic;
}

- (void)downLoadWithURL:(NSURL *)url {
    [self downLoadWithURL:url success:nil fail:nil];
}

- (void)downLoadWithURL:(NSURL *)url success:(LJDownLoadSucessBlock)success fail:(LJDownLoadFailBlock)fail {
    [self downLoadWithURL:url success:success progress:nil fail:fail];
}

- (void)downLoadWithURL:(NSURL *)url success:(LJDownLoadSucessBlock)success progress:(LJDownLoadProgressBlock)progress fail:(LJDownLoadFailBlock)fail {
    NSString *md5 = [url.absoluteString md5Str];
    LJDownLoader *downLoader = self.downLoadInfoDic[md5];
    if (downLoader) {
        [downLoader resume];
        return;
    }
    downLoader = [[LJDownLoader alloc] init];
    self.downLoadInfoDic[md5] = downLoader;
    __weak __typeof(self)wself = self;
    [downLoader downLoadWithURL:url downLoadInfo:nil progress:^(float progressFloat) {
        if (progress) {
            progress(progressFloat);
        }
    } downLoadSuccess:^(NSString *filePath) {
        NSLog(@"infodic----%@", [NSThread currentThread]);
        [wself.downLoadInfoDic removeObjectForKey:md5];
        if (success) {
            success(filePath);
        }
    } downLoadFail:^(NSError *error){
        [wself.downLoadInfoDic removeObjectForKey:md5];
        if (fail) {
            fail(error);
        }
    }];
}

- (void)pauseWithURL:(NSURL *)url {
    NSString *md5 = [url.absoluteString md5Str];
    LJDownLoader *downLoader = self.downLoadInfoDic[md5];
    [downLoader pause];
}

- (void)cancelWithURL:(NSURL *)url {
    NSString *md5 = [url.absoluteString md5Str];
    LJDownLoader *downLoader = self.downLoadInfoDic[md5];
    [downLoader cancel];
}

- (void)pauseAll {
    [[self.downLoadInfoDic allValues] performSelector:@selector(pause)];
}


@end
