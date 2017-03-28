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
@property (nonatomic, strong) LJDownLoadManager *downloadManager;
@end

@implementation DownloadController

- (LJDownLoadManager *)downloadManager {
    if (!_downloadManager) {
        _downloadManager = [LJDownLoadManager new];
    }
    return _downloadManager;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    
}

- (IBAction)star:(id)sender {
//    LJDownLoadManager *manager = [LJDownLoadManager new];
//    [manager downLoadWithURL:[NSURL URLWithString:@"http://m5.pc6.com/cjh5/thunder305.dmg"]];
    

    [self.downloadManager downLoadWithURL:[NSURL URLWithString:@"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg"]];
}

- (IBAction)continue:(id)sender {
    [self.downloadManager resume];
    NSLog(@"=================继续===================");
}

- (IBAction)pause:(id)sender {
    [self.downloadManager pause];
    NSLog(@"===================暂停===================");
}

- (IBAction)cancel:(id)sender {
    [self.downloadManager cancel];
    NSLog(@"===================取消===================");
}
@end
