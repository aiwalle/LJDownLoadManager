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
@interface DownloadController ()
@property (weak, nonatomic) IBOutlet UIProgressView *progressView;
@property (weak, nonatomic) IBOutlet UIProgressView *progressView2;

@property (nonatomic, copy) NSString *url1;
@property (nonatomic, copy) NSString *url2;
@end

@implementation DownloadController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
}

- (IBAction)star:(id)sender {
    self.url1 = @"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg";
    
    [[LJDownLoadManager shareInstance] downLoadWithURL:[NSURL URLWithString:self.url1] success:^(NSString *filePath) {
        NSLog(@"filePatch111111==%@", filePath);
    } progress:^(float progressFloat) {
        self.progressView.progress = progressFloat;
    } fail:^{
        NSLog(@"下载失败111111");
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
    } fail:^{
        NSLog(@"下载失败2222222");
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
