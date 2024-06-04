
import Alamofire
import Foundation
import CRUICore
import RxSwift
import GTSDK

// 注册/忘记密码
public enum UsedFor: Int {
    case register = 1
    case forgotPassword = 2
    case login = 3
}

typealias CompletionHandler = (_ errCode: Int, _ errMsg: String?) -> Void

class CustomSessionManagerWrapper {
    var sessionManager: SessionManager!

    init() {
        let serverTrustPolicies: [String: ServerTrustPolicy] = [
            "192.168.0.43": .disableEvaluation,
        ]

        let serverTrustPolicyManager = ServerTrustPolicyManager(policies: serverTrustPolicies)

        let configuration = URLSessionConfiguration.default
        configuration.httpAdditionalHeaders = SessionManager.defaultHTTPHeaders

        sessionManager = SessionManager(configuration: configuration, serverTrustPolicyManager: serverTrustPolicyManager)
    }

    func request(_ url: URLConvertible, method: HTTPMethod, parameters: Parameters? = nil, completion: @escaping (DataResponse<Any>) -> Void) {
        sessionManager?.request(url, method: method, parameters: parameters).responseJSON(completionHandler: completion)
    }
}

open class AccountViewModel {
    
    // 业务服务器地址
    static var API_BASE_URL = UserDefaults.standard.string(forKey: bussinessSeverAddrKey)!
    static var ADMIN_BASE_URL = UserDefaults.standard.string(forKey: adminSeverAddrKey)!
    // 实际开发，抽离网络部分
    static let IMPreLoginAccountKey = "IMPreLoginAccountKey"
    static let IMUidKey = "DemoIMUidKey"
    static let IMTokenKey = "DemoIMTokenKey"
    static let bussinessTokenKey = "bussinessTokenKey"
    
    private static let LoginAPI = "/account/login"
    private static let RegisterAPI = "/account/register"
    private static let CodeAPI = "/account/code/send"
    private static let VerifyCodeAPI = "/account/code/verify"
    private static let ResetPasswordAPI = "/account/password/reset"
    private static let ChangePasswordAPI = "/account/password/change"
    private static let UpdateUserInfoAPI = "/user/update"
    private static let QueryUserInfoAPI = "/user/find/full"
    private static let SearchUserFullInfoAPI = "/user/search/full"
    private static let GetClientConfigAPI = "/client_config/get"
    private static let BlockAddAPI = "/block/add"
    
    private static let sessionManagerWrapper = CustomSessionManagerWrapper()
    
    private let _disposeBag = DisposeBag()
    
    // 业务层提供给CRIMUIKit数据
    // 业务查询好友逻辑
    static func ifQueryFriends() {
        
        CRIMApi.queryFriendsWithCompletionHandler = { (keywords, completion: @escaping ([UserInfo]) -> Void) in
            AccountViewModel.queryFriends(content: keywords.first!, valueHandler: { users in
                let result = users.compactMap {
                    UserInfo.init(userID: $0.userID!, nickname: $0.nickname, phoneNumber: $0.phoneNumber, email: $0.email)
                }
                completion(result)
            }, completionHandler: {(errCode, errMsg) -> Void in
                completion([])
            })
        }
    }
    
    // 业务查询用户信息
    static func ifQueryUserInfo() {
        
        CRIMApi.queryUsersInfoWithCompletionHandler = { (keywords, completion: @escaping ([UserInfo]) -> Void) in
            AccountViewModel.queryUserInfo(userIDList: keywords,
                                           valueHandler: { users in
                let result = users.compactMap {
                    UserInfo(userID: $0.userID!,
                             nickname: $0.nickname,
                             phoneNumber: $0.phoneNumber,
                             email: $0.email,
                             faceURL: $0.faceURL,
                             birth: $0.birth,
                             gender: Gender(rawValue: $0.gender ?? 0),
                             landline: $0.telephone
                    )
                }
                completion(result)
            }, completionHandler: {(errCode, errMsg) -> Void in
                completion([])
            })
        }
    }
    
    // 全局配置信息
    static func ifQeuryConfig() {
        CRIMApi.queryConfigHandler = { (completion: @escaping ([String: Any]) -> Void) in
            completion(AccountViewModel.clientConfig?.toMap() ?? [:])
        }
    }
    
    static func loginDemo(phone: String? = nil, account: String? = nil, psw: String? = nil, verificationCode: String? = nil, areaCode: String, completionHandler: @escaping CompletionHandler) {
        let body = JsonTool.toJson(fromObject: Request(phoneNumber: phone,
                                                       account: account,
                                                       psw: psw,
                                                       verificationCode: verificationCode,
                                                       areaCode: areaCode)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + LoginAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")

        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        // 登录IM
                        savePreLoginAccount(phone)
                        loginIM(uid: res.data!.userID, imToken: res.data!.imToken, chatToken: res.data!.chatToken, completionHandler: completionHandler)
                    } else {
                        IMController.writeLog(content: "输出错误：\(String(describing: res.errMsg))")
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                    let err = JsonTool.fromJson(result, toClass: DemoError.self)
                    IMController.writeLog(content: "输出错误：\(String(describing: err?.errMsg))")
                    completionHandler(err?.errCode ?? -1, err?.errMsg)
                }
            case .failure(let err):
                IMController.writeLog(content: "输出错误：failure \(err.localizedDescription)")
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    static func blockAdd(userID: String? = nil, adminToken: String? = nil, completionHandler: @escaping CompletionHandler) {
        let body = JsonTool.toJson(fromObject: ["userID": userID ?? ""]).data(using: .utf8)
        
        var req = try! URLRequest(url: ADMIN_BASE_URL + BlockAddAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        req.addValue(adminToken ?? "", forHTTPHeaderField: "token")

        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<AdminEntity>.self) {
                    if res.errCode == 0 {
                        
                        completionHandler(res.errCode, nil)
                    } else {
                        IMController.writeLog(content: "输出错误：\(String(describing: res.errMsg))")
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                    let err = JsonTool.fromJson(result, toClass: DemoError.self)
                    IMController.writeLog(content: "输出错误：\(String(describing: err?.errMsg))")
                    completionHandler(err?.errCode ?? -1, err?.errMsg)
                }
            case .failure(let err):
                IMController.writeLog(content: "输出错误：failure \(err.localizedDescription)")
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    static func loginAdmin(account: String? = nil, psw: String? = nil, completionHandler: @escaping CompletionHandler) {
        let body = JsonTool.toJson(fromObject: AdminRequest(account: account,
                                                            psw: psw)).data(using: .utf8)
        
        var req = try! URLRequest(url: ADMIN_BASE_URL + LoginAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")

        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<AdminEntity>.self) {
                    if res.errCode == 0 {
                        adminToken = res.data?.adminToken
                        completionHandler(res.errCode, nil)
                    } else {
                        IMController.writeLog(content: "输出错误：\(String(describing: res.errMsg))")
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                    let err = JsonTool.fromJson(result, toClass: DemoError.self)
                    IMController.writeLog(content: "输出错误：\(String(describing: err?.errMsg))")
                    completionHandler(err?.errCode ?? -1, err?.errMsg)
                }
            case .failure(let err):
                IMController.writeLog(content: "输出错误：failure \(err.localizedDescription)")
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    static var adminToken: String?
    
    static func registerAccount(phone: String,
                                areaCode: String,
                                verificationCode: String,
                                password: String,
                                faceURL: String,
                                nickName: String,
                                birth: Int = Int(NSDate().timeIntervalSince1970),
                                gender: Int = 1,
                                email: String = "",
                                invitationCode: String? = nil,
                                completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    RegisterRequest(
                                        phone: phone,
                                        areaCode: areaCode,
                                        verificationCode: verificationCode,
                                        password: password,
                                        faceURL: faceURL,
                                        nickName: nickName,
                                        birth: birth,
                                        gender: gender,
                                        invitationCode: invitationCode)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + RegisterAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        saveUser(uid: res.data?.userID, imToken: res.data?.imToken, chatToken: res.data?.chatToken)
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {}
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    // [usedFor] 1：注册，2：重置密码， 3: 登录
    static func requestCode(phone: String, areaCode: String, invaitationCode: String? = nil, useFor: UsedFor, completionHandler: @escaping CompletionHandler) {
        let body = JsonTool.toJson(fromObject:
                                    CodeRequest(
                                        phone: phone,
                                        areaCode: areaCode,
                                        usedFor: useFor.rawValue,
                                        invaitationCode: invaitationCode)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + CodeAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                }
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    // [usedFor] 1：注册，2：重置密码
    static func verifyCode(phone: String, areaCode: String, useFor: UsedFor, verificationCode: String, completionHandler: @escaping CompletionHandler) {
        let body = JsonTool.toJson(fromObject:
                                    CodeRequest(
                                        phone: phone,
                                        areaCode: areaCode,
                                        usedFor: useFor.rawValue,
                                        verificationCode: verificationCode)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + VerifyCodeAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {}
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    static func resetPassword(phone: String,
                              areaCode: String,
                              verificationCode: String,
                              password: String,
                              completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    Request(
                                        phoneNumber: phone,
                                        psw: password,
                                        verificationCode: verificationCode,
                                        areaCode: areaCode)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + ResetPasswordAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {}
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    static func changePassword(userID: String, password: String, completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    UpdateUserInfoRequest(
                                        userID: userID,
                                        password: password)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + ChangePasswordAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UserEntity>.self) {
                    if res.errCode == 0 {
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {}
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    // 更新个人信息
    static func updateUserInfo(userID: String,
                               account: String? = nil,
                               areaCode: String? = nil,
                               phone: String? = nil,
                               email: String? = nil,
                               nickname: String? = nil,
                               faceURL: String? = nil,
                               gender: Gender? = nil,
                               birth: Int? = nil,
                               level: Int? = nil,
                               allowAddFriend: Int? = nil,
                               allowBeep: Int? = nil,
                               allowVibration: Int? = nil,
                               completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    UpdateUserInfoRequest(userID: userID,
                                                          phone: phone,
                                                          faceURL: faceURL,
                                                          nickname: nickname,
                                                          birth: birth,
                                                          gender: gender,
                                                          account: account,
                                                          level: level,
                                                          email: email,
                                                          allowAddFriend: allowAddFriend,
                                                          allowBeep: allowBeep,
                                                          allowVibration: allowVibration)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + UpdateUserInfoAPI, method: .post)
        req.httpBody = body
        req.addValue(UserDefaults.standard.string(forKey: bussinessTokenKey)!, forHTTPHeaderField: "token")
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<UpdateUserInfoRequest>.self) {
                    if res.errCode == 0 {
                        completionHandler(res.errCode, nil)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {}
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    // 获取个人信息
    static func queryUserInfo(pageNumber: Int = 1,
                              showNumber: Int = 10,
                              userIDList: [String],
                              valueHandler: @escaping ([QueryUserInfo]) -> Void,
                              completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    QueryUserInfoRequest(userIDList: userIDList)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + QueryUserInfoAPI, method: .post)
        req.httpBody = body
        req.addValue(UserDefaults.standard.string(forKey: bussinessTokenKey)!, forHTTPHeaderField: "token")
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<QueryUserInfoData>.self) {
                    if res.errCode == 0 {
                        valueHandler(res.data!.users)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                    completionHandler(-1, "")
                }
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    // 查询好友
    static func queryFriends(pageNumber: Int = 1,
                             showNumber: Int = 100,
                             content: String,
                             valueHandler: @escaping ([QueryUserInfo]) -> Void,
                             completionHandler: @escaping CompletionHandler)
    {
        let body = JsonTool.toJson(fromObject:
                                    QueryFriendsRequest(keyword: content,
                                                        pageNumber: pageNumber,
                                                        showNumber: showNumber)).data(using: .utf8)
        
        var req = try! URLRequest(url: API_BASE_URL + SearchUserFullInfoAPI, method: .post)
        req.httpBody = body
        req.addValue(UserDefaults.standard.string(forKey: bussinessTokenKey)!, forHTTPHeaderField: "token")
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        print("输入地址：\(req)")
        IMController.writeLog(content: "输入地址：\(req)")
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<QueryUserInfoData>.self) {
                    if res.errCode == 0 {
                        valueHandler(res.data!.users)
                    } else {
                        completionHandler(res.errCode, res.errMsg)
                    }
                } else {
                    completionHandler(-1, nil)
                }
            case .failure(let err):
                completionHandler(-1, err.localizedDescription)
            }
        }
    }
    
    static func loginIM(uid: String, imToken: String, chatToken: String, completionHandler: @escaping CompletionHandler) {
        
        let enableToken = UserDefaults.standard.object(forKey: useTokenKey) == nil
        ? false : UserDefaults.standard.bool(forKey: useTokenKey)
        
        var appID = ""
        var appSecret = ""
        var token = ""
        
        if enableToken {
            token = UserDefaults.standard.string(forKey: sdkTokenKey) ?? defaultToken
        } else {
            appID = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? defaultAppID
            appSecret = UserDefaults.standard.string(forKey: sdkAPPSecretKey) ?? defaultAppSecret
            appSecret = appSecret.md5()
        }
        
        IMController.shared.login(uid: uid, token: token, appId: appID, appSecret: appSecret) { resp in
            print("login onSuccess \(String(describing: resp))")
            ifQueryFriends()
            ifQueryUserInfo()
            ifQeuryConfig()
            saveUser(uid: uid, imToken: imToken, chatToken: chatToken)
            GeTuiSdk.bindAlias(uid, andSequenceNum: "crim") // 绑定别名
            completionHandler(0, nil)
        } onFail: { (code: Int, msg: String?) in
            let reason = "login onFail: code \(code), reason \(String(describing: msg))"
            print(reason)
            completionHandler(code, reason)
            saveUser(uid: nil, imToken: nil, chatToken: nil)
        }
    }
    
    static func saveUser(uid: String?, imToken: String?, chatToken: String?) {
        IMController.shared.token = imToken ?? ""
        
        UserDefaults.standard.set(uid, forKey: IMUidKey)
        UserDefaults.standard.set(imToken, forKey: IMTokenKey)
        UserDefaults.standard.set(chatToken, forKey: bussinessTokenKey)
        UserDefaults.standard.synchronize()
        
        IMController.shared.setup(businessServer: UserDefaults.standard.string(forKey: bussinessSeverAddrKey)!, businessToken: chatToken)
    }
    
    static func savePreLoginAccount(_ account: String?) {
        UserDefaults.standard.set(account, forKey: IMPreLoginAccountKey)
        UserDefaults.standard.synchronize()
    }
    
    static var perLoginAccount: String? {
        return UserDefaults.standard.string(forKey: IMPreLoginAccountKey)
    }
    
    static var userID: String? {
        return UserDefaults.standard.string(forKey: IMUidKey)
    }
    
    static var baseUser: UserEntity {
        return UserEntity(userID: UserDefaults.standard.string(forKey: IMUidKey) ?? "",
                          imToken: UserDefaults.standard.string(forKey: IMTokenKey) ?? "",
                          chatToken: UserDefaults.standard.string(forKey: bussinessTokenKey) ?? "",
                          expiredTime: nil)
    }
    
    // 获取配置
    static func getClientConfig() {
        let body = try! JSONSerialization.data(withJSONObject: ["operationID": UUID().uuidString], options: .prettyPrinted)
        
        var req = try! URLRequest(url: ADMIN_BASE_URL + GetClientConfigAPI, method: .post)
        req.httpBody = body
        req.addValue(UUID().uuidString, forHTTPHeaderField: "operationID")
        
        CRIMSessionManagerWrapper.shared.sessionManager.request(req).responseString(encoding: .utf8) { (response: DataResponse<String>) in
            switch response.result {
            case .success(let result):
                if let res = JsonTool.fromJson(result, toClass: Response<ClientConfigData>.self) {
                    if res.errCode == 0 {
                        clientConfig = res.data
                    } else {
                    }
                } else {}
            case .failure(_):
                break
            }
        }
    }
    // 配置
    static var clientConfig: ClientConfigData?
}


class Request: Encodable {
    private let areaCode: String?
    private let appID: String?
    private let phoneNumber: String?
    private let password: String
    private let verifyCode: String?
    private let platform: Int = 1
    private let account: String?
    init(phoneNumber: String? = nil, account: String? = nil, psw: String? = nil, verificationCode: String? = nil, areaCode: String? = nil) {
        self.phoneNumber = phoneNumber
        self.account = account
        self.password = psw?.md5() ?? ""
        self.areaCode = areaCode
        self.verifyCode = verificationCode
        
        // 解析appID
        let enableToken = UserDefaults.standard.object(forKey: useTokenKey) == nil
        ? false : UserDefaults.standard.bool(forKey: useTokenKey)
        var appID = ""
        if enableToken {
            var token = UserDefaults.standard.string(forKey: sdkTokenKey) ?? defaultToken
            let tokenArr = token.components(separatedBy: ".")
            if tokenArr.count > 2 {
                var base64String = tokenArr[1]
                let padding = base64String.count % 4
                if padding > 0 {
                    base64String = base64String.padding(toLength: base64String.count + 4 - padding, withPad: "=", startingAt: 0)
                }
                if let data = Data(base64Encoded: base64String),
                   let decodedString = String(data: data, encoding: .utf8) {
                    
                    if let jsonData = decodedString.data(using: .utf8) {
                        do {
                            if let json = try JSONSerialization.jsonObject(with: jsonData, options: []) as? [String: Any] {
                                if let appId = json["appID"] as? String {
                                    appID = appId
                                    print("appId: \(appId)")
                                }
                            }
                        } catch {
                            print("JSON解析错误: \(error)")
                        }
                    }
                }
                
            }
            
        } else {
            appID = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? defaultAppID
        }
        appID = appID.trimmingCharacters(in: .whitespaces)
        self.appID = appID
    }
}

class AdminRequest: Encodable {
    private let account: String?
    private let password: String
    
    init(account: String? = nil, psw: String? = nil) {
        self.account = account
        self.password = psw ?? ""
    }
}

class Response<T: Decodable>: Decodable {
    var data: T? = nil
    var errCode: Int = 0
    var errMsg: String? = nil
    var errDlt: String?
}

struct UserEntity: Decodable {
    let userID: String
    let imToken: String
    let chatToken: String
    let expiredTime: Int?
}

struct AdminEntity: Decodable {
    let adminUserID: String
    let adminToken: String
    let imUserID: String
    let imToken: String
}

class RegisterRequest: Encodable {
    private var verifyCode: String?
    private let platform: Int = 1
    private let user: UpdateUserInfoRequest
    private let invitationCode: String?
    private let deviceID = UUID().uuidString
    private let autoLogin = true
    
    init(phone: String, areaCode: String, verificationCode: String?, password: String?, faceURL: String?, nickName: String?, birth: Int?, gender: Int?, email: String? = nil, invitationCode: String?) {
        self.user = UpdateUserInfoRequest(userID: "", phone: phone, password: password, areaCode: areaCode, nickname: nickName)
        self.verifyCode = verificationCode
        self.invitationCode = invitationCode
    }
}

class CodeRequest: Encodable {
    private let areaCode: String
    private let phoneNumber: String
    private let usedFor: Int
    private let verifyCode: String?
    private let invaitationCode: String?
    private let platform: Int = 1
    
    init(phone: String, areaCode: String, usedFor: Int, invaitationCode: String? = nil, verificationCode: String? = nil) {
        self.phoneNumber = phone
        self.areaCode = areaCode
        self.usedFor = usedFor
        self.verifyCode = verificationCode
        self.invaitationCode = invaitationCode
    }
}

class QueryUserInfoRequest: Encodable {
    private let userIDs: [String]
    
    init(pageNumber: Int = 1, showNumber: Int = 10, userIDList: [String]) {
        self.userIDs = userIDList
    }
}

class QueryUserInfoData: Decodable {
    let users: [QueryUserInfo]
    let totalNumber: Int?
}

class QueryUserInfo: UpdateUserInfoRequest {}

class UpdateUserInfoRequest: Codable {
    let userID: String?
    let account: String?
    let password: String?
    let level: Int?
    var faceURL: String?
    var nickname: String?
    let areaCode: String?
    let phoneNumber: String?
    let telephone: String?
    let hireDate: String?
    private var platform: Int? = 1
    var birth: Int?
    var gender: Int?
    let email: String?
    let englishName: String?
    let forbidden: Int?
    let allowAddFriend: Int?
    let allowBeep: Int?
    let allowVibration: Int?
    
    init(userID: String,
         phone: String? = nil,
         password: String? = nil,
         telephone: String? = nil,
         areaCode: String? = nil,
         faceURL: String? = nil,
         nickname: String? = nil,
         englishName: String? = nil,
         birth: Int? = nil,
         gender: Gender? = nil,
         account: String? = nil,
         level: Int? = nil,
         email: String? = nil,
         hireDate: String? = nil,
         allowAddFriend: Int? = nil,
         allowBeep: Int? = nil,
         allowVibration: Int? = nil,
         forbidden: Int? = nil)
    {
        self.areaCode = areaCode
        self.telephone = telephone
        self.password = password?.md5()
        self.phoneNumber = phone
        self.faceURL = faceURL
        self.nickname = nickname
        self.englishName = englishName
        self.birth = birth
        self.gender = gender?.rawValue
        self.email = email
        self.account = account
        self.level = level
        self.userID = userID
        self.hireDate = hireDate
        self.allowAddFriend = allowAddFriend
        self.allowBeep = allowBeep
        self.allowVibration = allowVibration
        self.forbidden = forbidden
    }
}

class QueryFriendsRequest: Encodable {
    private let pagination: Pagination
    private let keyword: String
    private let platform: Int = 1
    
    init(keyword: String, pageNumber: Int = 1, showNumber: Int = 100) {
        self.keyword = keyword
        self.pagination = Pagination(pageNumber: pageNumber, showNumber: showNumber)
    }
}

class Pagination: Encodable {
    private let pageNumber: Int
    private let showNumber: Int
    
    init(pageNumber: Int, showNumber: Int) {
        self.pageNumber = pageNumber
        self.showNumber = showNumber
    }
}

class ClientConfigData: Codable {
    var discoverPageURL: String?
    var ordinaryUserAddFriend: Int?
    var bossUserID: String?
    var adminURL: String?
    var allowSendMsgNotFriend: Int?
    var needInvitationCodeRegister: Int?
    var robots: [String]?
    
    func toMap() -> [String: Any] {
        return JsonTool.toMap(fromObject: self)
    }
}

struct DemoError: Error, Decodable {
    let errCode: Int
    let errMsg: String?
    
    var localizedDescription: String {
        let msg: String = errMsg ?? "no message"
        return "code: \(errCode), msg: \(msg)"
    }
}
