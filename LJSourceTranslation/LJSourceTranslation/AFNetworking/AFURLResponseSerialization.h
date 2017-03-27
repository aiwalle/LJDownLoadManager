// AFURLResponseSerialization.h
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
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

/**
 The `AFURLResponseSerialization` protocol is adopted by an object that decodes data into a more useful object representation, according to details in the server response. Response serializers may additionally perform validation on the incoming response and data.

 For example, a JSON response serializer may check for an acceptable status code (`2XX` range) and content type (`application/json`), decoding a valid JSON response into an object.
 
    根据响应服务器的细节，一个对象将遵循AFURLResponseSerialization协议，来将响应体转化为更有用的对象
    比如说，一个JSON响应序列化对象可以检查接受状态吗和内容的种类，将一个有效的响应转为一个对象
 */
@protocol AFURLResponseSerialization <NSObject, NSSecureCoding, NSCopying>

/**
 The response object decoded from the data associated with a specified response.

 @param response The response to be processed.
 @param data The response data to be decoded.
 @param error The error that occurred while attempting to decode the response data.

 @return The object decoded from the specified response data.
    
    响应对象根据一个特定的响应来解码
    需要被处理的response
    需要被解码的data
    当解码响应数据时候发生的错误
    返回从指定响应数据中解码的对象
 */
- (nullable id)responseObjectForResponse:(nullable NSURLResponse *)response
                           data:(nullable NSData *)data
                          error:(NSError * _Nullable __autoreleasing *)error NS_SWIFT_NOTHROW;

@end

#pragma mark -

/**
 `AFHTTPResponseSerializer` conforms to the `AFURLRequestSerialization` & `AFURLResponseSerialization` protocols, offering a concrete base implementation of query string / URL form-encoded parameter serialization and default request headers, as well as response status code and content type validation.

 Any request or response serializer dealing with HTTP is encouraged to subclass `AFHTTPResponseSerializer` in order to ensure consistent default behavior.
 
    AFHTTPResponseSerializer遵循AFURLRequestSerialization和AFURLResponseSerialization协议，提供了一个具体的基础实现通过请求字符串，URL形式编码参数序列化和默认的请求头，在响应状态码和内容形式正确的情况下
    
    任何请求和响应序列化对象处理HTTP通过AFHTTPResponseSerializer,来确保一致的默认行为
 */
@interface AFHTTPResponseSerializer : NSObject <AFURLResponseSerialization>

- (instancetype)init;

@property (nonatomic, assign) NSStringEncoding stringEncoding DEPRECATED_MSG_ATTRIBUTE("The string encoding is never used. AFHTTPResponseSerializer only validates status codes and content types but does not try to decode the received data in any way.");
// 字符串编码从不使用。 AFHTTPResponseSerializer仅验证状态代码和内容类型，但不尝试以任何方式解码接收的数据。
/**
 Creates and returns a serializer with default configuration.
    通过默认的配置来创建返回一个序列化对象
 */
+ (instancetype)serializer;

///-----------------------------------------
/// @name Configuring Response Serialization            配置响应序列化
///-----------------------------------------

/**
 The acceptable HTTP status codes for responses. When non-`nil`, responses with status codes not contained by the set will result in an error during validation.

 See http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
 
    响应的可接受HTTP状态代码。 当非`nil'时，集合不包含的状态代码的响应将导致验证期间的错误。
 */
@property (nonatomic, copy, nullable) NSIndexSet *acceptableStatusCodes;

/**
 The acceptable MIME types for responses. When non-`nil`, responses with a `Content-Type` with MIME types that do not intersect with the set will result in an error during validation.
 
    响应的可接受的MIME types。 当非`nil'时，带有MIME类型的`Content-Type`的响应与集合不相交将导致验证期间的错误。。
 */
@property (nonatomic, copy, nullable) NSSet <NSString *> *acceptableContentTypes;

/**
 Validates the specified response and data.

 In its base implementation, this method checks for an acceptable status code and content type. Subclasses may wish to add other domain-specific checks.

 @param response The response to be validated.
 @param data The data associated with the response.
 @param error The error that occurred while attempting to validate the response.

 @return `YES` if the response is valid, otherwise `NO`.
 
    验证特定的响应和数据
    在基本的实现，这个方法检查接收到的状态码和内容种类，子类可能下网添加特定的域名检查
    
    需要被验证的响应
    响应相关的数据
    当尝试验证响应时发生的错误
 */
- (BOOL)validateResponse:(nullable NSHTTPURLResponse *)response
                    data:(nullable NSData *)data
                   error:(NSError * _Nullable __autoreleasing *)error;

@end

#pragma mark -


/**
 `AFJSONResponseSerializer` is a subclass of `AFHTTPResponseSerializer` that validates and decodes JSON responses.

 By default, `AFJSONResponseSerializer` accepts the following MIME types, which includes the official standard, `application/json`, as well as other commonly-used types:

 - `application/json`
 - `text/json`
 - `text/javascript`
 
    AFJSONResponseSerializer是AFHTTPResponseSerializer的子类,用来验证和解码JSON响应
    默认情况下，AFJSONResponseSerializer接受下面的MIME种类，包含官方标准的application/json以及其他常用的类型
    - `application/json`
    - `text/json`
    - `text/javascript`
 */
@interface AFJSONResponseSerializer : AFHTTPResponseSerializer

- (instancetype)init;

/**
 Options for reading the response JSON data and creating the Foundation objects. For possible values, see the `NSJSONSerialization` documentation section "NSJSONReadingOptions". `0` by default.
    读取JSON响应数据和创建基础对象的选项，默认情况下是0
 
 */
@property (nonatomic, assign) NSJSONReadingOptions readingOptions;

/**
 Whether to remove keys with `NSNull` values from response JSON. Defaults to `NO`.
    是否移除response中NSNull值对应的key，默认情况下是NO
 */
@property (nonatomic, assign) BOOL removesKeysWithNullValues;

/**
 Creates and returns a JSON serializer with specified reading and writing options.

 @param readingOptions The specified JSON reading options.
 
    通过特定的读写选项来创建和返回JSON序列化对象
    特定的JSON读取选项
 */
+ (instancetype)serializerWithReadingOptions:(NSJSONReadingOptions)readingOptions;

@end

#pragma mark -

/**
 `AFXMLParserResponseSerializer` is a subclass of `AFHTTPResponseSerializer` that validates and decodes XML responses as an `NSXMLParser` objects.

 By default, `AFXMLParserResponseSerializer` accepts the following MIME types, which includes the official standard, `application/xml`, as well as other commonly-used types:

 - `application/xml`
 - `text/xml`
 
    AFXMLParserResponseSerializer是AFHTTPResponseSerializer的子类，来验证和解码XML响应作为NSXMLParser对象
     默认情况下，AFXMLParserResponseSerializer接受下面的MIME种类，包含官方标准的application/xml以及其他常用的类型
     - `application/xml`
     - `text/xml`
 */
@interface AFXMLParserResponseSerializer : AFHTTPResponseSerializer

@end

#pragma mark -

#ifdef __MAC_OS_X_VERSION_MIN_REQUIRED

/**
 `AFXMLDocumentResponseSerializer` is a subclass of `AFHTTPResponseSerializer` that validates and decodes XML responses as an `NSXMLDocument` objects.

 By default, `AFXMLDocumentResponseSerializer` accepts the following MIME types, which includes the official standard, `application/xml`, as well as other commonly-used types:

 - `application/xml`
 - `text/xml`
 */
@interface AFXMLDocumentResponseSerializer : AFHTTPResponseSerializer

- (instancetype)init;

/**
 Input and output options specifically intended for `NSXMLDocument` objects. For possible values, see the `NSJSONSerialization` documentation section "NSJSONReadingOptions". `0` by default.
    读取NSXMLDocument响应数据和创建基础对象的选项，默认情况下是0
 */
@property (nonatomic, assign) NSUInteger options;

/**
 Creates and returns an XML document serializer with the specified options.

 @param mask The XML document options.
 
     通过特定的读写选项来创建和返回XML序列化对象
     特定的XML读取选项
 */
+ (instancetype)serializerWithXMLDocumentOptions:(NSUInteger)mask;

@end

#endif

#pragma mark -

/**
 `AFPropertyListResponseSerializer` is a subclass of `AFHTTPResponseSerializer` that validates and decodes XML responses as an `NSXMLDocument` objects.

 By default, `AFPropertyListResponseSerializer` accepts the following MIME types:

 - `application/x-plist`
 
     AFPropertyListResponseSerializer是AFHTTPResponseSerializer的子类，来验证和解码XML响应作为NSXMLParser对象
     默认情况下，AFPropertyListResponseSerializer接受下面的MIME种类
      - `application/x-plist`
 */
@interface AFPropertyListResponseSerializer : AFHTTPResponseSerializer

- (instancetype)init;

/**
 The property list format. Possible values are described in "NSPropertyListFormat".
 
    属性列表格式，可能的值在NSPropertyListFormat
 */
@property (nonatomic, assign) NSPropertyListFormat format;

/**
 The property list reading options. Possible values are described in "NSPropertyListMutabilityOptions."
 
    属性列表读取选项，可能的值在NSPropertyListMutabilityOptions
 */
@property (nonatomic, assign) NSPropertyListReadOptions readOptions;

/**
 Creates and returns a property list serializer with a specified format, read options, and write options.

 @param format The property list format.
 @param readOptions The property list reading options.
 
    通过特定的格式，读写选项来创建和返回属性列表序列化对象
    属性列表格式
    属性列表的读取选项
 */
+ (instancetype)serializerWithFormat:(NSPropertyListFormat)format
                         readOptions:(NSPropertyListReadOptions)readOptions;

@end

#pragma mark -

/**
 `AFImageResponseSerializer` is a subclass of `AFHTTPResponseSerializer` that validates and decodes image responses.

 By default, `AFImageResponseSerializer` accepts the following MIME types, which correspond to the image formats supported by UIImage or NSImage:

 - `image/tiff`
 - `image/jpeg`
 - `image/gif`
 - `image/png`
 - `image/ico`
 - `image/x-icon`
 - `image/bmp`
 - `image/x-bmp`
 - `image/x-xbitmap`
 - `image/x-win-bitmap`
 
     AFImageResponseSerializer是AFHTTPResponseSerializer的子类，来验证和解码图像响应体
     默认情况下，AFImageResponseSerializer接受下面的MIME种类，和UIImage
     - `image/tiff`
     - `image/jpeg`
     - `image/gif`
     - `image/png`
     - `image/ico`
     - `image/x-icon`
     - `image/bmp`
     - `image/x-bmp`
     - `image/x-xbitmap`
     - `image/x-win-bitmap`
 */
@interface AFImageResponseSerializer : AFHTTPResponseSerializer

#if TARGET_OS_IOS || TARGET_OS_TV || TARGET_OS_WATCH
/**
 The scale factor used when interpreting the image data to construct `responseImage`. Specifying a scale factor of 1.0 results in an image whose size matches the pixel-based dimensions of the image. Applying a different scale factor changes the size of the image as reported by the size property. This is set to the value of scale of the main screen by default, which automatically scales images for retina displays, for instance.
    
    使用来缩放的因素，解读图像数据到responseImage结构。指定比例因子1.0会导致图像的大小与图像的基于像素的尺寸匹配。 应用不同的缩放因子会更改由size属性报告的图像大小。 这被设置为默认情况下主屏幕的缩放值，例如，它自动缩放视网膜显示器的图像。
 */
@property (nonatomic, assign) CGFloat imageScale;

/**
 Whether to automatically inflate response image data for compressed formats (such as PNG or JPEG). Enabling this can significantly improve drawing performance on iOS when used with `setCompletionBlockWithSuccess:failure:`, as it allows a bitmap representation to be constructed in the background rather than on the main thread. `YES` by default.
 
    是否自动为压缩格式（如PNG或JPEG）填充响应图像数据。 启用此功能可以显着提高iOS上的绘图性能，与`setCompletionBlockWithSuccess：failure：`一起使用，因为它允许在后台而不是主线程中构造位图表示。 `YES'默认值。
 */
@property (nonatomic, assign) BOOL automaticallyInflatesResponseImage;
#endif

@end

#pragma mark -

/**
 `AFCompoundSerializer` is a subclass of `AFHTTPResponseSerializer` that delegates the response serialization to the first `AFHTTPResponseSerializer` object that returns an object for `responseObjectForResponse:data:error:`, falling back on the default behavior of `AFHTTPResponseSerializer`. This is useful for supporting multiple potential types and structures of server responses with a single serializer.
 
    AFCompoundSerializer是AFHTTPResponseSerializer的子类，它将响应序列化委托给第一个`AFHTTPResponseSerializer`对象，该对象返回`responseObjectForResponse：data：error：`的对象，回溯到AFHTTPResponseSerializer的默认行为。 这对于通过单个串行器支持多种潜在类型和结构的服务器响应非常有用。
 */
@interface AFCompoundResponseSerializer : AFHTTPResponseSerializer

/**
 The component response serializers.
 
    响应的序列化集合的组成
 */
@property (readonly, nonatomic, copy) NSArray <id<AFURLResponseSerialization>> *responseSerializers;

/**
 Creates and returns a compound serializer comprised of the specified response serializers.

 @warning Each response serializer specified must be a subclass of `AFHTTPResponseSerializer`, and response to `-validateResponse:data:error:`.
 
    创建并返回由指定的响应序列化程序组成的复合序列化程序。
    ⚠️：每一个响应序列化对象必须是AFHTTPResponseSerializer的子类，而且要实现validateResponse:data:error:
 */
+ (instancetype)compoundSerializerWithResponseSerializers:(NSArray <id<AFURLResponseSerialization>> *)responseSerializers;

@end

///----------------
/// @name Constants
///----------------

/**
 ## Error Domains       错误的域名

 The following error domain is predefined.
 下面的错误域名是预定义的

 - `NSString * const AFURLResponseSerializationErrorDomain`

 ### Constants

 `AFURLResponseSerializationErrorDomain`
 AFURLResponseSerializer errors. Error codes for `AFURLResponseSerializationErrorDomain` correspond to codes in `NSURLErrorDomain`.
 
    AFURLResponseSerializationErrorDomain错误，AFURLResponseSerializationErrorDomain的错误码与NSURLErrorDomain的代码相一致
 */
FOUNDATION_EXPORT NSString * const AFURLResponseSerializationErrorDomain;

/**
 ## User info dictionary keys       用户信息字典keys

 These keys may exist in the user info dictionary, in addition to those defined for NSError.

    这些keys也许会存在用户信息字典中，处理那些定义的NSError信息
 
 - `NSString * const AFNetworkingOperationFailingURLResponseErrorKey`
 - `NSString * const AFNetworkingOperationFailingURLResponseDataErrorKey`

 ### Constants

 `AFNetworkingOperationFailingURLResponseErrorKey`
 The corresponding value is an `NSURLResponse` containing the response of the operation associated with an error. This key is only present in the `AFURLResponseSerializationErrorDomain`.
 
    相应的值是一个`NSURLResponse`，它包含与错误相关的操作的响应。 这个键只存在于`AFURLResponseSerializationErrorDomain`中。

 `AFNetworkingOperationFailingURLResponseDataErrorKey`
 The corresponding value is an `NSData` containing the original data of the operation associated with an error. This key is only present in the `AFURLResponseSerializationErrorDomain`.
    
 `AFNetworkingOperationFailingURLResponseDataErrorKey`
   相应的值是一个包含与错误相关的操作的原始数据的“NSData”。 这个键只存在于`AFURLResponseSerializationErrorDomain`中。
 */
FOUNDATION_EXPORT NSString * const AFNetworkingOperationFailingURLResponseErrorKey;

FOUNDATION_EXPORT NSString * const AFNetworkingOperationFailingURLResponseDataErrorKey;

NS_ASSUME_NONNULL_END
