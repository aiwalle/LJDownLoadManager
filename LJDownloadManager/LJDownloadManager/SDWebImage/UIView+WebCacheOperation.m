/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import "UIView+WebCacheOperation.h"

#if SD_UIKIT || SD_MAC

#import "objc/runtime.h"

static char loadOperationKey;

typedef NSMutableDictionary<NSString *, id> SDOperationsDictionary;

@implementation UIView (WebCacheOperation)

// 返回一个字典，存放的是operation对象
- (SDOperationsDictionary *)operationDictionary {
    SDOperationsDictionary *operations = objc_getAssociatedObject(self, &loadOperationKey);
    if (operations) {
        return operations;
    }
    operations = [NSMutableDictionary dictionary];
    // 以下这段代码等价于给UIView添加了@property (nonatomic, strong) NSMutableDictionary *loadOperationKey;
    // 且UIView的实例对象loadOperationKey为operations
    objc_setAssociatedObject(self, &loadOperationKey, operations, OBJC_ASSOCIATION_RETAIN_NONATOMIC);
    return operations;
}

// 这里很疑惑为什么要取消掉对应的operation，因为一个view对象只需要保证有一个图片的下载进程就可以了
// 如果现在要先设置a.png再设置p.png到一个视图上，这里存在一个覆盖问题（如果先下载的b，再下载的a,这里的下载都是异步的，那么很可能a会覆盖b，但是我们的目的是想将b设置到视图上，这显然没有达到我们的要求.......eg:cell中imageView的复用问题尤其经典），因此这里会先取消掉之前视图对象绑定的下载任务，用新的任务（operation）来进行绑定
// 这里的key，是之前有下载任务就会生成的key，这里的operatin是刚刚产生的operation
- (void)sd_setImageLoadOperation:(nullable id)operation forKey:(nullable NSString *)key {
    if (key) {
        [self sd_cancelImageLoadOperationWithKey:key];
        // ⚠️这里是operation1,与下面的operation2不是一个operation
        if (operation) {
            SDOperationsDictionary *operationDictionary = [self operationDictionary];
            operationDictionary[key] = operation;
        }
    }
}

// 这里根据传入的key，取出当前key对应的operation
- (void)sd_cancelImageLoadOperationWithKey:(nullable NSString *)key {
    // Cancel in progress downloader from queue
    // operation的序列
    SDOperationsDictionary *operationDictionary = [self operationDictionary];
    // ⚠️这里是operation2,与上面的operation1不是一个operation
    id operations = operationDictionary[key];
    if (operations) {
        if ([operations isKindOfClass:[NSArray class]]) {
            for (id <SDWebImageOperation> operation in operations) {
                if (operation) {
                    [operation cancel];
                }
            }
        } else if ([operations conformsToProtocol:@protocol(SDWebImageOperation)]){
            [(id<SDWebImageOperation>) operations cancel];
        }
        [operationDictionary removeObjectForKey:key];
    }
}

- (void)sd_removeImageLoadOperationWithKey:(nullable NSString *)key {
    if (key) {
        SDOperationsDictionary *operationDictionary = [self operationDictionary];
        [operationDictionary removeObjectForKey:key];
    }
}

@end

#endif
