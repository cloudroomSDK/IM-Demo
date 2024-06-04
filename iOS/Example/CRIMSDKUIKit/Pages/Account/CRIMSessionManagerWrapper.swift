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
    }

    func updateCertificateValidation() {
        
        let protocolType = UserDefaults.standard.integer(forKey: protocolKey)
        
        if protocolType == 2 {
            let severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultHost
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


