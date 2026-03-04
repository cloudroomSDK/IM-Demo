//
//  KingfisherImageDownloader.swift
//  CRIMSDKUIKit
//
//  Created by tom on 2026/2/3.
//

import Kingfisher
import Foundation

// 全局自定义下载器：跳过HTTPS证书校验（完全匹配Kingfisher 7.7源码）
let insecureImageDownloader: ImageDownloader = {
    // 1. 按源码要求，通过name初始化ImageDownloader（name不能空）
    let downloader = ImageDownloader(name: "insecure-ssl-downloader")
    
    // 2. 配置跳过SSL的URLSessionConfiguration（核心）
    let sslConfig = URLSessionConfiguration.default
    sslConfig.urlCredentialStorage = nil // 关闭系统证书存储，基础配置
    sslConfig.requestCachePolicy = .reloadIgnoringLocalCacheData // 测试可选：禁用缓存，避免旧缓存干扰
    sslConfig.timeoutIntervalForRequest = 15.0 // 下载超时时间，可自定义
    
    // 3. 赋值给downloader的sessionConfiguration（源码didSet会自动重建URLSession）
    downloader.sessionConfiguration = sslConfig
    
    // 4. 设置自定义的SessionDelegate（处理证书校验，核心）
    downloader.sessionDelegate = InsecureSessionDelegate()
    
    // 5. 可选：关闭信任主机校验（避免和自定义代理冲突）
    downloader.trustedHosts = nil
    // 可选：设置下载超时（和session配置一致，双层保障）
    downloader.downloadTimeout = 15.0
    
    return downloader
}()

// 自定义SessionDelegate：核心跳过SSL证书校验（适配Kingfisher 7.7的SessionDelegate）
// 注意：继承Kingfisher的SessionDelegate，而非直接NSObject+URLSessionDelegate
class InsecureSessionDelegate: SessionDelegate {
    // 重写证书校验的核心方法，处理服务器SSL挑战
    override func urlSession(
        _ session: URLSession,
        didReceive challenge: URLAuthenticationChallenge,
        completionHandler: @escaping (URLSession.AuthChallengeDisposition, URLCredential?) -> Void
    ) {
        // 仅处理HTTPS服务器证书校验，其他挑战按默认处理
        guard challenge.protectionSpace.authenticationMethod == NSURLAuthenticationMethodServerTrust else {
            completionHandler(.performDefaultHandling, nil)
            return
        }
        // 关键：信任所有服务器证书，跳过SSL校验
        guard let serverTrust = challenge.protectionSpace.serverTrust else {
            completionHandler(.cancelAuthenticationChallenge, nil)
            return
        }
        let credential = URLCredential(trust: serverTrust)
        completionHandler(.useCredential, credential)
    }
}
