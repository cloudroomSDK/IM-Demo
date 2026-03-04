import Foundation
import UIKit


class DefaultURLSessionDelegate: NSObject, URLSessionDelegate {
    var shouldValidateSSL: Bool = false // 默认开启校验
    
    func urlSession(_ session: URLSession, didReceive challenge: URLAuthenticationChallenge,
                    completionHandler: @escaping (URLSession.AuthChallengeDisposition, URLCredential?) -> Void) {
        guard challenge.protectionSpace.authenticationMethod == NSURLAuthenticationMethodServerTrust else {
            completionHandler(.performDefaultHandling, nil)
            return
        }
        if shouldValidateSSL {
            completionHandler(.performDefaultHandling, nil)
        } else {
            guard let serverTrust = challenge.protectionSpace.serverTrust else {
                completionHandler(.cancelAuthenticationChallenge, nil)
                return
            }
            completionHandler(.useCredential, URLCredential(trust: serverTrust))
        }
    }
}

public struct DefaultImageLoader: ImageLoader {
    private let sessionDelegate = DefaultURLSessionDelegate()
    
    var shouldValidateSSL: Bool {
        get { sessionDelegate.shouldValidateSSL }
        set { sessionDelegate.shouldValidateSSL = newValue }
    }
    
    public mutating func updateTrustMode(validate: Bool) {
        shouldValidateSSL = validate
    }
    
    public init() {}

    public func loadImage(from url: URL, completion: @escaping (Result<UIImage, Error>) -> Void) {
        let request = URLRequest(url: url, cachePolicy: .reloadIgnoringCacheData)
        let sessionConfig = URLSessionConfiguration.default
        sessionConfig.urlCredentialStorage = nil
        let session = URLSession(
            configuration: sessionConfig,
            delegate: sessionDelegate,
            delegateQueue: OperationQueue()
        )
        let task = session.dataTask(with: request) { data, _, error in
            DispatchQueue.global(qos: .utility).async {
                if let error = error {
                    DispatchQueue.main.async { completion(.failure(error)) }
                    return
                }
                guard let data = data, let image = UIImage(data: data) else {
                    DispatchQueue.main.async { completion(.failure(SourceError.corruptedData)) }
                    return
                }
                DispatchQueue.main.async { completion(.success(image)) }
            }
        }
        task.resume()
    }
}
