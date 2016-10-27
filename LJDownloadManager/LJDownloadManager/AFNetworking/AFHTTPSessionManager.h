// AFHTTPSessionManager.h
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
#if !TARGET_OS_WATCH
#import <SystemConfiguration/SystemConfiguration.h>
#endif
#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_WATCH || TARGET_OS_TV
#import <MobileCoreServices/MobileCoreServices.h>
#else
#import <CoreServices/CoreServices.h>
#endif

#import "AFURLSessionManager.h"

/**
 `AFHTTPSessionManager` is a subclass of `AFURLSessionManager` with convenience methods for making HTTP requests. When a `baseURL` is provided, requests made with the `GET` / `POST` / et al. convenience methods can be made with relative paths.

    AFHTTPSessionManager 是AFURLSessionManager的子类，通过便利方法来进行HTTP的请求，当一个基本的URL被提供，请求可以通过GET/POST等方式，便利方法可以通过相关的路径来生成。
 
 ## Subclassing Notes

 Developers targeting iOS 7 or Mac OS X 10.9 or later that deal extensively with a web service are encouraged to subclass `AFHTTPSessionManager`, providing a class method that returns a shared singleton object on which authentication and other configuration can be shared across the application.

    开发者的工程要求大于等于iOS7且Mac OS X 大于等于10.9，使用AFHTTPSessionManager来广泛的处理网络服务，提供了一个shared singleton的类方法来生成单例对象，在程序运行期间进行创建或配置对象。
 
 For developers targeting iOS 6 or Mac OS X 10.8 or earlier, `AFHTTPRequestOperationManager` may be used to similar effect.

    开发者的工程要求小鱼iOS6且Mac OS X 大于小于10.8，AFHTTPRequestOperationManager可用于类似的方式。
 
 
 ## Methods to Override 重写的方法

 To change the behavior of all data task operation construction, which is also used in the `GET` / `POST` / et al. convenience methods, override `dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:`.
 
    改变所有数据任务操作的行为，也同样使用GET/POST等方法。
    便利方法，重写dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 
 ## Serialization 序列化

 Requests created by an HTTP client will contain default headers and encode parameters according to the `requestSerializer` property, which is an object conforming to `<AFURLRequestSerialization>`.
 
    被客户端创建的HTTP请求包含默认的请求头，和编码参数，通过设置requestSerializer属性，它是一个AFURLRequestSerialization对象。

 Responses received from the server are automatically validated and serialized by the `responseSerializers` property, which is an object conforming to `<AFURLResponseSerialization>`
    
    从服务端接收到的响应会自动验证，通过responseSerializers属性来进行序列化，他是一个AFURLResponseSerialization对象。

 ## URL Construction Using Relative Paths   URL的建立相关的路径

 For HTTP convenience methods, the request serializer constructs URLs from the path relative to the `-baseURL`, using `NSURL +URLWithString:relativeToURL:`, when provided. If `baseURL` is `nil`, `path` needs to resolve to a valid `NSURL` object using `NSURL +URLWithString:`.
 
    为了HTTP便利方法，当baseURL设定了，请求的序列化结构依据baseURL进行关联，使用NSURL +URLWithString:relativeToURL:，如果baseURL为空，路径的解决方式是提供一个可用的NSURL对象，使用NSURL +URLWithString:。

 Below are a few examples of how `baseURL` and relative paths interact:
 下面是一些例子是baseURL如何关联路径来相互作用的：

    下面的路径就不翻译了，就是路径的拼接。
    NSURL *baseURL = [NSURL URLWithString:@"http://example.com/v1/"];
    [NSURL URLWithString:@"foo" relativeToURL:baseURL];                  // http://example.com/v1/foo
    [NSURL URLWithString:@"foo?bar=baz" relativeToURL:baseURL];          // http://example.com/v1/foo?bar=baz
    [NSURL URLWithString:@"/foo" relativeToURL:baseURL];                 // http://example.com/foo
    [NSURL URLWithString:@"foo/" relativeToURL:baseURL];                 // http://example.com/v1/foo
    [NSURL URLWithString:@"/foo/" relativeToURL:baseURL];                // http://example.com/foo/
    [NSURL URLWithString:@"http://example2.com/" relativeToURL:baseURL]; // http://example2.com/

 Also important to note is that a trailing slash will be added to any `baseURL` without one. This would otherwise cause unexpected behavior when constructing URLs using paths without a leading slash.

    同样重要需要注明的是，最后的斜杠需要被添加在baseURL，不要忘记了。
    如果URL没有斜杠，将会引起不想产生的效果
 
 @warning Managers for background sessions must be owned for the duration of their use. This can be accomplished by creating an application-wide or shared singleton instance.
 
    警告：后台任务的管理者必须在他们使用期间持续拥有，可以通过创建一个全局或者单例来完成。
 */

NS_ASSUME_NONNULL_BEGIN

@interface AFHTTPSessionManager : AFURLSessionManager <NSSecureCoding, NSCopying>

/**
 The URL used to construct requests from relative paths in methods like `requestWithMethod:URLString:parameters:`, and the `GET` / `POST` / et al. convenience methods.
 
    这个URL来构造请求从关联路径，通过方法requestWithMethod:URLString:parameters:，和GET/POST等，便利方法。
 */
@property (readonly, nonatomic, strong, nullable) NSURL *baseURL;

/**
 Requests created with `requestWithMethod:URLString:parameters:` & `multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:` are constructed with a set of default headers using a parameter serialization specified by this property. By default, this is set to an instance of `AFHTTPRequestSerializer`, which serializes query string parameters for `GET`, `HEAD`, and `DELETE` requests, or otherwise URL-form-encodes HTTP message bodies.
 
    当通过requestWithMethod:URLString:parameters:和multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:来创建请求，使用此属性指定的参数序列化的一组默认标题。
    默认情况下，这个参数是被一个AFHTTPRequestSerializer对象来设置的，这个对象连续请求通过GET/HEAD和DELETE等，或者其他形式的HTTP消息体URL编码。

 @warning `requestSerializer` must not be `nil`.
 
    警告：requestSerializer一定不能为空。
 */
@property (nonatomic, strong) AFHTTPRequestSerializer <AFURLRequestSerialization> * requestSerializer;

/**
 Responses sent from the server in data tasks created with `dataTaskWithRequest:success:failure:` and run using the `GET` / `POST` / et al. convenience methods are automatically validated and serialized by the response serializer. By default, this property is set to an instance of `AFJSONResponseSerializer`.
 
    通过dataTaskWithRequest:success:failure:和使用GET/POST等方法，响应体在数据任务中通过服务端来创建。
    便利方法会自动的验证和序列化响应体，默认情况下，这个属性设置为AFJSONResponseSerializer

 @warning `responseSerializer` must not be `nil`.
 */
@property (nonatomic, strong) AFHTTPResponseSerializer <AFURLResponseSerialization> * responseSerializer;

///-------------------------------
/// @name Managing Security Policy  安全管理策略
///-------------------------------

/**
 The security policy used by created session to evaluate server trust for secure connections. `AFURLSessionManager` uses the `defaultPolicy` unless otherwise specified. A security policy configured with `AFSSLPinningModePublicKey` or `AFSSLPinningModeCertificate` can only be applied on a session manager initialized with a secure base URL (i.e. https). Applying a security policy with pinning enabled on an insecure session manager throws an `Invalid Security Policy` exception.
 
    创建会话的安全策略，来评估服务器对安全连接的信任度。
    AFURLSessionManager通常情况下采取的默认的策略，安全策略通过AFSSLPinningModePublicKey和AFSSLPinningModeCertificate来配置，只能应用在会话管理者初始化一个安全的基础URL（像https）。
    在一个不安全的会话管理应用一个安全策略会抛出异常‘无效的安全策略’。👌
 */
@property (nonatomic, strong) AFSecurityPolicy *securityPolicy;

///---------------------
/// @name Initialization
///---------------------

/**
 Creates and returns an `AFHTTPSessionManager` object.
 
    创建返回一个AFHTTPSessionManager对象
 */
+ (instancetype)manager;

/**
 Initializes an `AFHTTPSessionManager` object with the specified base URL.

 @param url The base URL for the HTTP client.

 @return The newly-initialized HTTP client
 
    初始化AFHTTPSessionManager对象通过指定的根URL。
    url指的是给HTTP客户端设置的根URL
    返回一个新初始化的HTTP客户端
 */
- (instancetype)initWithBaseURL:(nullable NSURL *)url;

/**
 Initializes an `AFHTTPSessionManager` object with the specified base URL.

 This is the designated initializer.

 @param url The base URL for the HTTP client.
 @param configuration The configuration used to create the managed session.

 @return The newly-initialized HTTP client
 
 
    初始化AFHTTPSessionManager对象通过指定的根URL。
    这里是工厂方法
    url指的是给HTTP客户端设置的根URL
    configuration是创建一个回话管理者的配置
    返回一个新初始化的HTTP客户端
 */
- (instancetype)initWithBaseURL:(nullable NSURL *)url
           sessionConfiguration:(nullable NSURLSessionConfiguration *)configuration NS_DESIGNATED_INITIALIZER;

///---------------------------
/// @name Making HTTP Requests
///---------------------------

/**
 Creates and runs an `NSURLSessionDataTask` with a `GET` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
 
    创建了一个NSURLSessionDataTask通过GET请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 */
- (nullable NSURLSessionDataTask *)GET:(NSString *)URLString
                   parameters:(nullable id)parameters
                      success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                      failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;


/**
 Creates and runs an `NSURLSessionDataTask` with a `GET` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param downloadProgress A block object to be executed when the download progress is updated. Note this block is called on the session queue, not the main queue.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 
    创建了一个NSURLSessionDataTask通过GET请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    下载进度的block，当下载的进度更新。注意：这个block在当前会话所在的线程，而不是在主线程。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 
 */
- (nullable NSURLSessionDataTask *)GET:(NSString *)URLString
                            parameters:(nullable id)parameters
                              progress:(nullable void (^)(NSProgress *downloadProgress))downloadProgress
                               success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                               failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a `HEAD` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes a single arguments: the data task.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
    创建了一个NSURLSessionDataTask通过HEAD请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)HEAD:(NSString *)URLString
                    parameters:(nullable id)parameters
                       success:(nullable void (^)(NSURLSessionDataTask *task))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a `POST` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
    创建了一个NSURLSessionDataTask通过POST请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                    parameters:(nullable id)parameters
                       success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 Creates and runs an `NSURLSessionDataTask` with a `POST` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param uploadProgress A block object to be executed when the upload progress is updated. Note this block is called on the session queue, not the main queue.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 
 
    创建了一个NSURLSessionDataTask通过POST请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    下载进度的block，当下载的进度更新。注意：这个block在当前会话所在的线程，而不是在主线程。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a multipart `POST` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param block A block that takes a single argument and appends data to the HTTP body. The block argument is an object adopting the `AFMultipartFormData` protocol.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
    创建了一个NSURLSessionDataTask通过多部件的POST请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    一个block对象，只有一个参数，加入数据到HTTP体中，这个对象是遵循了AFMultipartFormData协议的 👌似乎是提交表单这个block快就是提交form表单用的，他会和【 parameters:(NSDictionary *)parameters】参数一起由AFN合并提交给后台服务器的
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                    parameters:(nullable id)parameters
     constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                       success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                       failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure DEPRECATED_ATTRIBUTE;

/**
 Creates and runs an `NSURLSessionDataTask` with a multipart `POST` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param block A block that takes a single argument and appends data to the HTTP body. The block argument is an object adopting the `AFMultipartFormData` protocol.
 @param uploadProgress A block object to be executed when the upload progress is updated. Note this block is called on the session queue, not the main queue.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 
 
    创建了一个NSURLSessionDataTask通过多部件的POST请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数
    一个block对象，只有一个参数，加入数据到HTTP体中，这个对象是遵循了AFMultipartFormData协议的 👌似乎是提交表单这个block快就是提交form表单用的，他会和【 parameters:(NSDictionary *)parameters】参数一起由AFN合并提交给后台服务器的
    下载进度的block，当下载的进度更新。注意：这个block在当前会话所在的线程，而不是在主线程。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:uploadProgress:downloadProgress:completionHandler:
 */
- (nullable NSURLSessionDataTask *)POST:(NSString *)URLString
                             parameters:(nullable id)parameters
              constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                               progress:(nullable void (^)(NSProgress *uploadProgress))uploadProgress
                                success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                                failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a `PUT` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
    创建了一个NSURLSessionDataTask通过PUT请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PUT:(NSString *)URLString
                   parameters:(nullable id)parameters
                      success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                      failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a `PATCH` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
 
    创建了一个NSURLSessionDataTask通过PATCH请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 */
- (nullable NSURLSessionDataTask *)PATCH:(NSString *)URLString
                     parameters:(nullable id)parameters
                        success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                        failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

/**
 Creates and runs an `NSURLSessionDataTask` with a `DELETE` request.

 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded according to the client request serializer.
 @param success A block object to be executed when the task finishes successfully. This block has no return value and takes two arguments: the data task, and the response object created by the client response serializer.
 @param failure A block object to be executed when the task finishes unsuccessfully, or that finishes successfully, but encountered an error while parsing the response data. This block has no return value and takes a two arguments: the data task and the error describing the network or parsing error that occurred.

 @see -dataTaskWithRequest:completionHandler:
    
    创建了一个NSURLSessionDataTask通过DELETE请求。
    URLStringURLString是用来创建请求的URL
    参数是客户端请求需要的参数。
    一个block对象，当任务成功完成时候执行。这个请求无返回值，有两个参数，一个是数据任务，一个是客户端响应者创建的响应对象
    一个block对象，当任务不成功完成时候执行，或者成功完成，但是在解析响应数据的时候发生错误。这个请求无返回值，有两个参数，一个是数据任务，一个是描述发生的网络错误或解析错误
 
 
    具体实现：-dataTaskWithRequest:completionHandler:
 
 */
- (nullable NSURLSessionDataTask *)DELETE:(NSString *)URLString
                      parameters:(nullable id)parameters
                         success:(nullable void (^)(NSURLSessionDataTask *task, id _Nullable responseObject))success
                         failure:(nullable void (^)(NSURLSessionDataTask * _Nullable task, NSError *error))failure;

@end

NS_ASSUME_NONNULL_END
