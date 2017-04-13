//
//  ViewController.m
//  LJDownloadManager
//
//  Created by liang on 16/10/25.
//  Copyright © 2016年 liang. All rights reserved.
//

#import "ViewController.h"
#import "SDWebImage/UIImageView+WebCache.h"
#import "UIView+Animation.h"
#import "DownloadController.h"

#import "NSString+LJMD5.h"
@interface ViewController ()<NSURLSessionDelegate>

@property (weak, nonatomic) IBOutlet UIImageView *imageView;
@property (nonatomic, copy) NSString *cacheFilePath;

@property (nonatomic, copy) NSString *tempFilePath;
@property (nonatomic, strong) NSURLSession *session;
@property (nonatomic, weak) NSURLSessionDownloadTask *dataTask;
@end

@implementation ViewController




- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSString *urlStr = @"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg";
    NSURL *url = [NSURL URLWithString:urlStr];
    
    // 最终的下载地址
    self.cacheFilePath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject stringByAppendingString:url.lastPathComponent];
    // 临时文件地址
    self.tempFilePath = [NSTemporaryDirectory() stringByAppendingString:[url.absoluteString md5Str]];
//
//    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:urlStr];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration delegate:self delegateQueue:[NSOperationQueue mainQueue]];
    self.session = session;
    NSURLSessionDownloadTask *task = [self.session downloadTaskWithURL:url];
    self.dataTask = task;
    [self.dataTask resume];
}

- (void)testCode {
    NSString *urlStr = @"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg";
    NSURL *url = [NSURL URLWithString:urlStr];
    
    // 最终的下载地址
    self.cacheFilePath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject stringByAppendingString:url.lastPathComponent];
    // 临时文件地址
    self.tempFilePath = [NSTemporaryDirectory() stringByAppendingString:[url.absoluteString md5Str]];
    //
    //    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:urlStr];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration delegate:self delegateQueue:[NSOperationQueue mainQueue]];
    self.session = session;
    NSURLSessionDownloadTask *task = [self.session downloadTaskWithURL:url];
    self.dataTask = task;
    [self.dataTask resume];
    
}


@end
