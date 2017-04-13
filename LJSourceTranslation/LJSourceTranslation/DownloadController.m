//
//  DownloadController.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "DownloadController.h"
#import "LJDownLoadManager.h"
#import "NSString+LJMD5.h"
#import "UIView+Animation.h"
@interface DownloadController ()<NSURLSessionDelegate>
@property (weak, nonatomic) IBOutlet UIProgressView *progressView;
@property (weak, nonatomic) IBOutlet UIProgressView *progressView2;

@property (nonatomic, copy) NSString *url1;
@property (nonatomic, copy) NSString *url2;


@property (nonatomic, copy) NSString *cacheFilePath;

@property (nonatomic, copy) NSString *tempFilePath;
@property (nonatomic, strong) NSURLSession *session;
@property (nonatomic, weak) NSURLSessionDownloadTask *dataTask;
@property (nonatomic, strong) UIImageView *imageView;
@end

@implementation DownloadController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
//    [self testCode];
    
    
    self.imageView = [UIImageView new];
    [self.imageView lj_setImageWithURL:[NSURL URLWithString:@"https://www.baidu.com/img/bd_logo1.png"] placeholderImage:nil];
}




- (void)testCode {
//    NSString *urlStr = @"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg";
    
    NSString *urlStr = @"http://120.25.226.186:32812/resources/images/minion_08.png";
    NSURL *url = [NSURL URLWithString:urlStr];
    
    // 最终的下载地址
    self.cacheFilePath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES).firstObject stringByAppendingString:url.lastPathComponent];
    // 临时文件地址
    self.tempFilePath = [NSTemporaryDirectory() stringByAppendingString:[url.absoluteString md5Str]];
    //
    //    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    
    NSString *identifier = [NSString stringWithFormat:@"%@.LJDownLoad", [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleIdentifier"]];
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration backgroundSessionConfigurationWithIdentifier:identifier];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration delegate:self delegateQueue:[NSOperationQueue mainQueue]];
    self.session = session;
//    NSURLSessionDownloadTask *task = [self.session downloadTaskWithURL:url];
    
    
    NSURLSessionDownloadTask *task = [self.session downloadTaskWithURL:url completionHandler:^(NSURL * _Nullable location, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        NSString *full = [self.cacheFilePath stringByAppendingPathComponent:response.suggestedFilename];
        
        [[NSFileManager defaultManager] moveItemAtURL:location toURL:[NSURL URLWithString:full] error:nil];
        
        NSLog(@"%@--full", full);
    }];
    
    self.dataTask = task;
    [self.dataTask resume];
    
}





- (IBAction)star:(id)sender {
    self.url1 = @"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg";
    
    [[LJDownLoadManager shareInstance] downLoadWithURL:[NSURL URLWithString:self.url1] success:^(NSString *filePath) {
        NSLog(@"filePatch111111==%@", filePath);
    } progress:^(float progressFloat) {
        self.progressView.progress = progressFloat;
    } fail:^(NSError *error){
        NSLog(@"下载失败-%@", error.userInfo);
    }];

}

- (IBAction)continue:(id)sender {
    [[LJDownLoadManager shareInstance] downLoadWithURL:[NSURL URLWithString:self.url1]];
    NSLog(@"=================继续111111===================");
}

- (IBAction)pause:(id)sender {
    [[LJDownLoadManager shareInstance] pauseWithURL:[NSURL URLWithString:self.url1]];
    NSLog(@"===================暂停11111===================");
}

- (IBAction)cancel:(id)sender {
    [[LJDownLoadManager shareInstance] cancelWithURL:[NSURL URLWithString:self.url1]];
    NSLog(@"===================取消1111===================");
}

- (IBAction)startWithUrl2:(id)sender {
    self.url2 = @"http://m5.pc6.com/cjh5/thunder305.dmg";
    
    [[LJDownLoadManager shareInstance] downLoadWithURL:[NSURL URLWithString:self.url2] success:^(NSString *filePath) {
        NSLog(@"filePatch222222==%@", filePath);
    } progress:^(float progressFloat) {
        self.progressView2.progress = progressFloat;
    } fail:^(NSError *error){
        NSLog(@"下载失败-%@", error.userInfo);
    }];
}

- (IBAction)continueWithUrl2:(id)sender {
    [[LJDownLoadManager shareInstance] downLoadWithURL:[NSURL URLWithString:self.url2]];
    NSLog(@"=================继续222222===================");
}

- (IBAction)pauseWithUrl2:(id)sender {
    [[LJDownLoadManager shareInstance] pauseWithURL:[NSURL URLWithString:self.url2]];
    NSLog(@"===================暂停22222===================");
}

- (IBAction)cancelWithUrl2:(id)sender {
    [[LJDownLoadManager shareInstance] cancelWithURL:[NSURL URLWithString:self.url2]];
    NSLog(@"===================取消2222===================");
}

@end
