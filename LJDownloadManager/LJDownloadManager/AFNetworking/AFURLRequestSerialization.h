// AFURLRequestSerialization.h
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
#import <TargetConditionals.h>

#if TARGET_OS_IOS || TARGET_OS_TV
#import <UIKit/UIKit.h>
#elif TARGET_OS_WATCH
#import <WatchKit/WatchKit.h>
#endif

NS_ASSUME_NONNULL_BEGIN

/**
 Returns a percent-escaped string following RFC 3986 for a query string key or value.
 RFC 3986 states that the following characters are "reserved" characters.
 - General Delimiters: ":", "#", "[", "]", "@", "?", "/"
 - Sub-Delimiters: "!", "$", "&", "'", "(", ")", "*", "+", ",", ";", "="

 In RFC 3986 - Section 3.4, it states that the "?" and "/" characters should not be escaped to allow
 query strings to include a URL. Therefore, all "reserved" characters with the exception of "?" and "/"
 should be percent-escaped in the query string.
 
 @param string The string to be percent-escaped.
 
 @return The percent-escaped string.
 
    返回查询字符串键或值的RFC 3986后的百分比转义字符串
    RFC 3986 声明以下字符是保留字符
    通用分割符
    子类分割符
    在RFC3986中，它声明了？和/字符不允许被转义
    查询字符串包括一个URL，因此除了?和/以外的所有转义字符，在查询字符串中应该被百分比转义
    string 将要被百分比转义的字符串
    返回百分比转义后的字符串
 
 */
FOUNDATION_EXPORT NSString * AFPercentEscapedStringFromString(NSString *string);

/**
 A helper method to generate encoded url query parameters for appending to the end of a URL.

 @param parameters A dictionary of key/values to be encoded.

 @return A url encoded query string
 
    一个有用的方法来生成编码后的url查询阐述在URL的末尾
    参数  一个要被编码的键值对字典
    返回一个URL编码查询字符串
 */


FOUNDATION_EXPORT NSString * AFQueryStringFromParameters(NSDictionary *parameters);

/**
 The `AFURLRequestSerialization` protocol is adopted by an object that encodes parameters for a specified HTTP requests. Request serializers may encode parameters as query strings, HTTP bodies, setting the appropriate HTTP header fields as necessary.

 For example, a JSON request serializer may set the HTTP body of the request to a JSON representation, and set the `Content-Type` HTTP header field value to `application/json`.
 
    AFURLRequestSerialization 协议是 一个指定的HTTP请求编码参数的对象采用。如果有必要的话，请求序列也许会编码阐述作为请求字符串，HTTP响应体，设置合适的HTTP头文件
    例如，一个JSON请求序列化对象也会回设置HTTP请求体来呈现JSON，设置Content-Type的HTTP头的值为application/json
 
 */
@protocol AFURLRequestSerialization <NSObject, NSSecureCoding, NSCopying>

/**
 Returns a request with the specified parameters encoded into a copy of the original request.

 @param request The original request.
 @param parameters The parameters to be encoded.
 @param error The error that occurred while attempting to encode the request parameters.

 @return A serialized request.
 
    通过指定的参数编码来生成一个copy原始请求的新请求
    
    原始的请求
    需要被编码的参数
    当尝试编码这个请求参数的时候产生的错误信息
    返回一个序列化后的请求
 */
- (nullable NSURLRequest *)requestBySerializingRequest:(NSURLRequest *)request
                               withParameters:(nullable id)parameters
                                        error:(NSError * _Nullable __autoreleasing *)error NS_SWIFT_NOTHROW;

@end

#pragma mark -

/**

 */
typedef NS_ENUM(NSUInteger, AFHTTPRequestQueryStringSerializationStyle) {
    AFHTTPRequestQueryStringDefaultStyle = 0,
};

@protocol AFMultipartFormData;

/**
 `AFHTTPRequestSerializer` conforms to the `AFURLRequestSerialization` & `AFURLResponseSerialization` protocols, offering a concrete base implementation of query string / URL form-encoded parameter serialization and default request headers, as well as response status code and content type validation.

 Any request or response serializer dealing with HTTP is encouraged to subclass `AFHTTPRequestSerializer` in order to ensure consistent default behavior.
 
    AFHTTPRequestSerializer 遵照 AFURLRequestSerialization 和 AFURLResponseSerialization 协议，提供了一个具体的基本实现，通过请求字符串URL形式编码参数序列，默认的请求头，响应状态和内容类型的确认。
    任何的请求和响应序列化对象都应该子类化AFHTTPRequestSerializer，来确保一致性的默认行为。
 */
@interface AFHTTPRequestSerializer : NSObject <AFURLRequestSerialization>

/**
 The string encoding used to serialize parameters. `NSUTF8StringEncoding` by default.
 
    序列化的参数，默认是NSUTF8StringEncoding
 */
@property (nonatomic, assign) NSStringEncoding stringEncoding;

/**
 Whether created requests can use the device’s cellular radio (if present). `YES` by default.

 @see NSMutableURLRequest -setAllowsCellularAccess:
 
    是否创建请求通过设备的蜂窝网络（如果有），默认情况下是yes
 */
@property (nonatomic, assign) BOOL allowsCellularAccess;

/**
 The cache policy of created requests. `NSURLRequestUseProtocolCachePolicy` by default.

 @see NSMutableURLRequest -setCachePolicy:
 
    创建请求的缓存策略，NSURLRequestUseProtocolCachePolicy是默认的
 */
@property (nonatomic, assign) NSURLRequestCachePolicy cachePolicy;

/**
 Whether created requests should use the default cookie handling. `YES` by default.

 @see NSMutableURLRequest -setHTTPShouldHandleCookies:
 
    创建请求是够使用默认的cookie协议，默认是yes
 */
@property (nonatomic, assign) BOOL HTTPShouldHandleCookies;

/**
 Whether created requests can continue transmitting data before receiving a response from an earlier transmission. `NO` by default

 @see NSMutableURLRequest -setHTTPShouldUsePipelining:
 
    HTTPShouldUsePipelining表示receiver(理解为iOS客户端)的下一个信息是否必须等到上一个请求回复才能发送。
    如果为YES表示可以，NO表示必须等receiver收到先前的回复才能发送下个信息
 */
@property (nonatomic, assign) BOOL HTTPShouldUsePipelining;

/**
 The network service type for created requests. `NSURLNetworkServiceTypeDefault` by default.

 @see NSMutableURLRequest -setNetworkServiceType:
 
    设定request的network service类型. 默认是`NSURLNetworkServiceTypeDefault`.
    这个network service是为了告诉系统网络层这个request使用的目的
    比如NSURLNetworkServiceTypeVoIP表示的就这个request是用来请求网际协议通话技术(Voice over IP)。
    系统能根据提供的信息来优化网络处理，从而优化电池寿命，网络性能等等
 */
@property (nonatomic, assign) NSURLRequestNetworkServiceType networkServiceType;

/**
 The timeout interval, in seconds, for created requests. The default timeout interval is 60 seconds.

 @see NSMutableURLRequest -setTimeoutInterval:
    
    超时的机制，默认情况下为60s
 */
@property (nonatomic, assign) NSTimeInterval timeoutInterval;

///---------------------------------------
/// @name Configuring HTTP Request Headers
///---------------------------------------

/**
 Default HTTP header field values to be applied to serialized requests. By default, these include the following:

 - `Accept-Language` with the contents of `NSLocale +preferredLanguages`
 - `User-Agent` with the contents of various bundle identifiers and OS designations

 @discussion To add or remove default request headers, use `setValue:forHTTPHeaderField:`.
 
    默认的HTTP头文件值来序列化请求。默认情况下，包含下面的信息
    接受的语言   本地的语言偏好
    用户代理 bundle id和系统名称
 
    通过setValue:forHTTPHeaderField: 来添加或者移除默认的请求头
 */
@property (readonly, nonatomic, strong) NSDictionary <NSString *, NSString *> *HTTPRequestHeaders;

/**
 Creates and returns a serializer with default configuration.
 
    通过默认的配置来创建和返回一个序列化对象
 */
+ (instancetype)serializer;

/**
 Sets the value for the HTTP headers set in request objects made by the HTTP client. If `nil`, removes the existing value for that header.

 @param field The HTTP header to set a default value for
 @param value The value set as default for the specified header, or `nil`
 
    在HTTP客户端的请求对象里设置HTTp请求头，如果为nil，那么会移除处在的值
    http头部来设置默认的值
    设置特定的请求，或者为nil
 */
- (void)setValue:(nullable NSString *)value
forHTTPHeaderField:(NSString *)field;

/**
 Returns the value for the HTTP headers set in the request serializer.

 @param field The HTTP header to retrieve the default value for

 @return The value set as default for the specified header, or `nil`
 
    返回在请求序列化中HTTP头的值
    同上
 */
- (nullable NSString *)valueForHTTPHeaderField:(NSString *)field;

/**
 Sets the "Authorization" HTTP header set in request objects made by the HTTP client to a basic authentication value with Base64-encoded username and password. This overwrites any existing value for this header.

 @param username The HTTP basic auth username
 @param password The HTTP basic auth password
 
    设置http头的授权信息，通过base64位编码的用户名和密码来设置http客户端的授权值。这个值可以在任何头存在的地方重写
 */
- (void)setAuthorizationHeaderFieldWithUsername:(NSString *)username
                                       password:(NSString *)password;

/**
 Clears any existing value for the "Authorization" HTTP header.
    
    清除所有的http头授权信息。
 */
- (void)clearAuthorizationHeader;

///-------------------------------------------------------
/// @name Configuring Query String Parameter Serialization
///-------------------------------------------------------

/**
 HTTP methods for which serialized requests will encode parameters as a query string. `GET`, `HEAD`, and `DELETE` by default.
 
    序列化请求的http方法通过传入字符串的方式来对请求的字符串进行编码作用。
 */
@property (nonatomic, strong) NSSet <NSString *> *HTTPMethodsEncodingParametersInURI;

/**
 Set the method of query string serialization according to one of the pre-defined styles.

 @param style The serialization style.

 @see AFHTTPRequestQueryStringSerializationStyle
 
    根据一个预定义的风格来设置请求字符串序列化的方法
 */
- (void)setQueryStringSerializationWithStyle:(AFHTTPRequestQueryStringSerializationStyle)style;

/**
 Set the a custom method of query string serialization according to the specified block.

 @param block A block that defines a process of encoding parameters into a query string. This block returns the query string and takes three arguments: the request, the parameters to encode, and the error that occurred when attempting to encode parameters for the given request.
 
    设置一个自定义的请求字符串序列化的方法根据特定的block
    
    block,一个定义了编码参数到请求字符串的进程block。这个block返回了请求的字符串和三个参数：请求，对应的参数，当通过参数来解析对应的请求时发生的错误信息
 */
- (void)setQueryStringSerializationWithBlock:(nullable NSString * (^)(NSURLRequest *request, id parameters, NSError * __autoreleasing *error))block;

///-------------------------------
/// @name Creating Request Objects
///-------------------------------

/**
 Creates an `NSMutableURLRequest` object with the specified HTTP method and URL string.

 If the HTTP method is `GET`, `HEAD`, or `DELETE`, the parameters will be used to construct a url-encoded query string that is appended to the request's URL. Otherwise, the parameters will be encoded according to the value of the `parameterEncoding` property, and set as the request body.

 @param method The HTTP method for the request, such as `GET`, `POST`, `PUT`, or `DELETE`. This parameter must not be `nil`.
 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be either set as a query string for `GET` requests, or the request HTTP body.
 @param error The error that occurred while constructing the request.

 @return An `NSMutableURLRequest` object.
 
    创建了一个可改变的NSMutableURLRequest对象通过指定的HTTP方法和URL字符串
    如果HTTP方法是`GET`, `HEAD`, or `DELETE`，这个参数会被用来构造一个基于给定URL的url编码请求字符串。另一方面，这个参数会依据parameterEncoding属性来编码和设置请求体。
    
    http请求方式
    创建url的Url字符串
    请求参数
    错误信息
 */
- (NSMutableURLRequest *)requestWithMethod:(NSString *)method
                                 URLString:(NSString *)URLString
                                parameters:(nullable id)parameters
                                     error:(NSError * _Nullable __autoreleasing *)error;

/**
 Creates an `NSMutableURLRequest` object with the specified HTTP method and URLString, and constructs a `multipart/form-data` HTTP body, using the specified parameters and multipart form data block. See http://www.w3.org/TR/html4/interact/forms.html#h-17.13.4.2

 Multipart form requests are automatically streamed, reading files directly from disk along with in-memory data in a single HTTP body. The resulting `NSMutableURLRequest` object has an `HTTPBodyStream` property, so refrain from setting `HTTPBodyStream` or `HTTPBody` on this request object, as it will clear out the multipart form body stream.

 @param method The HTTP method for the request. This parameter must not be `GET` or `HEAD`, or `nil`.
 @param URLString The URL string used to create the request URL.
 @param parameters The parameters to be encoded and set in the request HTTP body.
 @param block A block that takes a single argument and appends data to the HTTP body. The block argument is an object adopting the `AFMultipartFormData` protocol.
 @param error The error that occurred while constructing the request.

 @return An `NSMutableURLRequest` object
 
    创建了一个可改变的NSMutableURLRequest对象通过指定的HTTP方法和URL字符串，构造一个“表单信息”的http请求体，使用特定的参数和多部件的表格数据block
 
    多部件的表单请求会自动流式传输，通过磁盘和内存的数据来读取文件。这个NSMutableURLRequest对象的结果有一个HTTPBodyStream，，因此禁止设置HTTPBodyStream和HTTPBody在请求对象，否则会清空多部件表格流体
 
    block,（这里只翻译block），有一个参数，在http请求提上加载数据，这个block的参数是一个遵循了AFMultipartFormData协议的对象
 */
- (NSMutableURLRequest *)multipartFormRequestWithMethod:(NSString *)method
                                              URLString:(NSString *)URLString
                                             parameters:(nullable NSDictionary <NSString *, id> *)parameters
                              constructingBodyWithBlock:(nullable void (^)(id <AFMultipartFormData> formData))block
                                                  error:(NSError * _Nullable __autoreleasing *)error;

/**
 Creates an `NSMutableURLRequest` by removing the `HTTPBodyStream` from a request, and asynchronously writing its contents into the specified file, invoking the completion handler when finished.

 @param request The multipart form request. The `HTTPBodyStream` property of `request` must not be `nil`.
 @param fileURL The file URL to write multipart form contents to.
 @param handler A handler block to execute.

 @discussion There is a bug in `NSURLSessionTask` that causes requests to not send a `Content-Length` header when streaming contents from an HTTP body, which is notably problematic when interacting with the Amazon S3 webservice. As a workaround, this method takes a request constructed with `multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:error:`, or any other request with an `HTTPBodyStream`, writes the contents to the specified file and returns a copy of the original request with the `HTTPBodyStream` property set to `nil`. From here, the file can either be passed to `AFURLSessionManager -uploadTaskWithRequest:fromFile:progress:completionHandler:`, or have its contents read into an `NSData` that's assigned to the `HTTPBody` property of the request.

 @see https://github.com/AFNetworking/AFNetworking/issues/1398
    
    通过移除请求中的http请求提流来床架一个NSMutableURLRequest对象，在定制文件异步写入内容，当完成时会调用对应block
    多部件请求，请求的HTTPBodyStream属性必须为空
    将多部分表单内容写入的文件URL
    执行完毕的block
    // 这里有一个bug
 */
- (NSMutableURLRequest *)requestWithMultipartFormRequest:(NSURLRequest *)request
                             writingStreamContentsToFile:(NSURL *)fileURL
                                       completionHandler:(nullable void (^)(NSError * _Nullable error))handler;

@end

#pragma mark -

/**
 The `AFMultipartFormData` protocol defines the methods supported by the parameter in the block argument of `AFHTTPRequestSerializer -multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:`.
 
    AFMultipartFormData协议定义了支持AFHTTPRequestSerializer -multipartFormRequestWithMethod:URLString:parameters:constructingBodyWithBlock:方法的参数
 */
@protocol AFMultipartFormData

/**
 Appends the HTTP header `Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`, followed by the encoded file data and the multipart form boundary.

 The filename and MIME type for this data in the form will be automatically generated, using the last path component of the `fileURL` and system associated MIME type for the `fileURL` extension, respectively.

 @param fileURL The URL corresponding to the file whose content will be appended to the form. This parameter must not be `nil`.
 @param name The name to be associated with the specified data. This parameter must not be `nil`.
 @param error If an error occurs, upon return contains an `NSError` object that describes the problem.

 @return `YES` if the file data was successfully appended, otherwise `NO`.
 
    附加了HTTP头`Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`，依据编码的文件数据和多部件表单
    对应的数据的文件名和MIME类型会自动生成，分别使用`fileURL`的最后一个路径组件和`fileURL`扩展的系统相关MIME类型。
    fileURL:    文件内容会被附加到表格，这个URL对应文件的地址，参数不能为空
    name:       指定数据相关的名称，这个参数不能为空
    error:      如果错误产生了，会返回个NSError对象来描述问题
    如果返回yes，文件数据成功被附件，否则为NO
 */
- (BOOL)appendPartWithFileURL:(NSURL *)fileURL
                         name:(NSString *)name
                        error:(NSError * _Nullable __autoreleasing *)error;

/**
 Appends the HTTP header `Content-Disposition: file; filename=#{filename}; name=#{name}"` and `Content-Type: #{mimeType}`, followed by the encoded file data and the multipart form boundary.

 @param fileURL The URL corresponding to the file whose content will be appended to the form. This parameter must not be `nil`.
 @param name The name to be associated with the specified data. This parameter must not be `nil`.
 @param fileName The file name to be used in the `Content-Disposition` header. This parameter must not be `nil`.
 @param mimeType The declared MIME type of the file data. This parameter must not be `nil`.
 @param error If an error occurs, upon return contains an `NSError` object that describes the problem.

 @return `YES` if the file data was successfully appended otherwise `NO`.
 
    附加了HTTP头`Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`，依据编码的文件数据和多部件表单
 
    fileURL:    文件内容会被附加到表格，这个URL对应文件的地址，参数不能为空
    name:       指定数据相关的名称，这个参数不能为空
    error:      如果错误产生了，会返回个NSError对象来描述问题
    mimetype:   文件数据的声明MIME类型对象，这个参数不能为空
    如果返回yes，文件数据成功被附件，否则为NO
 
 */
- (BOOL)appendPartWithFileURL:(NSURL *)fileURL
                         name:(NSString *)name
                     fileName:(NSString *)fileName
                     mimeType:(NSString *)mimeType
                        error:(NSError * _Nullable __autoreleasing *)error;

/**
 Appends the HTTP header `Content-Disposition: file; filename=#{filename}; name=#{name}"` and `Content-Type: #{mimeType}`, followed by the data from the input stream and the multipart form boundary.

 @param inputStream The input stream to be appended to the form data
 @param name The name to be associated with the specified input stream. This parameter must not be `nil`.
 @param fileName The filename to be associated with the specified input stream. This parameter must not be `nil`.
 @param length The length of the specified input stream in bytes.
 @param mimeType The MIME type of the specified data. (For example, the MIME type for a JPEG image is image/jpeg.) For a list of valid MIME types, see http://www.iana.org/assignments/media-types/. This parameter must not be `nil`.
 
 附加了HTTP头`Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`，依据编码的文件数据和多部件表单
 
 inputStream： 加入到表单数据中的输入流
 name:       指定输入流相关的名称，这个参数不能为空
 fileName:   指定输入流相关的文件名称，不能为空
 length：     指定输入流的长度byte
 mimetype:   文件数据的声明MIME类型对象，这个参数不能为空
 */
- (void)appendPartWithInputStream:(nullable NSInputStream *)inputStream
                             name:(NSString *)name
                         fileName:(NSString *)fileName
                           length:(int64_t)length
                         mimeType:(NSString *)mimeType;

/**
 Appends the HTTP header `Content-Disposition: file; filename=#{filename}; name=#{name}"` and `Content-Type: #{mimeType}`, followed by the encoded file data and the multipart form boundary.

 @param data The data to be encoded and appended to the form data.
 @param name The name to be associated with the specified data. This parameter must not be `nil`.
 @param fileName The filename to be associated with the specified data. This parameter must not be `nil`.
 @param mimeType The MIME type of the specified data. (For example, the MIME type for a JPEG image is image/jpeg.) For a list of valid MIME types, see http://www.iana.org/assignments/media-types/. This parameter must not be `nil`.
 
 附加了HTTP头`Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`，依据编码的文件数据和多部件表单
 
 data:       被编码和附加到表单数据的数据
 name:       指定数据相关的名称，这个参数不能为空
 fileName:   指定输入流相关的文件名称，不能为空
 mimetype:   文件数据的声明MIME类型对象，这个参数不能为空
 */
- (void)appendPartWithFileData:(NSData *)data
                          name:(NSString *)name
                      fileName:(NSString *)fileName
                      mimeType:(NSString *)mimeType;

/**
 Appends the HTTP headers `Content-Disposition: form-data; name=#{name}"`, followed by the encoded data and the multipart form boundary.

 @param data The data to be encoded and appended to the form data.
 @param name The name to be associated with the specified data. This parameter must not be `nil`.
 
 附加了HTTP头`Content-Disposition: file; filename=#{generated filename}; name=#{name}"` and `Content-Type: #{generated mimeType}`，依据编码的文件数据和多部件表单
 
 data:       被编码和附加到表单数据的数据
 name:       指定数据相关的名称，这个参数不能为空
 */

- (void)appendPartWithFormData:(NSData *)data
                          name:(NSString *)name;


/**
 Appends HTTP headers, followed by the encoded data and the multipart form boundary.

 @param headers The HTTP headers to be appended to the form data.
 @param body The data to be encoded and appended to the form data. This parameter must not be `nil`.
 
 附加HTTP标头，后跟编码数据和多部分表单边界。
 headers:   加入到表单数据的HTTP头
 body:      被编码和附加到表单数据的数据。不能为空
 */
- (void)appendPartWithHeaders:(nullable NSDictionary <NSString *, NSString *> *)headers
                         body:(NSData *)body;

/**
 Throttles request bandwidth by limiting the packet size and adding a delay for each chunk read from the upload stream.

 When uploading over a 3G or EDGE connection, requests may fail with "request body stream exhausted". Setting a maximum packet size and delay according to the recommended values (`kAFUploadStream3GSuggestedPacketSize` and `kAFUploadStream3GSuggestedDelay`) lowers the risk of the input stream exceeding its allocated bandwidth. Unfortunately, there is no definite way to distinguish between a 3G, EDGE, or LTE connection over `NSURLConnection`. As such, it is not recommended that you throttle bandwidth based solely on network reachability. Instead, you should consider checking for the "request body stream exhausted" in a failure block, and then retrying the request with throttled bandwidth.

 @param numberOfBytes Maximum packet size, in number of bytes. The default packet size for an input stream is 16kb.
 @param delay Duration of delay each time a packet is read. By default, no delay is set.
 
    节流阀请求带宽，通过限制数据包大小并为从上传流读取的每个块添加延迟来请求带宽
    当通过3G或EDGE连接上传时，请求可能会失败，并显示“请求正文流耗尽”。 根据推荐值（`kAFUploadStream3GSuggestedPacketSize`和`kAFUploadStream3GSuggestedDelay'）设置最大数据包大小和延迟可降低输入流超出其分配带宽的风险。 不幸的是，没有确定的方法来区分通过NSURLConnection的3G，EDGE或LTE连接。 因此，不建议仅基于网络可达性来限制带宽。 相反，您应该考虑检查故障阻塞中的“请求正文流耗尽”，然后使用节制的带宽重试请求。
 */
- (void)throttleBandwidthWithPacketSize:(NSUInteger)numberOfBytes
                                  delay:(NSTimeInterval)delay;

@end

#pragma mark -

/**
 `AFJSONRequestSerializer` is a subclass of `AFHTTPRequestSerializer` that encodes parameters as JSON using `NSJSONSerialization`, setting the `Content-Type` of the encoded request to `application/json`.
 
    AFJSONRequestSerializer是AFHTTPRequestSerializer的子类，使用NSJSONSerialization将json最为编码参数，设置编码请求的Content-Type为application/json
 */
@interface AFJSONRequestSerializer : AFHTTPRequestSerializer

/**
 Options for writing the request JSON data from Foundation objects. For possible values, see the `NSJSONSerialization` documentation section "NSJSONWritingOptions". `0` by default.
 
    从Foundation对象中写入请求JSON数据的选项。 有关可能的值，请参见“NSJSONSerialization”文档一节“NSJSONWritingOptions”。 `0`默认
 */
@property (nonatomic, assign) NSJSONWritingOptions writingOptions;

/**
 Creates and returns a JSON serializer with specified reading and writing options.

 @param writingOptions The specified JSON writing options.
 
    通过特定的读写选项，来创建返回一个JSON序列化对象
    writingOptions: 特定的JSON写入选项
 */
+ (instancetype)serializerWithWritingOptions:(NSJSONWritingOptions)writingOptions;

@end

#pragma mark -

/**
 `AFPropertyListRequestSerializer` is a subclass of `AFHTTPRequestSerializer` that encodes parameters as JSON using `NSPropertyListSerializer`, setting the `Content-Type` of the encoded request to `application/x-plist`.
 
    AFPropertyListRequestSerializer是AFHTTPRequestSerializer的子类，使用NSPropertyListSerializer将json最为编码参数，设置编码请求的Content-Type为application/x-plist
 */
@interface AFPropertyListRequestSerializer : AFHTTPRequestSerializer

/**
 The property list format. Possible values are described in "NSPropertyListFormat".
 
 属性清单的格式，参看NSPropertyListFormat
 */
@property (nonatomic, assign) NSPropertyListFormat format;

/**
 @warning The `writeOptions` property is currently unused.
 
    警告：writeOptions属性当前未使用
 */
@property (nonatomic, assign) NSPropertyListWriteOptions writeOptions;

/**
 Creates and returns a property list serializer with a specified format, read options, and write options.

 @param format The property list format.
 @param writeOptions The property list write options.

 @warning The `writeOptions` property is currently unused.
 
 通过特定的格式，读写选项，来创建返回一个属性列表序列化对象
 format: 属性列表的格式
 writeOptions: 属性列表的写入选项
 警告：writeOptions属性当前未使用
 */
+ (instancetype)serializerWithFormat:(NSPropertyListFormat)format
                        writeOptions:(NSPropertyListWriteOptions)writeOptions;

@end

#pragma mark -

///----------------
/// @name Constants
///----------------

/**
 ## Error Domains

 The following error domain is predefined.

 - `NSString * const AFURLRequestSerializationErrorDomain`

 ### Constants

 `AFURLRequestSerializationErrorDomain`
 AFURLRequestSerializer errors. Error codes for `AFURLRequestSerializationErrorDomain` correspond to codes in `NSURLErrorDomain`.
 
 预定义以下错误域
 */
FOUNDATION_EXPORT NSString * const AFURLRequestSerializationErrorDomain;

/**
 ## User info dictionary keys

 These keys may exist in the user info dictionary, in addition to those defined for NSError.

 - `NSString * const AFNetworkingOperationFailingURLRequestErrorKey`

 ### Constants

 `AFNetworkingOperationFailingURLRequestErrorKey`
 The corresponding value is an `NSURLRequest` containing the request of the operation associated with an error. This key is only present in the `AFURLRequestSerializationErrorDomain`.
 */
FOUNDATION_EXPORT NSString * const AFNetworkingOperationFailingURLRequestErrorKey;

/**
 ## Throttling Bandwidth for HTTP Request Input Streams

 @see -throttleBandwidthWithPacketSize:delay:

 ### Constants

 `kAFUploadStream3GSuggestedPacketSize`
 Maximum packet size, in number of bytes. Equal to 16kb.

 `kAFUploadStream3GSuggestedDelay`
 Duration of delay each time a packet is read. Equal to 0.2 seconds.
 */
FOUNDATION_EXPORT NSUInteger const kAFUploadStream3GSuggestedPacketSize;
FOUNDATION_EXPORT NSTimeInterval const kAFUploadStream3GSuggestedDelay;

NS_ASSUME_NONNULL_END
