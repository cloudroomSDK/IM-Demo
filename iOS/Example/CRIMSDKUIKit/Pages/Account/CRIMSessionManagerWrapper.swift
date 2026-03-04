//
//  CRIMSessionManager.swift
//  CRIMSDKUIKit_Example
//
//  Created by YunWu01 on 2024/4/16.
//

import Foundation
import Alamofire

class CRIMSessionManagerWrapper {
    static let shared = CRIMSessionManagerWrapper()

    public var sessionManager: SessionManager
    
    private init() {
        let serverTrustPolicyManager = ServerTrustPolicyManager(policies: [:])
        sessionManager = SessionManager(serverTrustPolicyManager: serverTrustPolicyManager)
        
        updateCertificateValidation()
    }

    func updateCertificateValidation() {

        let skipVerifyCert = UserDefaults.standard.bool(forKey: skipVerifyCertKey)
        
        if skipVerifyCert {
            let bussinessSeverAddr = UserDefaults.standard.string(forKey: bussinessSeverAddrKey) ?? AppConfig.defaultServerAddr
            let severAddress = bussinessSeverAddr.parseToServerAddress()
            let serverTrustPolicyManager = ServerTrustPolicyManager(policies: [
                severAddress: .disableEvaluation,
            ])
            sessionManager = SessionManager(serverTrustPolicyManager: serverTrustPolicyManager)
        } else {
            let serverTrustPolicyManager = ServerTrustPolicyManager(policies: [:])
            sessionManager = SessionManager(serverTrustPolicyManager: serverTrustPolicyManager)
        }
    }

    func request(_ url: URLConvertible, method: HTTPMethod, parameters: Parameters? = nil, completion: @escaping (DataResponse<Any>) -> Void) {
        sessionManager.request(url, method: method, parameters: parameters).responseJSON(completionHandler: completion)
    }
}

extension String {
    /// 从完整网址中去除协议、端口、路径，仅保留服务器地址（域名/IP）
    func parseToServerAddress() -> String {
        guard !self.isEmpty else { return "" }
        let handleUrlStr = self.hasPrefix("http") ? self : "https://\(self)"
        guard let url = URL(string: handleUrlStr) else {
            return self
        }
        guard let host = url.host else { return self }
        return host
    }
}
