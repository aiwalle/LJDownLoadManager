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
@interface ViewController ()

@property (weak, nonatomic) IBOutlet UIImageView *imageView;

@end

@implementation ViewController
- (IBAction)push:(id)sender {
    
    [self.navigationController pushViewController:[DownloadController new] animated:YES];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    //    [self.imageView sd_setImageWithURL:[NSURL URLWithString:@"https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png"] placeholderImage:[UIImage imageNamed:@"placeholder"]];
    
    self.imageView.backgroundColor = [UIColor grayColor];
    
    //    __weak typeof(self) weakSelf = self;
    //    [self.imageView sd_setImageWithURL:[NSURL URLWithString:@"https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png"] placeholderImage:[UIImage imageNamed:@"placeholder"] options:SDWebImageAvoidAutoSetImage completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
    //        [weakSelf.imageView crossDissolveAnimationForWebImageWithType:cacheType image:image];
    //    }];
    
//    [self.imageView lj_setImageWithURL:[NSURL URLWithString:@"https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png"] placeholderImage:[UIImage imageNamed:@"placeholder"]];
    

    
    
}

- (void)testCode {
    //    [[SDWebImageManager sharedManager] setCacheKeyFilter:^(NSURL *url) {
    //        url = [[NSURL alloc] initWithScheme:url.scheme host:url.host path:url.path];
    //        return [url absoluteString];
    //    }];
    //
    //
    //    [self.imageView sd_setImageWithURL:[NSURL URLWithString:@"http://www.httpwatch.com/httpgallery/authentication/authenticatedimage/default.aspx?0.35786508303135633"] completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
    //
    //    }];
    //
    //
    //    [self.imageView sd_setImageWithURL:[NSURL URLWithString:@"http://www.httpwatch.com/httpgallery/authentication/authenticatedimage/default.aspx?0.35786508303135633"] placeholderImage:nil options:SDWebImageLowPriority&SDWebImageCacheMemoryOnly progress:^(NSInteger receivedSize, NSInteger expectedSize, NSURL * _Nullable targetURL) {
    //
    //    } completed:^(UIImage * _Nullable image, NSError * _Nullable error, SDImageCacheType cacheType, NSURL * _Nullable imageURL) {
    //        
    //    }];
    
    
}


@end
