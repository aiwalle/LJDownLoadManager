//
//  DownloadController.m
//  LJSourceTranslation
//
//  Created by liang on 17/3/27.
//  Copyright © 2017年 liang. All rights reserved.
//

#import "DownloadController.h"
#import "LJDownLoadManager.h"
@interface DownloadController ()

@end

@implementation DownloadController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
//    LJDownLoadManager *manager = [LJDownLoadManager new];
//    [manager downLoadWithURL:[NSURL URLWithString:@"http://m5.pc6.com/cjh5/thunder305.dmg"]];
    
    
    LJDownLoadManager *manager2 = [LJDownLoadManager new];
    
    [manager2 downLoadWithURL:[NSURL URLWithString:@"http://issuecdn.baidupcs.com/issue/netdisk/MACguanjia/BaiduNetdisk_mac_2.1.0.dmg"]];
}



@end
