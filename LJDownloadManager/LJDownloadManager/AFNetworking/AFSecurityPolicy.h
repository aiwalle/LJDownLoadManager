// AFSecurityPolicy.h
// Copyright (c) 2011–2016 Alamofire Software Foundation ( http://alamofire.org/ )
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import <Foundation/Foundation.h>
#import <Security/Security.h>

typedef NS_ENUM(NSUInteger, AFSSLPinningMode) {
    AFSSLPinningModeNone,
    //表示不做SSL pinning，只跟浏览器一样在系统的信任机构列表里验证服务端返回的证书。若证书是信任机构签发的就会通过，若是自己服务器生成的证书，这里是不会通过的。
    AFSSLPinningModePublicKey,
    //表示用证书绑定方式验证证书，需要客户端保存有服务端的证书拷贝，这里验证分两步，第一步验证证书的域名/有效期等信息，第二步是对比服务端返回的证书跟客户端返回的是否一致
    AFSSLPinningModeCertificate,
    //这个模式同样是用证书绑定方式验证，客户端要有服务端的证书拷贝，只是验证时只验证证书里的公钥，不验证证书的有效期等信息
};

/**
 `AFSecurityPolicy` evaluates server trust against pinned X.509 certificates and public keys over secure connections.

 Adding pinned SSL certificates to your app helps prevent man-in-the-middle attacks and other vulnerabilities. Applications dealing with sensitive customer data or financial information are strongly encouraged to route all communication over an HTTPS connection with SSL pinning configured and enabled.
 
 AFSecurityPolicy 通过安全连接评估服务器对固定证书X.509和公钥的信任
 
 添加固定的SSL证书到你的APP帮助防止中间人攻击，和一些别的漏洞，强烈建议处理敏感客户数据或财务信息的应用程序通过配置和启用SSL绑定的HTTPS连接路由所有通信。
 */

NS_ASSUME_NONNULL_BEGIN

@interface AFSecurityPolicy : NSObject <NSSecureCoding, NSCopying>

/**
 The criteria by which server trust should be evaluated against the pinned SSL certificates. Defaults to `AFSSLPinningModeNone`.
 
 服务器信任的评估SSL证书的基本模式，默认情况下是AFSSLPinningModeNone
 */
@property (readonly, nonatomic, assign) AFSSLPinningMode SSLPinningMode;

/**
 The certificates used to evaluate server trust according to the SSL pinning mode. 

  By default, this property is set to any (`.cer`) certificates included in the target compiling AFNetworking. Note that if you are using AFNetworking as embedded framework, no certificates will be pinned by default. Use `certificatesInBundle` to load certificates from your target, and then create a new policy by calling `policyWithPinningMode:withPinnedCertificates`.
 
 Note that if pinning is enabled, `evaluateServerTrust:forDomain:` will return true if any pinned certificate matches.
 
 用于根据SSL锁定模式评估服务器信任的证书
 
 默认情况下，这个属性是被设置成一些包含在target下的.cer的证书集合。注意如果你将AFNetworking作为一个嵌入式的framework，默认情况下是无证书导入的，使用certificatesInBundle来读取工程中的证书，然后通过policyWithPinningMode:withPinnedCertificates来创建一个新的策略。
 */
@property (nonatomic, strong, nullable) NSSet <NSData *> *pinnedCertificates;

/**
 Whether or not to trust servers with an invalid or expired SSL certificates. Defaults to `NO`.
 属性代表是否允许不信任的证书（证书无效、证书时间过期）通过验证 ，默认为NO.
 */
@property (nonatomic, assign) BOOL allowInvalidCertificates;

/**
 Whether or not to validate the domain name in the certificate's CN field. Defaults to `YES`.
  是否验证域名证书的CN(common name)字段。默认值为YES。
 */
@property (nonatomic, assign) BOOL validatesDomainName;

///-----------------------------------------
/// @name Getting Certificates from the Bundle   在目录下获取证书
///-----------------------------------------

/**
 Returns any certificates included in the bundle. If you are using AFNetworking as an embedded framework, you must use this method to find the certificates you have included in your app bundle, and use them when creating your security policy by calling `policyWithPinningMode:withPinnedCertificates`.

 @return The certificates included in the given bundle.
 
 返回目录下包含的所有.cer文件，如果你将AFNetworking作为一个嵌入框架来使用，你必须使用policyWithPinningMode:withPinnedCertificates来找到你当前app下的证书，人后用他们创建你的安全策略
 */
+ (NSSet <NSData *> *)certificatesInBundle:(NSBundle *)bundle;

///-----------------------------------------
/// @name Getting Specific Security Policies   获得特定的安全策略
///-----------------------------------------

/**
 Returns the shared default security policy, which does not allow invalid certificates, validates domain name, and does not validate against pinned certificates or public keys.

 @return The default security policy.
 
 返回一个默认的安全策略：不允许使用无效证书、验证域名CN、不验证绑定的证书和公钥。
 */
+ (instancetype)defaultPolicy;

///---------------------
/// @name Initialization    初始化
///---------------------

/**
 Creates and returns a security policy with the specified pinning mode.

 @param pinningMode The SSL pinning mode.

 @return A new security policy.
 
 通过特定的pinning mode 来创建和返回一个安全策略
 */
+ (instancetype)policyWithPinningMode:(AFSSLPinningMode)pinningMode;

/**
 Creates and returns a security policy with the specified pinning mode.

 @param pinningMode The SSL pinning mode.
 @param pinnedCertificates The certificates to pin against.

 @return A new security policy.
 
 通过特定的pinning mode 来创建和返回一个安全策略，和证书
 */
+ (instancetype)policyWithPinningMode:(AFSSLPinningMode)pinningMode withPinnedCertificates:(NSSet <NSData *> *)pinnedCertificates;

///------------------------------
/// @name Evaluating Server Trust   服务端信任评估
///------------------------------

/**
 Whether or not the specified server trust should be accepted, based on the security policy.

 This method should be used when responding to an authentication challenge from a server.

 @param serverTrust The X.509 certificate trust of the server.
 @param domain The domain of serverTrust. If `nil`, the domain will not be validated.

 @return Whether or not to trust the server.
 
    核心类  当服务器响应提出进行证书验证时，此方法将会被调用。然后app根据之前设置的验证策略来进行判断验证是否通过。
    根据severTrust和domain来检查服务器端发来的证书是否可信
    其中SecTrustRef是一个CoreFoundation类型，用于对服务器端传来的X.509证书评估的
    而我们都知道，数字证书的签发机构CA，在接收到申请者的资料后进行核对并确定信息的真实有效，然后就会制作一份符合X.509标准的文件。证书中的证书内容包含的持有者信息和公钥等都是由申请者提供的，而数字签名则是CA机构对证书内容进行hash加密后得到的，而这个数字签名就是我们验证证书是否是有可信CA签发的数据。
 */
- (BOOL)evaluateServerTrust:(SecTrustRef)serverTrust
                  forDomain:(nullable NSString *)domain;

@end

NS_ASSUME_NONNULL_END

///----------------
/// @name Constants
///----------------

/**
 ## SSL Pinning Modes

 The following constants are provided by `AFSSLPinningMode` as possible SSL pinning modes.

 enum {
 AFSSLPinningModeNone,
 AFSSLPinningModePublicKey,
 AFSSLPinningModeCertificate,
 }

 `AFSSLPinningModeNone`
 Do not used pinned certificates to validate servers.

 `AFSSLPinningModePublicKey`
 Validate host certificates against public keys of pinned certificates.

 `AFSSLPinningModeCertificate`
 Validate host certificates against pinned certificates.
*/
