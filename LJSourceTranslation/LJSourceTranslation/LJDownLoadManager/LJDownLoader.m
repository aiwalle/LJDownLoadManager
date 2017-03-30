//
//  LJDownLoader.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/29.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "LJDownLoader.h"
#import "LJDownLoadFileTool.h"
#import "NSString+LJMD5.h"
#define LJCacheDir NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject
#define LJTempDir NSTemporaryDirectory()

@interface LJDownLoader()<NSURLSessionDelegate, NSURLSessionDataDelegate>
{
    long long _tempFileSize;
    long long _totalFileSize;
}
@property (nonatomic, copy) NSString *cacheFilePath;

@property (nonatomic, copy) NSString *tempFilePath;

@property (nonatomic, strong) NSURLSession *session;

@property (nonatomic, strong) NSOutputStream *outputStream;
@property (nonatomic, strong) NSURL *url;
//@property (nonatomic, strong) NSURL *url;
//@property (nonatomic, strong) NSOperationQueue *queue;
@property (nonatomic, weak) NSURLSessionDataTask *dataTask;
@property (nonatomic, strong) NSOperationQueue *queue;
@end

@implementation LJDownLoader
- (NSURLSession *)session {
    if (!_session) {
        NSURLSessionConfiguration *config = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:[self.url.absoluteString md5Str]];
        _queue = [[NSOperationQueue alloc] init];
        _queue.maxConcurrentOperationCount = 1;
        // TODO:队列问题还没解决
        _session = [NSURLSession sessionWithConfiguration:config delegate:self delegateQueue:_queue];
    }
    return _session;
}

- (void)setDownLoadStatus:(LJDownLoadStatus)downLoadStatus {
    if (_downLoadStatus == downLoadStatus) {
        return;
    }
    _downLoadStatus = downLoadStatus;
    if (self.downLoadStateChange) {
        self.downLoadStateChange(downLoadStatus);
    }
}

- (void)setProgress:(float)progress {
    _progress = progress;
    if (self.progressBlock) {
        self.progressBlock(progress);
    }
}

- (void)downLoadWithURL:(NSURL *)url downLoadInfo:(LJDownLoadInfoBlock)Info downLoadSuccess:(LJDownLoadSucessBlock)success downLoadFail:(LJDownLoadFailBlock)fail {
    self.infoBlock = Info;
    self.successBlock = success;
    self.failBlock = fail;
    [self downLoadWithURL:url];
}

- (void)downLoadWithURL:(NSURL *)url downLoadInfo:(LJDownLoadInfoBlock)Info progress:(LJDownLoadProgressBlock)progress downLoadSuccess:(LJDownLoadSucessBlock)success downLoadFail:(LJDownLoadFailBlock)fail {
    self.infoBlock = Info;
    self.progressBlock = progress;
    self.successBlock = success;
    self.failBlock = fail;
    [self downLoadWithURL:url];
}

- (void)downLoadWithURL:(NSURL *)url {
    _url = url;
    // 最终的下载地址
    self.cacheFilePath = [LJCacheDir stringByAppendingString:url.lastPathComponent];
    // 临时文件地址
    self.tempFilePath = [LJTempDir stringByAppendingString:[url.absoluteString md5Str]];
    
    // 看文件是否已经下载好
    if ([LJDownLoadFileTool isFileExists:self.cacheFilePath]) {
        self.downLoadStatus = LJDownLoadStatusSuccess;
        NSLog(@"该文件已存在");
        
        if (self.infoBlock) {
            self.infoBlock([LJDownLoadFileTool fileSizeWithPath:self.cacheFilePath]);
        }
        self.downLoadStatus = LJDownLoadStatusSuccess;
        if (self.successBlock) {
            self.successBlock(self.cacheFilePath);
        }
        return;
    }
    // 如果已经存在这个下载的URL，返回
    if ([url isEqual:self.dataTask.originalRequest.URL]) {
        // 如果暂停状态，恢复下载
        if (self.downLoadStatus == LJDownLoadStatusPause) {
            [self resume];
            return;
        }
        // 如果正在下载，返回
        if (self.downLoadStatus == LJDownLoadStatusDownLoading) {
            return;
        }
        
    }
    
    [self cancel];
    // 计算临时文件的大小
    _tempFileSize = [LJDownLoadFileTool fileSizeWithPath:self.tempFilePath];
    // 开始下载
    [self downLoadWithURL:url offset:_tempFileSize];
}

// 恢复
// 如果你调用了两次suspend，就需要调用两次resume来继续
- (void)resume {
    if (self.downLoadStatus == LJDownLoadStatusPause) {
        [self.dataTask resume];
        self.downLoadStatus = LJDownLoadStatusDownLoading;
    }
}

// 暂停
// 如果你调用了两次resume，就需要调用两次suspend来暂停
- (void)pause {
    if (self.downLoadStatus == LJDownLoadStatusDownLoading) {
        [self.dataTask suspend];
        self.downLoadStatus = LJDownLoadStatusPause;
    }
}

// 取消
- (void)cancel {
    [self.session invalidateAndCancel];
    self.session = nil;
}

// 取消并清除缓存
- (void)cancelAndClearCache {
    [self cancel];
    [LJDownLoadFileTool removeFileAtPath:self.tempFilePath];
}

- (void)downLoadWithURL:(NSURL *)url offset:(long long)offset {
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url];
    [request setValue:[NSString stringWithFormat:@"bytes=%lld-", offset] forHTTPHeaderField:@"Range"];
    NSURLSessionDataTask *dataTask = [self.session dataTaskWithRequest:request];
    [dataTask resume];
    self.dataTask = dataTask;
}

#pragma mark - NSURLSessionDataDelegate
// 当收到响应的时候调用
// 如果超时 也会受到响应
- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveResponse:(NSURLResponse *)response completionHandler:(void (^)(NSURLSessionResponseDisposition))completionHandler {
//    NSLog(@"respon==%@", response);
    NSLog(@"tread---%@---url:%@", [NSThread currentThread], dataTask.originalRequest.URL);
    
    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
    // 获取到文件的大小
    _totalFileSize = [httpResponse.allHeaderFields[@"Content-Length"] longLongValue];
    // 上面获取到的文件可能不准确
    if (httpResponse.allHeaderFields[@"Content-Range"]) {
        NSString *rangeStr = httpResponse.allHeaderFields[@"Content-Range"];
        _totalFileSize = [[rangeStr componentsSeparatedByString:@"/"].lastObject longLongValue];
    }
    
    if (self.infoBlock) {
        self.infoBlock(_totalFileSize);
    }
    
    // 文件已经下载完成
    if (_tempFileSize == _totalFileSize) {
        NSLog(@"文件下载完毕，移到cache文件");
        [LJDownLoadFileTool moveFile:self.tempFilePath toPath:self.cacheFilePath];
        self.downLoadStatus = LJDownLoadStatusSuccess;
        completionHandler(NSURLSessionResponseCancel);
        return;
    }
    
    // 临时文件存在错误
    if (_tempFileSize > _totalFileSize) {
        NSLog(@"文件有错误，重新下载");
        [LJDownLoadFileTool removeFileAtPath:self.tempFilePath];
        completionHandler(NSURLSessionResponseCancel);
        [self downLoadWithURL:response.URL offset:0];
        return;
    }
    
    NSLog(@"继续下载文件");
    self.downLoadStatus = LJDownLoadStatusDownLoading;
    self.outputStream = [NSOutputStream outputStreamToFileAtPath:self.tempFilePath append:YES];
    [self.outputStream open];
    // 传入NSURLSessionResponseAllow，表示允许继续下载，如果不传入将终止下载
    completionHandler(NSURLSessionResponseAllow);
}

- (void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data {
    //    NSLog(@"正常接收数据中");
//    _tempFileSize += data.length;
//    self.progress = 1.0 * _tempFileSize / _totalFileSize;
    NSLog(@"tread---%@---url:%@", [NSThread currentThread], dataTask.originalRequest.URL);
    dispatch_async(dispatch_get_main_queue(), ^{
        _tempFileSize += data.length;
        self.progress = 1.0 * _tempFileSize / _totalFileSize;
    });
    
    //    NSLog(@"tread2222222---%@---%@", [NSThread currentThread], _url);
    [self.outputStream write:data.bytes maxLength:data.length];
}

- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error {
    NSLog(@"tread33333---%@", [NSThread currentThread]);
    [self.outputStream close];
    self.outputStream = nil;
    if (error) {
        self.downLoadStatus = LJDownLoadStatusFailed;
        NSLog(@"Error==%@", error.userInfo);
        if (self.failBlock) {
            self.failBlock(error);
        }
    } else {
        NSLog(@"文件正常下载成功了");
        if (self.successBlock) {
            self.successBlock(self.cacheFilePath);
        }
        self.downLoadStatus = LJDownLoadStatusSuccess;
        [LJDownLoadFileTool moveFile:self.tempFilePath toPath:self.cacheFilePath];
    }
}


@end
