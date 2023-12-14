
import Foundation
import MJExtension
import IQKeyboardManagerSwift
import CRIMSDK
import RxCocoa
import RxSwift
import UIKit
import AudioToolbox

// -1 链接失败 0 链接中 1 链接成功 2 同步开始 3 同步结束 4 同步错误
public enum ConnectionStatus: Int {
    case connectFailure = 0
    case connecting = 1
    case connected = 2
    case syncStart = 3
    case syncComplete = 4
    case syncFailure = 5
    case kickedOffline = 6
    
    public var title: String {
        switch self {
        case .connectFailure:
            return "连接失败"
        case .connecting:
            return "连接中".innerLocalized()
        case .connected:
            return "连接成功".innerLocalized()
        case .syncStart:
            return "同步开始".innerLocalized()
        case .syncComplete:
            return "同步完成".innerLocalized()
        case .syncFailure:
            return "同步失败".innerLocalized()
        case .kickedOffline:
            return "账号在其它设备登录".innerLocalized()
        }
    }
}

public enum SDKError: Int {
    case deletedByFriend = 601 // 被对方删除
    case blockedByFriend = 1302 // 被对方拉黑
    case refuseToAddFriends = 10007 // 该用户已设置不可添加
}

public enum CustomMessageType: Int {
    case call = 901 // 音视频
    case customEmoji = 902 // emoji
    case tagMessage = 903 // 标签消息
    case blockedByFriend = 910 // 被拉黑
    case deletedByFriend = 911 // 被删除
}

public class IMController: NSObject {
    public static let addFriendPrefix = "io.crim.app/addFriend/"
    public static let joinGrpPrefix = "io.crim.app/joinGrp/"
    public static let shared: IMController = .init()
    private(set) var imManager: CRIMSDK.CRIMManager!
    /// 好友申请列表新增
    public let friendApplicationChangedSubject: PublishSubject<FriendApplication> = .init()
    /// 组申请信息更新
    public let groupApplicationChangedSubject: PublishSubject<GroupApplicationInfo> = .init()
    public let groupInfoChangedSubject: PublishSubject<GroupInfo> = .init()
    public let contactUnreadSubject: PublishSubject<Int> = .init()
    
    public let conversationChangedSubject: BehaviorSubject<[ConversationInfo]> = .init(value: [])
    public let friendInfoChangedSubject: BehaviorSubject<FriendInfo?> = .init(value: nil)
    
    public let onBlackAddedSubject: BehaviorSubject<BlackInfo?> = .init(value: nil)
    public let onBlackDeletedSubject: BehaviorSubject<BlackInfo?> = .init(value: nil)
    
    public let newConversationSubject: BehaviorSubject<[ConversationInfo]> = .init(value: [])
    public let totalUnreadSubject: BehaviorSubject<Int> = .init(value: 0)
    public let newMsgReceivedSubject: PublishSubject<MessageInfo> = .init()
    public let c2cReadReceiptReceived: BehaviorSubject<[ReceiptInfo]> = .init(value: [])
    public let groupReadReceiptReceived: BehaviorSubject<[ReceiptInfo]> = .init(value: [])
    public let groupMemberInfoChange: BehaviorSubject<GroupMemberInfo?> = .init(value: nil)
    public let joinedGroupAdded: BehaviorSubject<GroupInfo?> = .init(value: nil)
    public let joinedGroupDeleted: BehaviorSubject<GroupInfo?> = .init(value: nil)
    public let msgRevokeReceived: PublishSubject<MessageRevoked> = .init()
    public let currentUserRelay: BehaviorRelay<UserInfo?> = .init(value: nil)
    public let momentsReceivedSubject: PublishSubject<String?> = .init()
    public let organizationUpdated: PublishSubject<String?> = .init()
    // 连接状态
    public let connectionRelay: BehaviorRelay<ConnectionStatus> = .init(value: .connecting)
    
    public var userID: String = ""
    public var token: String = ""
    // 查询在线状态等使用
    public var sdkAPIAdrr = ""
    // 业务层查询组织架构等使用
    public var businessServer = ""
    public var businessToken: String?
    // 上次响铃时间
    private var remindTimeStamp: Double = NSDate().timeIntervalSince1970
    // 开启响铃
    public var enableRing = true
    // 开启震动
    public var enableVibration = true
    
    // 设置业务服务器的参数
    public func setup(businessServer: String, businessToken: String?) {
        self.businessServer = businessServer
        self.businessToken = businessToken
    }
    
    public func setup(sdkAPIAdrr: String, sdkOS: String, onKickedOffline: (() -> Void)? = nil) {
        self.sdkAPIAdrr = sdkAPIAdrr
        let manager = CRIMManager.manager
        
        var config = CRIMInitConfig()
        config.sdkServer = sdkAPIAdrr
        config.objectStorage = sdkOS
        config.logLevel = 6
        
        manager.initSDK(with: config)
        Self.shared.imManager = manager
        
        CRIMManager.callbacker.setConnListenerWithonConnecting { [weak self] in
            self?.connectionRelay.accept(.connecting)
        } onConnectFailure: { [weak self] code, msg in
            print("onConnectFailed code:\(code), msg:\(String(describing: msg))")
            self?.connectionRelay.accept(.connectFailure)
        } onConnectSuccess: { [weak self] in
            print("onConnectSuccess")
            self?.connectionRelay.accept(.connected)
        } onKickedOffline: { [weak self] in
            print("onKickedOffline")
            onKickedOffline?()
            self?.connectionRelay.accept(.kickedOffline)
        } onUserTokenExpired: {
            onKickedOffline?()
            print("onUserTokenExpired")
        }
        
        // Set listener
        CRIMSDK.CRIMManager.callbacker.addFriendListener(listener: self)
        CRIMSDK.CRIMManager.callbacker.addGroupListener(listener: self)
        CRIMSDK.CRIMManager.callbacker.addConversationListener(listener: self)
        CRIMSDK.CRIMManager.callbacker.addAdvancedMsgListener(listener: self)
        
        UINavigationBar.appearance().isTranslucent = true
        UINavigationBar.appearance().isOpaque = true
        
        if #available(iOS 13.0, *) {
            let app = UINavigationBarAppearance()
            app.configureWithOpaqueBackground()
            app.titleTextAttributes = [
                NSAttributedString.Key.font: UIFont.systemFont(ofSize: 18),
                NSAttributedString.Key.foregroundColor: StandardUI.color_333333,
            ]
            app.backgroundColor = UIColor.white
            app.shadowColor = .clear
            UINavigationBar.appearance().scrollEdgeAppearance = app
            UINavigationBar.appearance().standardAppearance = app
        }
        
        IQKeyboardManager.shared.enable = true
        IQKeyboardManager.shared.enableAutoToolbar = false
        IQKeyboardManager.shared.shouldResignOnTouchOutside = true
    }
    
    public func unInitSDK() {
        Self.shared.imManager.unInitSDK()
    }
    
    public class func writeLog(level: LogLevel = .info, content: String) {
        CRIMSDK.CRIMManager.writeLog(level.rawValue, content: content)
    }
    
    public func updateAPNsToken(token: String) {
        Self.shared.imManager.updateAPNsToken(token, expireTime: 0) { _ in
            
        } onFailure: { (code: Int, msg: String?) in
            
        }
    }
    
    public func login(uid: String, token:String, appId: String, appSecret: String, onSuccess: @escaping (String?) -> Void, onFail: @escaping (Int, String?) -> Void) {
        if token.isEmpty {
            Self.shared.imManager.login(uid, appId: appId, appSecret: appSecret) { [weak self] (resp: String?) in
                self?.userID = uid
                //self?.token = token
                let event = EventLoginSucceed()
                JNNotificationCenter.shared.post(event)
                onSuccess(resp)
            } onFailure: { (code: Int, msg: String?) in
                onFail(code, msg)
            }
        } else {
            Self.shared.imManager.login(uid, token: token) { [weak self] (resp: String?) in
                self?.userID = uid
                //self?.token = token
                let event = EventLoginSucceed()
                JNNotificationCenter.shared.post(event)
                onSuccess(resp)
            } onFailure: { (code: Int, msg: String?) in
                onFail(code, msg)
            }
        }
    }
    
    struct NetError: Error {
        let code: Int
        let message: String?
    }
    
    public typealias MessagesCallBack = ([MessageInfo]) -> Void
    public typealias SeqMessagesCallBack = (Int, [MessageInfo]) -> Void
    
    // 正在聊天的会话不响铃
    public var chatingConversationID: String = ""
    
    // 响铃或者震动
    func ringAndVibrate() {
        if NSDate().timeIntervalSince1970 - remindTimeStamp >= 1 { // 响铃间隔1秒钟
            // 如果当前会话有
            // 新消息铃声
            if enableRing {
                var theSoundID : SystemSoundID = 0
                let url = URL(fileURLWithPath: "/System/Library/Audio/UISounds/nano/sms-received1.caf")
                let urlRef = url as CFURL
                let err = AudioServicesCreateSystemSoundID(urlRef, &theSoundID)
                
                if err == kAudioServicesNoError {
                    AudioServicesPlaySystemSoundWithCompletion(theSoundID, {
                        AudioServicesDisposeSystemSoundID(theSoundID)
                    })
                }
            }
            // 新消息震动
            if enableVibration {
                AudioServicesPlayAlertSound(kSystemSoundID_Vibrate)
            }
            remindTimeStamp = NSDate().timeIntervalSince1970
        }
    }
}

extension String {
    // 将clientMsgID转化成UUID
    var uuid: UUID? {
        var str = self
        
        let index1 = str.index(str.startIndex, offsetBy: 8)
        let index2 = str.index(str.startIndex, offsetBy: 13)
        let index3 = str.index(str.startIndex, offsetBy: 18)
        let index4 = str.index(str.startIndex, offsetBy: 23)
        
        str.insert("-", at: index1)
        str.insert("-", at: index2)
        str.insert("-", at: index3)
        str.insert("-", at: index4)
        
        let uuid = UUID(uuidString: str)
        return uuid
    }
}

// MARK: - 对外协议

public protocol ContactsDataSource: AnyObject {
    func setFrequentUsers(_ users: [CRIMUserInfo])
    func getFrequentUsers() -> [CRIMUserInfo]
}

// MARK: - 联系人方法

extension IMController {
    /// 根据id查找群
    /// - Parameter ids: 群id
    /// - Returns: 第一个群的id
    public func getGrpListBy(id: String) -> Observable<String?> {
        return Observable<String?>.create { observer in
            Self.shared.imManager.getSpecifiedGrpsInfo([id], onSuccess: { (groups: [CRIMGroupInfo]?) in
                observer.onNext(groups?.first?.groupID)
                observer.onCompleted()
            }, onFailure: { (code: Int, msg: String?) in
                observer.onError(NetError(code: code, message: msg))
            })
            
            return Disposables.create()
        }
    }
    
    public func getJoinedGrpList(completion: @escaping ([GroupInfo]) -> Void) {
        Self.shared.imManager.getJoinedGrpListWith { (groups: [CRIMGroupInfo]?) in
            guard let groups = groups else {
                completion([])
                return
            }
            
            let joined: [GroupInfo] = groups.compactMap { $0.toGroupInfo() }
            completion(joined)
        } onFailure: { code, msg in
            print("拉取我的群组错误,code:\(code), msg: \(msg)")
        }
    }
    
    /// 根据id检测是否为好友关系
    /// - Parameter userID: 用户id
    /// - Returns: 是否为好友关系
    public func checkFriendBy(userID: String) -> Observable<Bool> {
        return Observable<Bool>.create { observer in
            Self.shared.imManager.checkFriend([userID]) { resultInfos in
                observer.onNext(resultInfos?.first?.result == 1)
                observer.onCompleted()
            } onFailure: { (code: Int, msg: String?) in
                observer.onError(NetError(code: code, message: msg))
            }
            return Disposables.create()
        }
    }
    
    /// 根据id查找用户
    /// - Parameter ids: 用户id
    /// - Returns: 第一个用户id
    public func getFriendsBy(id: String) -> Observable<FullUserInfo?> {
        return Observable<FullUserInfo?>.create { observer in
            Self.shared.imManager.getSpecifiedFriendsInfo([id]) { users in
                observer.onNext(users?.first?.toFullUserInfo())
                observer.onCompleted()
            } onFailure: { (code: Int, msg: String?) in
                observer.onError(NetError(code: code, message: msg))
            }
            return Disposables.create()
        }
    }
    
    public func getFriendsInfo(userIDs: [String], completion: @escaping CallBack.FullUserInfosReturnVoid) {
        Self.shared.imManager.getSpecifiedFriendsInfo(userIDs) { users in
            let r = users?.compactMap({ $0.toFullUserInfo() })
            completion(r ?? [])
        }
    }
    
    /// 获取好友申请列表
    /// - Parameter completion: 申请数组
    public func getFriendReqList(completion: @escaping ([FriendApplication]) -> Void) {
        Self.shared.imManager.getFriendReqListAsRecipientWith(onSuccess: { (applications: [CRIMFriendReq]?) in
            let arr = applications ?? []
            let ret = arr.compactMap { $0.toFriendApplication() }
            completion(ret)
        })
    }
    
    /// 接受好友申请
    /// - Parameters:
    ///   - uid: 指定好友ID
    ///   - handleMsg: 处理理由
    ///   - completion: 响应消息
    public func acceptFriendReq(uid: String, handleMsg: String? = nil, completion: @escaping (String?) -> Void) {
        Self.shared.imManager.acceptFriendReq(uid, handleMsg: handleMsg ?? "") { r in
            completion(nil)
        } onFailure: { code, msg in
            print("接受好友申请,code:\(code), msg: \(msg)")
            completion(msg)
        }
    }
    
    public func refuseFriendReq(uid: String, handleMsg: String? = nil, completion: @escaping (String?) -> Void) {
        
        Self.shared.imManager.refuseFriendReq(uid, handleMsg: handleMsg ?? "") { r in
            completion(nil)
        } onFailure: { code, msg in
            print("拒绝好友申请,code:\(code), msg: \(msg)")
            completion(msg)
        }
    }
    
    public func getGrpReqList(completion: @escaping ([GroupApplicationInfo]) -> Void) {
        Self.shared.imManager.getGrpReqListAsRecipientWith(onSuccess: { (applications: [CRIMGroupReqInfo]?) in
            let arr = applications ?? []
            let ret = arr.compactMap { $0.toGroupApplicationInfo() }
            completion(ret)
        })
    }
    
    public func getFriendList(completion: @escaping ([FullUserInfo]) -> Void) {
        Self.shared.imManager.getFriendListWith(onSuccess: { friends in
            let arr = friends ?? []
            let ret = arr.compactMap { $0.toFullUserInfo() }
            completion(ret)
        })
    }
    
    public func acceptGrpReq(groupID: String, fromUserId: String, handleMsg: String? = nil, completion: @escaping (String?) -> Void) {
        
        Self.shared.imManager.acceptGrpReq(groupID, fromUserId: fromUserId, handleMsg: handleMsg ?? "") { r in
            completion(nil)
        } onFailure: { code, msg in
            print("接受群申请,code:\(code), msg: \(msg)")
            completion(msg)
        }
    }
    
    public func refuseGrpReq(groupID: String, fromUserId: String, handleMsg: String? = nil, completion: @escaping (String?) -> Void) {
        Self.shared.imManager.refuseGrpReq(groupID, fromUserId: fromUserId, handleMsg: handleMsg ?? "") { r in
            completion(nil)
        } onFailure: { code, msg in
            print("拒绝群申请,code:\(code), msg: \(msg)")
            completion(msg)
        }
    }
    
    public func getGrpMemberList(groupId: String, filter: GroupMemberFilter = .member, offset: Int, count: Int, onSuccess: @escaping CallBack.GroupMembersReturnVoid) {
        Self.shared.imManager.getGrpMemberList(groupId,
                                                 filter: CRIMGroupMemberFilter(rawValue: filter.rawValue)!,
                                                 offset: offset,
                                                 count: count) { (memberInfos: [CRIMGroupMemberInfo]?) in
            let members: [GroupMemberInfo] = memberInfos?.compactMap { $0.toGroupMemberInfo() } ?? []
            onSuccess(members)
        }
    }
    
    public func getGrpMembersInfo(groupId: String, uids: [String], onSuccess: @escaping CallBack.GroupMembersReturnVoid) {

        Self.shared.imManager.getSpecifiedGrpMembersInfo(groupId, userIDs: uids) { (groupMembers: [CRIMGroupMemberInfo]?) in
            let members: [GroupMemberInfo] = groupMembers?.compactMap { $0.toGroupMemberInfo() } ?? []
            onSuccess(members)
        } onFailure: { code, msg in
            print("获取组成员信息失败:\(code),\(msg)")
        }
    }
    
    public func getGrpInfo(groupIds: [String], onSuccess: @escaping CallBack.GroupInfosReturnVoid) {
        Self.shared.imManager.getSpecifiedGrpsInfo(groupIds) { (groupInfos: [CRIMGroupInfo]?) in
            let groups: [GroupInfo] = groupInfos?.compactMap { $0.toGroupInfo() } ?? []
            onSuccess(groups)
        } onFailure: { code, msg in
            print("获取组信息失败:\(code), \(msg)")
        }
    }
    
    public func setGrpInfo(group: GroupInfo, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setGrpInfo(group.toCRIMGroupInfo(), onSuccess: onSuccess) { code, msg in
            print("更新群信息失败：\(code), \(msg)")
        }
    }
    
    public func setGrpVerification(groupId: String, type: GroupVerificationType, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setGrpVerification(groupId, needVerification: CRIMGroupVerificationType(rawValue: type.rawValue) ?? .allNeedVerification, onSuccess: onSuccess) { code, msg in
            print("更新群验证失败：\(code), \(msg)")
        }
    }
    
    public func setGrpMemberRoleLevel(groupId: String, userID: String, roleLevel: GroupMemberRole, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setGrpMemberRoleLevel(groupId, userID: userID, roleLevel: CRIMGroupMemberRole(rawValue: roleLevel.rawValue) ?? .member, onSuccess: onSuccess) { code, msg in
            print("设置身份失败：\(code), \(msg)")
        }
    }
    
    public func setGrpMemberNicknameOf(userid: String, inGroupId: String, with name: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setGrpMemberNickname(inGroupId, userID: userid, groupNickname: name, onSuccess: onSuccess) { code, msg in
            print("设置群成员昵称失败：\(code), \(msg)")
        }
    }
    
    public func transferOwner(groupId: String, to userId: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.transferGrpOwner(groupId, newOwner: userId, onSuccess: onSuccess) { code, msg in
            print("转移拥有者失败：\(code), \(msg)")
        }
    }
    
    public func dismissGrp(id: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.dismissGrp(id, onSuccess: onSuccess) { code, msg in
            print("解散群聊失败:\(code), \(msg)")
        }
    }
    
    public func quitGrp(id: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.quitGrp(id, onSuccess: onSuccess) { code, msg in
            print("退出群聊失败:\(code), \(msg)")
        }
    }
    
    public func joinGrp(id: String, reqMsg: String?, joinSource: JoinSource = .search, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.joinGrp(id, reqMsg: reqMsg, joinSource: CRIMJoinType(rawValue: Int32(joinSource.rawValue))!, onSuccess: onSuccess) { code, msg in
            print("加入群聊失败:\(code), \(msg)")
        }
    }
    
    public func inviteUsersToGrp(groupId: String, uids: [String], onSuccess: @escaping CallBack.VoidReturnVoid) {
        Self.shared.imManager.inviteUser(toGrp: groupId, reason: "", userIDs: uids) { r in
            onSuccess()
        } onFailure: { code, msg in
            print("邀请好友加入失败：\(code), \(msg)")
        }
    }
    
    public func kickGrpMember(groupId: String, uids: [String], onSuccess: @escaping CallBack.VoidReturnVoid) {
        Self.shared.imManager.kickGrpMember(groupId, reason: "", userIDs: uids) { r in
            onSuccess()
        } onFailure: { code, msg in
            print("邀请好友加入失败：\(code), \(msg)")
        }
    }
    
    public func addFriend(uid: String, reqMsg: String?, onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: CallBack.ErrorOptionalReturnVoid? = nil) {
        Self.shared.imManager.addFriend(uid, reqMessage: reqMsg, onSuccess: onSuccess, onFailure: onFailure)
    }
    
    public func deleteFriend(uid: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.deleteFriend(uid, onSuccess: onSuccess)
    }
    
    public func blockUser(uid: String, blocked: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        if blocked {
            Self.shared.imManager.add(toBlackList: uid, onSuccess: onSuccess)
        } else {
            Self.shared.imManager.remove(fromBlackList: uid, onSuccess: onSuccess)
        }
    }
    
    public func getBlackList(onSuccess: @escaping CallBack.BlackListOptionalReturnVoid) {
        Self.shared.imManager.getBlackListWith { (blackUsers: [CRIMBlackInfo]?) in
            let result = (blackUsers ?? []).compactMap { $0.toBlackInfo() }
            onSuccess(result)
        } onFailure: { code, msg in
            print("获取黑名单失败：\(code), \(msg)")
        }
    }
    
    public func removeFromBlackList(uid: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.remove(fromBlackList: uid, onSuccess: onSuccess) { code, msg in
            print("移除黑名单失败：\(code), \(msg)")
        }
    }
    
    public func setFriend(uid: String, remark: String?, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setFriendRemark(uid, remark: remark, onSuccess: onSuccess) { code, msg in
            print("设置好友备注失败:\(code), \(msg)")
        }
    }
}

// MARK: - 消息方法

extension IMController {
    public func getAllConversationList(completion: @escaping ([ConversationInfo]) -> Void) {
        Self.shared.imManager.getAllConversationListWith { (conversations: [CRIMConversationInfo]?) in
            let arr = conversations ?? []
            let ret = arr.compactMap { $0.toConversationInfo() }
            completion(ret)
        }
    }
    
    /// 删除指定会话（服务器和本地均删除）
    public func deleteConversation(conversationID: String, completion: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.deleteConversationAndDeleteAllMsg(conversationID, onSuccess: completion) { code, msg in
            print("清除指定会话失败:\(code) - \(msg)")
        }
    }
    
    public func getTotalUnreadMsgCount(completion: ((Int) -> Void)?) {
        Self.shared.imManager.getTotalUnreadMsgCountWith(onSuccess: completion, onFailure: nil)
    }
    
    public func getConversationRecvMsgOpt(conversationIds: [String], completion: (([ConversationNotDisturbInfo]?) -> Void)?) {
        Self.shared.imManager.getConversationRecvMsgOpt(conversationIds) { (conversationInfos: [CRIMConversationNotDisturbInfo]?) in
            let arr = conversationInfos?.compactMap { $0.toConversationNotDisturbInfo() }
            completion?(arr)
        }
    }
    
    public func setConversationRecvMsgOpt(conversationID: String, status: ReceiveMessageOpt, completion: ((String?) -> Void)?) {
        let opt: CRIMReceiveMessageOpt
        switch status {
        case .receive:
            opt = .receive
        case .notReceive:
            opt = .notReceive
        case .notNotify:
            opt = .notNotify
        }
        
        Self.shared.imManager.setConversationRecvMsgOpt(conversationID, status: opt, onSuccess: completion) { code, msg in
            print("修改免打扰状态失败:\(code), \(msg)")
        }
    }
    
    public func setGrpLookMemberInfo(id: String, rule: Int, completion: ((String?) -> Void)?) {
        Self.shared.imManager.setGrpLookMemberInfo(id, rule: Int32(rule), onSuccess: completion) { code, msg in
            print("pin conversation failed: \(code), \(msg)")
        }
    }
    
    public func setGrpApplyMemberFriend(id: String, rule: Int, completion: ((String?) -> Void)?) {
        Self.shared.imManager.setGrpApplyMemberFriend(id, rule: Int32(rule), onSuccess: completion) { code, msg in
            print("pin conversation failed: \(code), \(msg)")
        }
    }
    
    public func pinConversation(id: String, isPinned: Bool, completion: ((String?) -> Void)?) {
        Self.shared.imManager.pinConversation(id, isPinned: !isPinned, onSuccess: completion) { code, msg in
            print("pin conversation failed: \(code), \(msg)")
        }
    }
    
    public func changeGrpMute(groupID: String, isMute: Bool, completion: ((String?) -> Void)?) {
        Self.shared.imManager.changeGrpMute(groupID, isMute: isMute, onSuccess: completion) { code, msg in
            print("修改全体禁言状态失败:\(code), \(msg)")
        }
    }
    
    public func changeGrpMemberMute(groupID: String, userID: String, seconds: Int, completion: ((String?) -> Void)?) {
        Self.shared.imManager.changeGrpMemberMute(groupID, userID: userID, mutedSeconds: seconds, onSuccess: completion) { code, msg in
            print("修改禁言状态失败:\(code), \(msg)")
        }
    }
    
    public func getHistoryMsgList(conversationID: String,
                                      conversationType: ConversationType = .c1v1,
                                      startCliendMsgId: String?,
                                      lastMinSeq: Int = 0,
                                      count: Int = 50,
                                      completion: @escaping SeqMessagesCallBack) {
        
        let opts = CRIMGetAdvancedHistoryMessageListParam()
        opts.conversationID = conversationID
        opts.lastMinSeq = lastMinSeq
        opts.startClientMsgID = startCliendMsgId
        opts.count = count
        
        Self.shared.imManager.getAdvancedHistoryMsgList(opts) { msgListInfo in
            let arr = msgListInfo?.messageList.compactMap({ $0.toMessageInfo() }) ?? []
            completion(msgListInfo?.lastMinSeq ?? 0, arr)
        }
    }
    
    public func getHistoryMsgListReverse(conversationID: String,
                                             conversationType: ConversationType = .c1v1,
                                             startCliendMsgId: String?,
                                             lastMinSeq: Int = 0,
                                             count: Int = 50,
                                             completion: @escaping SeqMessagesCallBack) {
        let opts = CRIMGetAdvancedHistoryMessageListParam()
        opts.conversationID = conversationID
        opts.lastMinSeq = lastMinSeq
        opts.startClientMsgID = startCliendMsgId
        opts.count = count

        Self.shared.imManager.getAdvancedHistoryMsgListReverse(opts) { msgListInfo in
            let arr = msgListInfo?.messageList.compactMap({ $0.toMessageInfo() }) ?? []
            completion(msgListInfo?.lastMinSeq ?? 0, arr)
        } onFailure: { code, msg in
            print("getHistoryMsgListReverse error: \(code), \(msg)")
        }
    }
    
    public func sendMsg(message: MessageInfo, to recvID: String, conversationType: ConversationType = .c1v1, onComplete: @escaping CallBack.MessageReturnVoid) {
        sendHelper(message: message.toCRIMMessageInfo(), to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    private func sendHelper(message: CRIMMessageInfo,
                            to recvID: String,
                            conversationType: ConversationType,
                            onComplete: @escaping CallBack.MessageReturnVoid) {
        var model = message.toMessageInfo()
        model.isRead = false
        if conversationType == .c1v1 {
            Self.shared.imManager.sendMsg(message, recvID: recvID, groupID: nil, offlinePushInfo: message.offlinePush) { (newMessage: CRIMMessageInfo?) in
                if let respMessage = newMessage {
                    onComplete(respMessage.toMessageInfo())
                } else {
                    model.status = .sendSuccess
                    onComplete(model)
                }
            } onProgress: { (progress: Int) in
                print("sending message progress: \(progress)")
            } onFailure: { [weak self] (errCode: Int, msg: String?) in
                print("send message error:", msg)
                var customMessage: MessageInfo?
                
                if conversationType == .c1v1 {
                    if errCode == SDKError.blockedByFriend.rawValue {
//                        customMessage = self?.createCustomMsg(customType: .blockedByFriend, data: [:])
                    } else if errCode == SDKError.deletedByFriend.rawValue {
                        customMessage = self?.createCustomMsg(customType: .deletedByFriend, data: [:])
                    }
                }
                model.status = .sendFailure
                onComplete(model)
                
                if customMessage != nil {
                    Self.shared.imManager.insertSingleMsg(toLocalStorage: customMessage!.toCRIMMessageInfo(), recvID: recvID, sendID: model.sendID, onSuccess: nil, onFailure: nil)
                    customMessage?.recvID = recvID
                    print("type:\(customMessage?.customElem?.type)")
                    onComplete(customMessage!)
                }
            }
        }
        
        if conversationType == .group {
            Self.shared.imManager.sendMsg(message, recvID: nil, groupID: recvID, offlinePushInfo: message.offlinePush) { (newMessage: CRIMMessageInfo?) in
                if let respMessage = newMessage {
                    onComplete(respMessage.toMessageInfo())
                } else {
                    model.status = .sendSuccess
                    onComplete(model)
                }
            } onProgress: { (progress: Int) in
                print("sending message progress: \(progress)")
            } onFailure: { (_: Int, msg: String?) in
                print("send message error:", msg)
                model.status = .sendFailure
                onComplete(model)
            }
        }
    }
    
    private func sendCRIMMsg(message: CRIMMessageInfo,
                                to recvID: String,
                                conversationType: ConversationType,
                                onComplete: @escaping CallBack.MessageReturnVoid) {
        
        let model = message.toMessageInfo()
        
        if let desc = model.offlinePushInfo.desc, desc.isEmpty {
            let push = OfflinePushInfo()
            push.title = "你收到了一条消息"
            push.desc = "你收到了一条消息"
            message.offlinePush = push.toCRIMOfflinePushInfo()
        }
        
        model.isRead = false
        sendHelper(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func typingStatusUpdate(recvID: String, msgTips: String) {
        Self.shared.imManager.typingStatusUpdate(recvID, msgTip: msgTips, onSuccess: nil)
    }
    
    public func sendTextMsg(text: String,
                                quoteMessage: MessageInfo? = nil,
                                to recvID: String,
                                conversationType: ConversationType,
                                sending: CallBack.MessageReturnVoid,
                                onComplete: @escaping CallBack.MessageReturnVoid) {
        let message: CRIMMessageInfo
        if let quoteMessage = quoteMessage {
            let quote = quoteMessage.toCRIMMessageInfo()
            message = CRIMMessageInfo.createQuoteMsg(text, message: quote)
        } else {
            message = CRIMMessageInfo.createTextMsg(text)
        }
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func createAtAllFlag(displayText: String) -> AtInfo {
        let all = CRIMMessageInfo.create(atAllFlag: displayText)
        
        return all.toAtInfo()
    }
    
    public func sendAtTextMsg(text: String,
                                  atUsers: [AtInfo] = [],
                                  quoteMessage: MessageInfo? = nil,
                                  to recvID: String,
                                  conversationType: ConversationType,
                                  sending: CallBack.MessageReturnVoid,
                                  onComplete: @escaping CallBack.MessageReturnVoid) {
        
        var quote: CRIMMessageInfo?
        
        if let quoteMessage {
            quote = quoteMessage.toCRIMMessageInfo()
        }
        
        var message = CRIMMessageInfo.createText(atMsg: text,
                                                atUsersID: atUsers.map({ $0.atUserID! }),
                                                atUsersInfo: atUsers.map({ info in
            let atUserInfo = CRIMAtInfo()
            atUserInfo.atUserID = info.atUserID
            atUserInfo.groupNickname = info.groupNickname
            
            return atUserInfo
        }), message: quote)
        
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendImageMsg(path: String,
                                 to recvID: String,
                                 conversationType: ConversationType,
                                 sending: CallBack.MessageReturnVoid,
                                 onComplete: @escaping CallBack.MessageReturnVoid) {
        
        let message = CRIMMessageInfo.createImageMsg(fromFullPath: path)
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendVideoMsg(path: String,
                                 duration: Int,
                                 snapshotPath: String,
                                 to recvID: String,
                                 conversationType: ConversationType,
                                 sending: CallBack.MessageReturnVoid,
                                 onComplete: @escaping CallBack.MessageReturnVoid) {
        
        let message = CRIMMessageInfo.createVideoMsg(fromFullPath: path,
                                                        videoType: String(path.split(separator: ".").last ?? "mp4"),
                                                        duration: duration,
                                                        snapshotPath: snapshotPath)
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendAudioMsg(path: String,
                                 duration: Int,
                                 to recvID: String,
                                 conversationType: ConversationType,
                                 sending: CallBack.MessageReturnVoid,
                                 onComplete: @escaping CallBack.MessageReturnVoid) {
        let message = CRIMMessageInfo.createSoundMsg(fromFullPath: path, duration: duration)
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendCardMsg(card: CardElem,
                                to recvID: String,
                                conversationType: ConversationType,
                                sending: CallBack.MessageReturnVoid,
                                onComplete: @escaping CallBack.MessageReturnVoid) {
        let message = CRIMMessageInfo.createCardMsg(card.toCRIMCardElem())
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendLocation(latitude: Double,
                             longitude: Double,
                             desc: String,
                             to recvID: String,
                             conversationType: ConversationType,
                             sending: CallBack.MessageReturnVoid,
                             onComplete: @escaping CallBack.MessageReturnVoid) {
        let message = CRIMMessageInfo.createLocationMsg(desc, latitude: latitude, longitude: longitude)
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    
    public func sendFileMsg(filePath: String,
                                to recvID: String,
                                conversationType: ConversationType,
                                sending: CallBack.MessageReturnVoid,
                                onComplete: @escaping CallBack.MessageReturnVoid) {
        let message = CRIMMessageInfo.createFileMsg(fromFullPath: filePath,
                                                       fileName: (filePath as NSString).lastPathComponent)
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func sendMergeMsg(messages: [MessageInfo],
                                 title: String, // let title = conversationType != .c1v1 ? "群聊的聊天记录" : "\(conversation.showName!)与\(currentUserRelay.value!.nickname!)的聊天记录"
                                 to recvID: String,
                                 conversationType: ConversationType,
                                 sending: CallBack.MessageReturnVoid,
                                 onComplete: @escaping CallBack.MessageReturnVoid) {
        
        let summaryList = messages.map { ($0.senderNickname ?? "") + ":" +  MessageHelper.getSummary(by: $0) }
        
        let message = CRIMMessageInfo.createMergeMsg(messages.map { $0.toCRIMMessageInfo() }, title: title, summaryList: summaryList)
        
        message.status = .sending
        sending(message.toMessageInfo())
        sendCRIMMsg(message: message, to: recvID, conversationType: conversationType, onComplete: onComplete)
    }
    
    public func revokeMsg(conversationID: String, clientMsgID: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.revokeMsg(conversationID, clientMsgID: clientMsgID, onSuccess: onSuccess) { code, msg in
            print("消息撤回失败:\(code), msg:\(msg)")
        }
    }
    
    public func deleteMsg(conversation: String, clientMsgID: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid, onFailure: @escaping CallBack.ErrorOptionalReturnVoid) {
        Self.shared.imManager.deleteMsg(conversation, clientMsgID: clientMsgID, onSuccess: onSuccess) { code, msg in
         print("消息删除失败:\(code), msg:\(msg)")
            onFailure(code, msg)
        }
    }
    
    public func createCustomMsg(customType: CustomMessageType, data: [String: Any]) -> MessageInfo {
        let dataStr = String.init(data: try! JSONSerialization.data(withJSONObject: ["customType": customType.rawValue,
                                                                                     "data": data]),
                                  encoding: .utf8)!
        return CRIMMessageInfo.createCustomMsg(dataStr, extension: nil).toMessageInfo()
    }
    
    public func markConversationMsg(byConID: String, msgIDList: [String], onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.markConversationMsg(asRead: byConID, onSuccess: onSuccess)
    }
    
    public func createGrpConversation(users: [UserInfo], groupName: String = "", onSuccess: @escaping CallBack.GroupInfoOptionalReturnVoid) {
        
        let nickname = currentUserRelay.value?.nickname
        
        let groupInfo = CRIMGroupBaseInfo()
        groupInfo.groupName = groupName.isEmpty ? nickname?.append(string: "创建的群聊".innerLocalized()) : groupName
        
        let createInfo = CRIMGroupCreateInfo()
        createInfo.memberUserIDs = users.compactMap({ $0.userID.isEmpty ? nil : $0.userID })
        createInfo.groupInfo = groupInfo
        
        Self.shared.imManager.createGrp(createInfo) { (groupInfo: CRIMGroupInfo?) in
            print("创建群聊成功")
            onSuccess(groupInfo?.toGroupInfo())
        } onFailure: { code, msg in
            print("创建群聊成功录失败:\(code), \(msg)")
        }
    }
    
    public func clear1v1HistoryMsgs(conversationID: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.clearConversationAndDeleteAllMsg(conversationID) { r in
            onSuccess(r)
        } onFailure: { code, msg in
            print("清空群聊天记录失败:\(code), \(msg)")
        }
    }
    
    public func clearGrpHistoryMsgs(conversationID: String, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.clearConversationAndDeleteAllMsg(conversationID) { r in
            
        } onFailure: { code, msg in
            print("清空群聊天记录失败:\(code), \(msg)")
        }
    }
    
    public func deleteAllMsgFromLocalAndSvr(onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.deleteAllMsgFromLocalAndSvrWith(onSuccess: onSuccess) { code, msg in
            print("清空群聊天记录失败:\(code), \(msg)")
        }
    }
    
    public func uploadFile(fullPath: String, onProgress: @escaping CallBack.ProgressReturnVoid, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.uploadFile(fullPath,
                                         name: nil,
                                         cause: nil) { total, current, save in
            let p = CGFloat(current) / CGFloat(total)
            //print("uploadFile total:\(total), current:\(current), save:\(save)")
            onProgress(p)
        } onCompletion: { c, u, t  in
            
        } onSuccess: { r in
            let dic = try! JSONSerialization.jsonObject(with: r!.data(using: .utf8)!, options: .allowFragments) as! [String: Any]
            
            onSuccess(dic["url"] as! String)
        } onFailure: { code, msg in
            print("上传文件失败:\(code), \(msg)")
        }
    }
    
    public func searchRecord(param: SearchParam, onSuccess: @escaping CallBack.SearchResultInfoOptionalReturnVoid) {
        Self.shared.imManager.searchLocalMsgs(param.toCRIMSearchParam()) { (result: CRIMSearchResultInfo?) in
            onSuccess(result?.toSearchResultInfo())
        }
    }
    
    public func searchGrps(param: SearchGroupParam, onSuccess: @escaping CallBack.GroupInfosReturnVoid) {
        Self.shared.imManager.searchGrps(param.toCRIMSearchGroupParam()) { result in
            
            let arr = result?.compactMap { $0.toGroupInfo() } ?? []
            onSuccess(arr)
        }
    }
    
    public func searchFriends(param: SearchUserParam, onSuccess: @escaping CallBack.SearchUsersInfoOptionalReturnVoid) {
        Self.shared.imManager.searchFriends(param.toCRIMSearchUserParam()) { result in
            
            let arr = result?.compactMap { $0.toSearchFriendsInfo() } ?? []
            onSuccess(arr)
        }
    }
    
    public func insertCustomMsg(conversationType: ConversationType,
                                    msg: MessageInfo,
                                    recvID: String,
                                    onSuccess: @escaping CallBack.MessageReturnVoid) {
        
        if conversationType == .c1v1 {
            Self.shared.imManager.insertSingleMsg(toLocalStorage: msg.toCRIMMessageInfo(),
                                                      recvID: recvID,
                                                      sendID: getLoginUserID(),
                                                      onSuccess: { message in
                onSuccess(message!.toMessageInfo())
            }) { code, msg in
                
                print("单聊插入本地失败:\(code), \(msg)")
            }
        } else {
            Self.shared.imManager.insertGrpMsg(toLocalStorage: msg.toCRIMMessageInfo(),
                                                     groupID: recvID,
                                                     sendID: getLoginUserID(),
                                                     onSuccess: { message in
                onSuccess(message!.toMessageInfo())
            }) { code, msg in
                print("群聊插入本地失败:\(code), \(msg)")
            }
        }
    }
}

// MARK: - 会话方法

extension IMController {
    
    public func getConversation(sessionType: ConversationType = .undefine, sourceId: String = "", conversationID: String = "", onSuccess: @escaping CallBack.ConversationInfoOptionalReturnVoid) {
        
        if !conversationID.isEmpty {
            
            Self.shared.imManager.getMultipleConversation([conversationID]) { conversations in
                onSuccess(conversations?.first?.toConversationInfo())
            } onFailure: { code, msg in
                print("创建会话失败:\(code), .msg:\(msg)")
            }
            
        } else {
            
            let conversationType = CRIMConversationType(rawValue: sessionType.rawValue) ?? CRIMConversationType.undefine
            
            Self.shared.imManager.getOneConversation(withSessionType: conversationType, sourceID: sourceId) { (conversation: CRIMConversationInfo?) in
                onSuccess(conversation?.toConversationInfo())
            } onFailure: { code, msg in
                print("创建会话失败:\(code), .msg:\(msg)")
            }
        }
    }
    
    
    public func setGlobalRecvMsgOpt(op: ReceiveMessageOpt, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        let opt = CRIMReceiveMessageOpt(rawValue: op.rawValue) ?? CRIMReceiveMessageOpt.receive
        Self.shared.imManager.setGlobalRecvMsgOpt(opt, onSuccess: onSuccess) { code, msg in
            print("设置全局免打扰失败:\(code), .msg:\(msg)")
        }
    }
    
    public func setOneConversationPrivateChat(conversationID: String, isPrivate: Bool, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        
        Self.shared.imManager.setConversationPrivateChat(conversationID, isPrivate: isPrivate, onSuccess: onSuccess) { code, msg in
            print("设置阅后即焚失败:\(code), .msg:\(msg)")
        }
    }
    
    public func setBurnDuration(conversationID: String, burnDuration: Int, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setConversationBurnDuration(conversationID, duration: burnDuration, onSuccess: onSuccess)
    }
}

// MARK: - User方法

extension IMController {
    /// 获取当前登录用户信息
    public func getSelfInfo(onSuccess: @escaping CallBack.UserInfoOptionalReturnVoid) {
        Self.shared.imManager.getSelfInfoWith { [weak self] (userInfo: CRIMUserInfo?) in
            let user = userInfo?.toUserInfo()
            self?.currentUserRelay.accept(user)
            onSuccess(user)
        } onFailure: { code, msg in
            print("拉取登录用户信息失败:\(code), msg:\(msg)")
        }
    }
    
    public func getUserInfo(uids: [String], onSuccess: @escaping CallBack.UsersInfoOptionalReturnVoid) {
        Self.shared.imManager.getUsersInfo(uids) { userInfos in
            let users = userInfos?.compactMap { $0.toUserInfo() } ?? []
            onSuccess(users)
        }
    }
    
    public func setSelfInfo(userInfo: UserInfo, onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.setSelfInfo(userInfo.toCRIMUserInfo(), onSuccess: onSuccess) { code, msg in
            print("更新个人信息失败:\(code), \(msg)")
        }
    }
    
    public func logout(onSuccess: @escaping CallBack.StringOptionalReturnVoid) {
        Self.shared.imManager.logoutWith(onSuccess: onSuccess) { code, msg in
            print("退出登录失败:\(code), \(msg)")
        }
    }
    
    public func getLoginUserID() -> String {
        return imManager.getLoginUserID()
    }
}

// MARK: - Listener

extension IMController: CRIMFriendshipListener {
    @objc public func onFriendReqAdded(_ application: CRIMFriendReq) {
        friendApplicationChangedSubject.onNext(application.toFriendApplication())
    }
    
    @objc public func onFriendInfoChanged(_ info: CRIMFriendInfo) {
        friendInfoChangedSubject.onNext(info.toFriendInfo())
    }
    
    public func onBlackAdded(_ info: CRIMBlackInfo) {
        onBlackAddedSubject.onNext(info.toBlackInfo())
    }
    
    public func onBlackDeleted(_ info: CRIMBlackInfo) {
        onBlackDeletedSubject.onNext(info.toBlackInfo())
    }
}

// MARK: CRIMGroupListener

extension IMController: CRIMGroupListener {
    public func onGrpMemberAdded(_ memberInfo: CRIMGroupMemberInfo) {
        
    }
    
    public func onGrpReqAdded(_ groupReq: CRIMGroupReqInfo) {
        groupApplicationChangedSubject.onNext(groupReq.toGroupApplicationInfo())
    }
    
    public func onGrpInfoChanged(_ changeInfo: CRIMGroupInfo) {
        groupInfoChangedSubject.onNext(changeInfo.toGroupInfo())
    }
    
    public func onGrpMemberInfoChanged(_ changeInfo: CRIMGroupMemberInfo) {
        groupMemberInfoChange.onNext(changeInfo.toGroupMemberInfo())
    }
    
    public func onJoinedGrpAdded(_ groupInfo: CRIMGroupInfo) {
        joinedGroupAdded.onNext(groupInfo.toGroupInfo())
    }
    
    public func onJoinedGrpDeleted(_ groupInfo: CRIMGroupInfo) {
        
    }
    
    public func onGrpReqDeleted(_ groupInfo: CRIMGroupInfo) {
        joinedGroupDeleted.onNext(groupInfo.toGroupInfo())
    }
}

// MARK: CRIMConversationListener

extension IMController: CRIMConversationListener {
    public func onConversationChanged(_ conversations: [CRIMConversationInfo]) {
        let conversations: [ConversationInfo] = conversations.compactMap {
            $0.toConversationInfo()
        }
        conversationChangedSubject.onNext(conversations)
    }
    
    public func onSyncServerStart() {
        connectionRelay.accept(.syncStart)
    }
    
    public func onSyncServerFinish() {
        connectionRelay.accept(.syncComplete)
    }
    
    public func onSyncServerFailed() {
        connectionRelay.accept(.syncFailure)
    }
    
    public func onNewConversation(_ conversations: [CRIMConversationInfo]) {
        
        let arr = conversations.compactMap { $0.toConversationInfo() }
        newConversationSubject.onNext(arr)
    }
    
    public func onTotalUnreadMsgCountChanged(_ totalUnreadCount: Int) {
        totalUnreadSubject.onNext(totalUnreadCount)
    }
}

// MARK: CRIMAdvancedMsgListener

extension IMController: CRIMAdvancedMsgListener {
    public func onRecvNewMsg(_ msg: CRIMMessageInfo) {
        if msg.contentType.rawValue < 1000,
           msg.contentType != .typing,
           msg.contentType != .revoke,
           msg.contentType != .hasReadReceipt,
           msg.contentType != .groupHasReadReceipt {
            Self.shared.imManager.getOneConversation(withSessionType: msg.sessionType,
                                                     sourceID: msg.sessionType == .c1v1 ? msg.sendID! : msg.groupID!,
                                                     onSuccess: { conversation in
                
                if conversation!.conversationID != self.chatingConversationID,
                   conversation!.unreadCount > 0,
                   conversation!.recvMsgOpt == .receive {
                    
                    self.ringAndVibrate()
                }
            })
        }
        newMsgReceivedSubject.onNext(msg.toMessageInfo())
    }
    
    public func onRecv1v1ReadReceipt(_ receiptList: [CRIMReceiptInfo]) {
        c2cReadReceiptReceived.onNext(receiptList.compactMap { $0.toReceiptInfo() })
    }
    
    public func onRecvGrpReadReceipt(_ groupMsgReceiptList: [CRIMReceiptInfo]) {
        groupReadReceiptReceived.onNext(groupMsgReceiptList.compactMap { $0.toReceiptInfo() })
    }
    
    public func onRecvMsgRevoked(_ messageRevoked: CRIMMessageRevokedInfo) {
        msgRevokeReceived.onNext(messageRevoked.toMessageRevoked())
    }
}

// MARK: - Models

// MARK: 主要模型

public class UserInfo: Codable {
    public var userID: String!
    public var nickname: String?
    public var faceURL: String?
    public var gender: Gender?
    public var phoneNumber: String?
    public var birth: Int?
    public var email: String?
    public var createTime: Int = 0
    public var landline: String? // 座机
    public var ex: String?
    public var globalRecvMsgOpt: ReceiveMessageOpt = .receive
    
    public init(userID: String,
                nickname: String? = nil,
                phoneNumber: String? = nil,
                email: String? = nil,
                faceURL: String? = nil,
                birth: Int? = nil,
                gender: Gender? = nil,
                landline: String? = nil) {
        self.userID = userID
        self.nickname = nickname
        self.phoneNumber = phoneNumber
        self.email = email
        self.faceURL = faceURL
        self.birth = birth
        self.gender = gender
        self.landline = landline
    }
    
    // 业务做选择逻辑
    public var isSelected: Bool = false
}

public class FriendApplication {
    public var fromUserID: String = ""
    public var fromNickname: String?
    public var fromFaceURL: String?
    public var toUserID: String = ""
    public var toNickname: String?
    public var toFaceURL: String?
    public var handleResult: ApplicationStatus = .normal
    public var reqMsg: String?
    public var handlerUserID: String?
    public var handleMsg: String?
    public var handleTime: Int?
}

public class GroupApplicationInfo {
    public var groupID: String = ""
    public var groupName: String?
    public var groupFaceURL: String?
    public var creatorUserID: String = ""
    public var ownerUserID: String = ""
    public var memberCount: Int = 0
    public var userID: String?
    public var nickname: String?
    public var userFaceURL: String?
    public var reqMsg: String?
    public var reqTime: Int?
    public var handleUserID: String?
    public var handledMsg: String?
    public var handledTime: Int?
    public var handleResult: ApplicationStatus = .normal
    public var joinSource: JoinSource = .search
    public var inviterUserID: String?
}

/// 申请状态
public enum ApplicationStatus: Int {
    /// 已拒绝
    case decline = -1
    /// 等待处理
    case normal = 0
    /// 已同意
    case accept = 1
}

/// 消息接收选项
public enum ReceiveMessageOpt: Int, Codable {
    /// 在线正常接收消息，离线时会使用 APNs
    case receive = 0
    /// 不会接收到消息，离线不会有推送通知
    case notReceive = 1
    /// 在线正常接收消息，离线不会有推送通知
    case notNotify = 2
}

public class ConversationInfo {
    public let conversationID: String
    public var userID: String?
    public var groupID: String?
    public var showName: String?
    public var faceURL: String?
    public var recvMsgOpt: ReceiveMessageOpt = .receive
    public var unreadCount: Int = 0
    public var conversationType: ConversationType = .c1v1
    public var latestMsgSendTime: Int = 0
    public var draftText: String?
    public var draftTextTime: Int = 0
    public var isPinned: Bool = false
    public var latestMsg: MessageInfo?
    public var ex: String?
    public var isPrivateChat: Bool = false
    public var burnDuration: Double = 30
    init(conversationID: String) {
        self.conversationID = conversationID
    }
}

open class MessageInfo: Encodable {
    public var clientMsgID: String = ""
    public var serverMsgID: String?
    public var createTime: TimeInterval = 0
    public var sendTime: TimeInterval = 0
    public var sessionType: ConversationType = .c1v1
    public var sendID: String = ""
    public var recvID: String?
    public var handleMsg: String?
    public var msgFrom: MessageLevel = .user
    public var contentType: MessageContentType = .unknown
    public var platformID: Int = 0
    public var senderNickname: String?
    public var senderFaceUrl: String?
    public var groupID: String?
    public var content: String?
    /// 消息唯一序列号
    var seq: Int = 0
    public var isRead: Bool = false // 标记收到的消息，是否已经标记已读
    public var status: MessageStatus = .undefine
    public var attachedInfo: String?
    public var ex: String?
    public var offlinePushInfo: OfflinePushInfo = .init()
    public var textElem: TextElem?
    public var pictureElem: PictureElem?
    public var soundElem: SoundElem?
    public var videoElem: VideoElem?
    public var fileElem: FileElem?
    public var mergeElem: MergeElem?
    public var atTextElem: AtTextElem?
    public var locationElem: LocationElem?
    public var quoteElem: QuoteElem?
    public var customElem: CustomElem?
    public var notificationElem: NotificationElem?
    public var faceElem: FaceElem?
    public var attachedInfoElem: AttachedInfoElem?
    public var hasReadTime: Double = 0
    public var cardElem: CardElem?
    public var typingElem: TypingElem?

    // 客户端调用的
    public var isPlaying = false
    public var isSelected = false
    public var isAnchor = false // 搜索聊天记录的消息
    
    public func getAbstruct() -> String? {
        switch contentType {
        case .text:
            return content
        case .quote:
            return quoteElem?.text
        case .at:
            return atTextElem?.atText
        default:
            return contentType.abstruct
        }
    }
    
    public func isCalling() -> Bool {
        if let data = customElem?.data {
            let obj = try! JSONSerialization.jsonObject(with: data.data(using: .utf8)!) as! [String: Any]
            if let type = obj["customType"] as? Int, type == 901 {
                return true
            }
        }
        
        return false
    }
    
    public func isTyping() -> Bool {
        guard contentType == .typing else { return false }
        
        return typingElem?.msgTips == "yes"
    }
}

extension MessageInfo {
    public var revokedInfo: MessageRevoked {
        assert(contentType == .revoke)
        let detail = notificationElem?.detail ?? content
        return JsonTool.fromJson(detail!, toClass: MessageRevoked.self) ?? MessageRevoked()
    }
}

public class GroupMemberBaseInfo: Encodable {
    public var userID: String?
    public var roleLevel: GroupMemberRole = .member
}

public enum GroupMemberFilter: Int, Codable {
    case all = 0
    /// 群主
    case owner = 1
    case admin = 2
    case member = 3
    case adminAndMember = 4
    case superAndAdmin = 5
}

public enum GroupMemberRole: Int, Codable {
    /// 未定义
    case undefine
    case singleChat = 1
    case owner  = 100
    case admin  = 60
    case member = 20
}

public class GroupMemberInfo: GroupMemberBaseInfo {
    public var groupID: String?
    public var nickname: String?
    public var faceURL: String?
    public var joinTime: Int = 0
    public var joinSource: JoinSource = .search
    public var operatorUserID: String?
    public var inviterUserID: String?
    public var muteEndTime: Double = 0
    public var ex: String?
    
    public override init() {
    }
    
    // 非SDK提供
    public var inviterUserName: String?
}

extension GroupMemberInfo {
    public var isSelf: Bool {
        return userID == IMController.shared.userID
    }
    
    public var joinWay: String {
        switch joinSource {
        case .invited:
            return "\(inviterUserName ?? "")邀请加入"
        case .search:
            return "搜索加入"
        case .QRCode:
            return "扫描二维码加入"
        }
    }
    
    public var roleLevelString: String {
        switch roleLevel {
        case .admin:
            return "管理员"
        case .owner:
            return "创建者"
        default:
            return ""
        }
    }
    
    public var isOwnerOrAdmin: Bool {
        return roleLevel == .owner || roleLevel == .admin
    }
}

public enum MessageContentType: Int, Codable {
    case unknown = -1
    
    // MARK: 消息类型
    
    case text = 101
    case image
    case audio
    case video
    case file
    /// @消息
    case at
    /// 合并消息
    case merge
    /// 名片消息
    case card
    case location
    case custom = 110
    case typing = 113
    case quote = 114
    /// 动图消息
    case face = 115
    case advancedText = 117
    case reactionMessageModifier = 121
    case reactionMessageDeleter = 122

    // MARK: 通知类型
    
    case friendAppApproved = 1201
    case friendAppRejected
    case friendApplication
    case friendAdded
    case friendDeleted
    /// 设置好友备注通知
    case friendRemarkSet
    case blackAdded
    case blackDeleted
    /// 会话免打扰设置通知
    case conversationOptChange = 1300
    case userInfoUpdated = 1303
    /// 会话通知
    case conversationNotification = 1307
    /// 会话不通知
    case conversationNotNotification
    /// oa通知
    case oaNotification = 1400
    case groupCreated = 1501
    /// 更新群信息通知
    case groupInfoSet
    case joinGrpApplication
    case memberQuit
    case groupAppAccepted
    case groupAppRejected
    /// 群主更换通知
    case groupOwnerTransferred
    case memberKicked
    case memberInvited
    case memberEnter
    /// 解散群通知
    case dismissGrp
    /// 群成员被禁言
    case groupMemberMuted = 1512
    /// 群成员被取消禁言
    case groupMemberCancelMuted = 1513
    /// 群禁言
    case groupMuted = 1514
    /// 取消群禁言
    case groupCancelMuted = 1515
    
    case groupMemberInfoSet = 1516
    case groupAnnouncement = 1519
    case groupSetName = 1520
    /// 阅后即焚
    case privateMessage = 1701
    
    case revoke = 2101
    /// 单聊已读回执
    case hasReadReceipt = 2150
    /// 群聊消息回执
    case groupHasReadReceipt = 2155
    
    public var abstruct: String? {
        switch self {
        case .image:
            return "[图片]"
        case .audio:
            return "[语音]"
        case .video:
            return "[视频]"
        case .file:
            return "[文件]"
        case .card:
            return "[名片]"
        case .location:
            return "[定位]"
        case .face:
            return "[自定义表情]"
        case .custom:
            return "[自定义消息]"
        default:
            return nil
        }
    }
}

public enum MessageStatus: Int, Codable {
    case undefine = 0
    case sending
    case sendSuccess
    case sendFailure
    case deleted
    case revoke
}

public enum MessageLevel: Int, Codable {
    case undefine = -1
    case user = 100
    case system = 200
}

public enum ConversationType: Int, Codable {
    /// 未定义
    case undefine
    /// 单聊
    case c1v1
    /// 群聊
    case group = 3
    /// 通知
    case notification
}

public enum GroupStatus: Int, Codable {
    case ok = 0
    case beBan = 1
    case dismissed = 2
    case muted = 3
}

public enum JoinSource: Int, Codable {
    case invited = 2 /// 通过邀请
    case search = 3 /// 通过搜索
    case QRCode = 4 /// 通过二维码
}

public enum GroupVerificationType: Int, Codable {
    case applyNeedVerificationInviteDirectly = 0 /// 申请需要同意 邀请直接进
    case allNeedVerification = 1 /// 所有人进群需要验证，除了群主管理员邀
    case directly = 2 /// 直接进群
    
}

public enum LogLevel: Int32, Codable {
    case error = 2
    case warn
    case info
    case debug
}

public class GroupBaseInfo: Encodable {
    public var groupName: String?
    public var introduction: String?
    public var notification: String?
    public var faceURL: String?
}

public class GroupInfo: GroupBaseInfo {
    public var groupID: String = ""
    public var ownerUserID: String?
    public var createTime: Int = 0
    public var memberCount: Int = 0
    public var creatorUserID: String?
    public var status: GroupStatus = .ok
    public var needVerification: GroupVerificationType = .applyNeedVerificationInviteDirectly
    public var lookMemberInfo: Int = 0
    public var applyMemberFriend: Int = 0
    
    public var isMine: Bool {
        return ownerUserID == IMController.shared.userID
    }
    
    public func needVerificationText() -> String {
        
        if (needVerification == .allNeedVerification) {
            return "需要发送验证信息".innerLocalized()
        } else if (needVerification == .directly) {
            return "允许任何人加群".innerLocalized()
        }
        return "群成员邀请无需验证".innerLocalized()
    }
}

public class ConversationNotDisturbInfo {
    public  let conversationId: String
    public var result: ReceiveMessageOpt = .receive
    init(conversationId: String) {
        self.conversationId = conversationId
    }
}

// MARK: 次要模型

public class FaceElem: Encodable {
    public var index: Int = 0
    public var data: String?
}

public class AttachedInfoElem: Encodable {
    public var groupHasReadInfo: GroupHasReadInfo?
    public var isPrivateChat: Bool = false
    public var burnDuration: Double = 30
    public var hasReadTime: Double = 0
}

public class GroupHasReadInfo: Encodable {
    public var hasReadUserIDList: [String]?
    public var hasReadCount: Int = 0
}

public class NotificationElem: Encodable {
    public var detail: String?

    private(set) var opUser: GroupMemberInfo?
    private(set) var quitUser: GroupMemberInfo?
    private(set) var entrantUser: GroupMemberInfo?
    /// 群改变新群主的信息
    private(set) var groupNewOwner: GroupMemberInfo?
    public private(set) var group: GroupInfo?
    private(set) var kickedUserList: [GroupMemberInfo]?
    private(set) var invitedUserList: [GroupMemberInfo]?
    init(opUser: GroupMemberInfo?, quitUser: GroupMemberInfo?, entrantUser: GroupMemberInfo?, groupNewOwner: GroupMemberInfo?, group: GroupInfo?, kickedUserList: [GroupMemberInfo]?, invitedUserList: [GroupMemberInfo]?) {
        self.opUser = opUser
        self.quitUser = quitUser
        self.entrantUser = entrantUser
        self.groupNewOwner = groupNewOwner
        self.group = group
        self.kickedUserList = kickedUserList
        self.invitedUserList = invitedUserList
    }
    public var detailObject: [String: Any] {
        guard let detail else {
            return [:]
        }
        
        if let obj = try? JSONSerialization.jsonObject(with: detail.data(using: .utf8)!) as? [String : Any] {
            return obj
        }
        
        return [:]
    }
}

public class OfflinePushInfo: Codable {
    public var title: String?
    public var desc: String?
    public var iOSPushSound: String?
    public var iOSBadgeCount: Bool = false
    public var operatorUserID: String?
    public var ex: String?
}

public class TextElem: Codable {
    public var content: String!
}

public class CardElem: Codable {
    public var userID: String!
    public var nickname: String!
    public var faceURL: String?
    public var ex: String?
    
    public init(userID: String, nickname: String, faceURL: String?) {
        self.userID = userID
        self.nickname = nickname
        self.faceURL = faceURL
    }
}

public class TypingElem: Codable {
    public var msgTips: String?
}

public class PictureElem: Codable {
    /// 本地资源地址
    public var sourcePath: String?
    /// 本地图片详情
    public var sourcePicture: PictureInfo?
    /// 大图详情
    public var bigPicture: PictureInfo?
    /// 缩略图详情
    public var snapshotPicture: PictureInfo?
}

public class PictureInfo: Codable {
    public var uuID: String?
    public var type: String?
    public var size: Int = 0
    public var width: CGFloat = 0
    public var height: CGFloat = 0
    /// 图片oss地址
    public var url: String?
}

public class SoundElem: Codable {
    public var uuID: String?
    /// 本地资源地址
    public var soundPath: String?
    /// oss地址
    public var sourceUrl: String?
    public var dataSize: Int = 0
    public var duration: Int = 0
}

public class VideoElem: Codable {
    public var videoUUID: String?
    public var videoPath: String?
    public var videoUrl: String?
    public var videoType: String?
    public var videoSize: Int = 0
    public var duration: Int = 0
    /// 视频快照本地地址
    public var snapshotPath: String?
    /// 视频快照唯一ID
    public var snapshotUUID: String?
    public var snapshotSize: Int = 0
    /// 视频快照oss地址
    public var snapshotUrl: String?
    public var snapshotWidth: CGFloat = 0
    public var snapshotHeight: CGFloat = 0
}

public class FileElem: Encodable {
    public var filePath: String?
    public var uuID: String?
    /// oss地址
    public var sourceUrl: String?
    public var fileName: String?
    public var fileSize: Int = 0
}

public class MergeElem: Encodable {
    public var title: String?
    public var abstractList: [String]?
    public var multiMessage: [MessageInfo]?
}

public class AtTextElem: Encodable {
    public var text: String?
    public var atUserList: [String]?
    public var atUsersInfo: [AtInfo]?
    public var quoteMessage: MessageInfo?
    public var isAtSelf: Bool = false
    
    public var atText: String {
        var temp = text!
        atUsersInfo?.forEach({ info in
            temp = temp.replacingOccurrences(of: "@\(info.atUserID!)",
                                             with: "@\(info.atUserID == IMController.shared.userID ? "我".innerLocalized() : info.groupNickname!)")
        })
        
        return temp
    }
    
    public var atAttributeString: NSAttributedString {
        var attrText = NSMutableAttributedString()
        // 将文本中的@人员替换下
        var texts = text!.split(separator: " ")
        
        guard let atUsersInfo else { return NSAttributedString() }
        texts.forEach({ text in
            let match = atUsersInfo.first { info in
                return "@\(info.atUserID!)" == String(text)
            }
            
            if match != nil {
                // 如有有@标识的人
                let atUserName = match!.atUserID == IMController.shared.userID ?
                "我".innerLocalized() : match!.groupNickname!
                
                attrText.append(NSMutableAttributedString(string: "@\(atUserName) ", attributes: [
                    NSAttributedString.Key.foregroundColor: match!.atUserID == IMController.shared.userID ? UIColor.systemBlue : UIColor.systemBlue]))
            } else {
                attrText.append(NSMutableAttributedString(string: String(" \(text)")))
            }
        })
        
        return attrText
    }
}

public class AtInfo: Encodable {
    public var atUserID: String?
    public var groupNickname: String?
    
    public init(atUserID: String, groupNickname: String) {
        self.atUserID = atUserID
        self.groupNickname = groupNickname
    }
}

public class LocationElem: Encodable {
    public var desc: String?
    public var longitude: Double = 0
    public var latitude: Double = 0
}

public class QuoteElem: Encodable {
    public var text: String?
    public var quoteMessage: MessageInfo?
}

public class CustomElem: Encodable {
    public var data: String?
    public var ext: String?
    public var description: String?
    
    public func value() -> [String: Any]? {
        if let data = data {
            let obj = try! JSONSerialization.jsonObject(with: data.data(using: .utf8)!) as! [String: Any]
            return obj["data"] as? [String: Any]
        }
        
        return nil
    }
    
    public var type: CustomMessageType? {
        if let data = data {
            let obj = try! JSONSerialization.jsonObject(with: data.data(using: .utf8)!) as! [String: Any]
            let t = obj["customType"] as! Int
            
            return CustomMessageType.init(rawValue: t)
        }
        
        return nil
    }
}

public struct BusinessCard: Codable {
    public let faceURL: String?
    public let nickname: String?
    public let userID: String
    public init(faceURL: String?, nickname: String?, userID: String) {
        self.faceURL = faceURL
        self.nickname = nickname
        self.userID = userID
    }
}

public class ReceiptInfo {
    public var userID: String?
    public var groupID: String?
    /// 已读消息id
    public var msgIDList: [String]?
    public var readTime: Int = 0
    public var msgFrom: MessageLevel = .user
    public var contentType: MessageContentType = .hasReadReceipt
    public var sessionType: ConversationType = .undefine
}

public class MessageRevoked: Codable {
    /*
     * 撤回者的id
     */
    public var revokerID: String?
    public var revokerNickname: String?
    
    /*
     * 撤回者的身份：例如：群主，群管理员
     */
    public var revokerRole: GroupMemberRole = .member
    public var clientMsgID: String?
    public var revokeTime: Double = 0
    public var sourceMessageSendTime: Double = 0
    public var sourceMessageSendID: String?
    public var sourceMessageSenderNickname: String?
    public var sessionType: ConversationType = .c1v1
    
    public init(revokerID: String? = nil, revokerNickname: String? = nil, revokerRole: GroupMemberRole = .member, clientMsgID: String? = nil, revokeTime: Double = 0, sourceMessageSendTime: Double = 0, sourceMessageSendID: String? = nil, sourceMessageSenderNickname: String? = nil, sessionType: ConversationType = .c1v1) {
        self.revokerID = revokerID
        self.revokerNickname = revokerNickname
        self.revokerRole = revokerRole
        self.clientMsgID = clientMsgID
        self.revokeTime = revokeTime
        self.sourceMessageSendID = sourceMessageSendID
        self.sourceMessageSenderNickname = sourceMessageSenderNickname
        self.sessionType = sessionType
    }
}

extension MessageRevoked {
    public var revokerIsSelf: Bool {
        return revokerID == IMController.shared.userID
    }
    
    public var sourceMessageSendIDIsSelf: Bool {
        return sourceMessageSendID == IMController.shared.userID
    }
}

public class FullUserInfo {
    public var publicInfo: PublicUserInfo?
    public var friendInfo: FriendInfo?
    public var blackInfo: BlackInfo?
    
    public var userID: String?
    public var showName: String?
    public var faceURL: String?
    public var gender: Gender = .male
}

public enum Gender: Int, Codable {
    case undefine = 0
    case male = 1
    case female = 2
    
    public var description: String {
        switch self {
        case .male:
            return "男".innerLocalized()
        case .female:
            return "女".innerLocalized()
        case .undefine:
            return "-".innerLocalized()
        }
    }
}

public enum Relationship: Int, Codable {
    case black = 0
    case friends = 1
}

public class PublicUserInfo: Encodable {
    public var userID: String?
    public var nickname: String?
    public var faceURL: String?
    public var gender: Gender = .male
}

public class FriendInfo: PublicUserInfo {
    public var ownerUserID: String?
    public var remark: String?
    public var createTime: Int = 0
    public var addSource: Int = 0
    public var operatorUserID: String?
    public var phoneNumber: String?
    public var birth: Int = 0
    public var email: String?
    public var attachedInfo: String?
    public var ex: String?
    
    public func toUserInfo() -> UserInfo? {
        let json = JsonTool.toJson(fromObject: self)
        let obj = JsonTool.fromJson(json, toClass: UserInfo.self)
        return obj
    }
}

public class BlackInfo: PublicUserInfo {
    public var operatorUserID: String?
    public var createTime: Int = 0
    public var addSource: Int = 0
    public var attachedInfo: String?
    public var ex: String?
}

public class SearchParam {
    public var conversationID: String = ""
    public var keywordList: [String] = []
    public var messageTypeList: [MessageContentType]?
    public var searchTimePosition: Int = 0
    public var searchTimePeriod: Int = 0
    public var pageIndex: Int = 1
    public var count: Int = 100
    public init() {
        
    }
}

public class SearchResultInfo {
    public var totalCount: Int = 0
    public var searchResultItems: [SearchResultItemInfo] = []
}

public class SearchResultItemInfo {
    public var conversationID: String = ""
    public var messageCount: Int = 0
    public var conversationType: ConversationType = .c1v1
    public var showName: String = ""
    public var faceURL: String = ""
    public var messageList: [MessageInfo] = []
}

public class SearchGroupParam {
    public var keywordList: [String] = []
    public var isSearchGroupID: Bool = true
    public var isSearchGroupName: Bool = true
    public init() {
        
    }
}

public class SearchUserParam {
    public var keywordList: [String] = []
    public var isSearchUserID: Bool = true
    public var isSearchNickname: Bool = true
    public var isSearchRemark: Bool = true
    public init() {
        
    }
}

public class SearchUserInfo: FriendInfo {
    public var relationship: Relationship = .friends
}

// MARK: - 模型转换(From SDK)

extension MessageInfo {
    func toCRIMMessageInfo() -> CRIMMessageInfo {
        let json: String = JsonTool.toJson(fromObject: self)
        if let item = CRIMMessageInfo.mj_object(withKeyValues: json) {
            return item
        }
        return CRIMMessageInfo()
    }
    
    public var isOutgoing: Bool {
        return sendID == IMController.shared.userID
    }
}

extension OfflinePushInfo {
    func toCRIMOfflinePushInfo() -> CRIMOfflinePushInfo {
        let json: String = JsonTool.toJson(fromObject: self)
        if let item = CRIMOfflinePushInfo.mj_object(withKeyValues: json) {
            return item
        }
        return CRIMOfflinePushInfo()
    }
}

extension MessageContentType {
    func toCRIMMessageContentType() -> CRIMMessageContentType {
        let type = CRIMMessageContentType(rawValue: rawValue) ?? CRIMMessageContentType.text
        return type
    }
}

extension CRIMGroupInfo {
    func toGroupInfo() -> GroupInfo {
        let item = GroupInfo()
        item.groupID = groupID ?? ""
        item.faceURL = faceURL
        item.createTime = createTime
        item.creatorUserID = creatorUserID
        item.memberCount = memberCount
        item.introduction = introduction
        item.notification = notification
        item.groupName = groupName
        item.status = GroupStatus(rawValue: status.rawValue) ?? .ok
        item.needVerification = GroupVerificationType(rawValue: needVerification.rawValue) ?? .applyNeedVerificationInviteDirectly
        item.lookMemberInfo = lookMemberInfo
        item.applyMemberFriend = applyMemberFriend
        
        return item
    }
}

extension CRIMUserInfo {
    public func toUserInfo() -> UserInfo {
        let item = UserInfo(userID: userID!)
        item.faceURL = faceURL
        item.nickname = nickname
        item.createTime = createTime
        item.ex = ex
        item.globalRecvMsgOpt = ReceiveMessageOpt(rawValue: globalRecvMsgOpt.rawValue) ?? .receive
        
        return item
    }
}

extension CRIMGroupReqInfo {
    func toGroupApplicationInfo() -> GroupApplicationInfo {
        let item = GroupApplicationInfo()
        item.groupID = groupID ?? ""
        item.groupName = groupName
        item.groupFaceURL = groupFaceURL
        item.creatorUserID = creatorUserID ?? ""
        item.ownerUserID = ownerUserID ?? ""
        item.memberCount = memberCount
        item.userID = userID
        item.nickname = nickname
        item.userFaceURL = userFaceURL
        item.reqMsg = reqMsg
        item.reqTime = reqTime
        item.handleUserID = handleUserID
        item.handledMsg = handledMsg
        item.handledTime = handledTime
        item.handleResult = ApplicationStatus(rawValue: handleResult.rawValue) ?? .normal
        item.joinSource = JoinSource(rawValue: Int(joinSource.rawValue)) ?? .search
        item.inviterUserID = inviterUserID
        return item
    }
}

extension CRIMFriendReq {
    func toFriendApplication() -> FriendApplication {
        let item = FriendApplication()
        item.fromUserID = fromUserID ?? ""
        item.fromNickname = fromNickname
        item.fromFaceURL = fromFaceURL
        item.toUserID = toUserID ?? ""
        item.toNickname = toNickname
        item.toFaceURL = toFaceURL
        item.handleResult = ApplicationStatus(rawValue: handleResult.rawValue) ?? .normal
        item.reqMsg = reqMsg
        item.handlerUserID = handlerUserID
        item.handleMsg = handleMsg
        item.handleTime = handleTime
        return item
    }
}

extension CRIMFullUserInfo {
    public func toUserInfo() -> UserInfo {
        let item = UserInfo(userID: userID)
        item.faceURL = faceURL
        // 注意此处值类型的不对应
        item.nickname = showName

        return item
    }
}

extension CRIMConversationInfo {
    func toConversationInfo() -> ConversationInfo {
        let item = ConversationInfo(conversationID: conversationID ?? "")
        item.userID = userID
        item.groupID = groupID
        item.showName = showName
        item.faceURL = faceURL
        item.recvMsgOpt = recvMsgOpt.toReceiveMessageOpt()
        item.unreadCount = unreadCount
        item.conversationType = conversationType.toConversationType()
        item.latestMsgSendTime = latestMsgSendTime
        item.draftText = draftText
        item.draftTextTime = draftTextTime
        item.isPinned = isPinned
        item.latestMsg = latestMsg?.toMessageInfo()
        item.isPrivateChat = isPrivateChat
        item.burnDuration = burnDuration
        item.ex = ex
        return item
    }
}

extension CRIMMessageInfo {
    public func toMessageInfo() -> MessageInfo {
        let item = MessageInfo()
        item.clientMsgID = clientMsgID ?? ""
        item.serverMsgID = serverMsgID
        item.createTime = createTime
        item.sendTime = sendTime
        item.sessionType = sessionType.toConversationType()
        item.sendID = sendID ?? ""
        item.recvID = recvID
        item.handleMsg = handleMsg
        item.msgFrom = msgFrom.toMessageLevel()
        item.contentType = contentType.toMessageContentType()
        item.platformID = platformID
        item.senderNickname = senderNickname
        item.senderFaceUrl = senderFaceUrl
        item.groupID = groupID
        item.content = content
        item.seq = seq
        item.isRead = isRead
        item.status = status.toMessageStatus()
        item.attachedInfo = attachedInfo
        item.ex = ex
        item.hasReadTime = hasReadTime
        item.offlinePushInfo = offlinePush.toOfflinePushInfo()
        item.textElem = textElem?.toTextElem()
        item.pictureElem = pictureElem?.toPictureElem()
        item.soundElem = soundElem?.toSoundElem()
        item.videoElem = videoElem?.toVideoElem()
        item.fileElem = fileElem?.toFileElem()
        item.mergeElem = mergeElem?.toMergeElem()
        item.atTextElem = atTextElem?.toAtTextElem()
        item.locationElem = locationElem?.toLocationElem()
        item.quoteElem = quoteElem?.toQuoteElem()
        item.customElem = customElem?.toCustomElem()
        item.notificationElem = notificationElem?.toNotificationElem()
        item.faceElem = faceElem?.toFaceElem()
        item.attachedInfoElem = attachedInfoElem?.toAttachedInfoElem()
        item.hasReadTime = hasReadTime
        item.cardElem = cardElem?.toCardElem()
        item.typingElem = typingElem?.toTypingElem()
        
        return item
    }
}

extension CRIMReceiveMessageOpt {
    func toReceiveMessageOpt() -> ReceiveMessageOpt {
        switch self {
        case .receive:
            return .receive
        case .notReceive:
            return .notReceive
        case .notNotify:
            return .notNotify
        }
    }
}

extension CRIMConversationType {
    func toConversationType() -> ConversationType {
        switch self {
        case .undefine:
            return .undefine
        case .c1v1:
            return .c1v1
        case .group:
            return .group
        case .notification:
            return .notification
        default:
            return .undefine
        }
    }
}

extension CRIMMessageLevel {
    func toMessageLevel() -> MessageLevel {
        switch self {
        case .user:
            return .user
        case .system:
            return .system
        default:
            return .undefine
        }
    }
}

extension CRIMMessageContentType {
    func toMessageContentType() -> MessageContentType {
        let item = MessageContentType(rawValue: rawValue) ?? MessageContentType.unknown
        return item
    }
}

extension CRIMMessageStatus {
    func toMessageStatus() -> MessageStatus {
        switch self {
        case .undefine:
            return .undefine
        case .sending:
            return .sending
        case .sendSuccess:
            return .sendSuccess
        case .sendFailure:
            return .sendFailure
        case .deleted:
            return .deleted
        case .revoke:
            return .revoke
        @unknown
        default:
            return .undefine
        }
    }
}

extension CRIMOfflinePushInfo {
    func toOfflinePushInfo() -> OfflinePushInfo {
        let item = OfflinePushInfo()
        item.title = title
        item.desc = desc
        item.iOSPushSound = iOSPushSound
        item.iOSBadgeCount = iOSBadgeCount
        item.operatorUserID = operatorUserID
        item.ex = ex
        return item
    }
}

extension CRIMTextElem {
    func toTextElem() -> TextElem {
        let item = TextElem()
        item.content = content
        
        return item
    }
}

extension CRIMCardElem {
    func toCardElem() -> CardElem {
        let item = CardElem(userID: userID, nickname: nickname, faceURL: faceURL)
        item.ex = ex
        
        return item
    }
}

extension CRIMTypingElem {
    func toTypingElem() -> TypingElem {
        let item = TypingElem()
        item.msgTips = msgTips
        
        return item
    }
}

extension CRIMPictureElem {
    func toPictureElem() -> PictureElem {
        let item = PictureElem()
        item.sourcePath = sourcePath
        item.sourcePicture = sourcePicture?.toPictureInfo()
        item.bigPicture = bigPicture?.toPictureInfo()
        item.snapshotPicture = snapshotPicture?.toPictureInfo()
        return item
    }
}

extension CRIMPictureInfo {
    func toPictureInfo() -> PictureInfo {
        let item = PictureInfo()
        item.uuID = uuID
        item.type = type
        item.size = size
        item.width = width
        item.height = height
        item.url = url
        return item
    }
}

extension CRIMSoundElem {
    func toSoundElem() -> SoundElem {
        let item = SoundElem()
        item.uuID = uuID
        item.soundPath = soundPath
        item.sourceUrl = sourceUrl
        item.dataSize = dataSize
        item.duration = duration
        return item
    }
}

extension CRIMVideoElem {
    func toVideoElem() -> VideoElem {
        let item = VideoElem()
        item.videoUUID = videoUUID
        item.videoPath = videoPath
        item.videoUrl = videoUrl
        item.videoType = videoType
        item.videoSize = videoSize
        item.duration = duration
        item.snapshotPath = snapshotPath
        item.snapshotUUID = snapshotUUID
        item.snapshotSize = snapshotSize
        item.snapshotUrl = snapshotUrl
        item.snapshotWidth = snapshotWidth
        item.snapshotHeight = snapshotHeight
        return item
    }
}

extension CRIMFileElem {
    func toFileElem() -> FileElem {
        let item = FileElem()
        item.uuID = uuID
        item.filePath = filePath
        item.sourceUrl = sourceUrl
        item.fileName = fileName
        item.fileSize = fileSize
        return item
    }
}

extension CRIMMergeElem {
    func toMergeElem() -> MergeElem {
        let item = MergeElem()
        item.title = title
        item.abstractList = abstractList
        item.multiMessage = multiMessage?.compactMap { $0.toMessageInfo() }
        return item
    }
}

extension CRIMAtTextElem {
    func toAtTextElem() -> AtTextElem {
        let item = AtTextElem()
        item.text = text
        item.atUserList = atUserList
        item.atUsersInfo = atUsersInfo?.compactMap { $0.toAtInfo() }
        item.isAtSelf = isAtSelf
        item.quoteMessage = quoteMessage?.toMessageInfo()
        
        return item
    }
}

extension CRIMAtInfo {
    func toAtInfo() -> AtInfo {
        let item = AtInfo(atUserID: atUserID!, groupNickname: groupNickname!)

        return item
    }
}

extension CRIMLocationElem {
    func toLocationElem() -> LocationElem {
        let item = LocationElem()
        item.desc = desc
        item.longitude = longitude
        item.latitude = latitude
        return item
    }
}

extension CRIMQuoteElem {
    func toQuoteElem() -> QuoteElem {
        let item = QuoteElem()
        item.text = text
        item.quoteMessage = quoteMessage?.toMessageInfo()
        return item
    }
}

extension CRIMCustomElem {
    func toCustomElem() -> CustomElem {
        let item = CustomElem()
        item.data = data
        item.ext = self.extension
        item.description = description
        return item
    }
}

extension CRIMFaceElem {
    func toFaceElem() -> FaceElem {
        let item = FaceElem()
        item.index = index
        item.data = data
        return item
    }
}

extension CRIMAttachedInfoElem {
    func toAttachedInfoElem() -> AttachedInfoElem {
        let item = AttachedInfoElem()
        item.groupHasReadInfo = groupHasReadInfo?.toGroupHasReadInfo()
        item.isPrivateChat = isPrivateChat
        item.burnDuration = burnDuration
        item.hasReadTime = hasReadTime
        return item
    }
}

extension CRIMGroupHasReadInfo {
    func toGroupHasReadInfo() -> GroupHasReadInfo {
        let item = GroupHasReadInfo()
        item.hasReadCount = hasReadCount
        item.hasReadUserIDList = hasReadUserIDList
        return item
    }
}

extension CRIMNotificationElem {
    func toNotificationElem() -> NotificationElem {
        let item = NotificationElem(opUser: opUser?.toGroupMemberInfo(),
                                    quitUser: quitUser?.toGroupMemberInfo(),
                                    entrantUser: entrantUser?.toGroupMemberInfo(),
                                    groupNewOwner: groupNewOwner?.toGroupMemberInfo(),
                                    group: group?.toGroupInfo(),
                                    kickedUserList: kickedUserList?.compactMap { $0.toGroupMemberInfo() },
                                    invitedUserList: invitedUserList?.compactMap { $0.toGroupMemberInfo() })
        item.detail = detail

        return item
    }
}

extension CRIMGroupMemberInfo {
    public func toGroupMemberInfo() -> GroupMemberInfo {
                
        let item = GroupMemberInfo()
        item.userID = userID
        item.roleLevel = GroupMemberRole(rawValue: roleLevel.rawValue) ?? .member
        item.groupID = groupID
        item.nickname = nickname
        item.faceURL = faceURL
        item.joinTime = joinTime
        item.joinSource = JoinSource(rawValue: Int(joinSource.rawValue)) ?? .search
        item.operatorUserID = operatorUserID
        item.inviterUserID = inviterUserID
        item.muteEndTime = muteEndTime
        item.ex = ex
        return item
    }
}

extension CRIMGroupMemberRole {
}

extension CRIMConversationNotDisturbInfo {
    func toConversationNotDisturbInfo() -> ConversationNotDisturbInfo {
        let item = ConversationNotDisturbInfo(conversationId: conversationID ?? "")
        item.result = result.toReceiveMessageOpt()
        return item
    }
}

extension CRIMReceiptInfo {
    func toReceiptInfo() -> ReceiptInfo {
        let item = ReceiptInfo()
        item.userID = userID
        item.groupID = groupID
        item.msgIDList = msgIDList
        item.readTime = readTime
        item.msgFrom = msgFrom.toMessageLevel()
        item.contentType = contentType.toMessageContentType()
        item.sessionType = sessionType.toConversationType()
        return item
    }
}

extension CRIMFullUserInfo {
    func toFullUserInfo() -> FullUserInfo {
        let item = FullUserInfo()
        item.blackInfo = blackInfo?.toBlackInfo()
        item.friendInfo = friendInfo?.toFriendInfo()
        item.publicInfo = publicInfo?.toPublicUserInfo()
        item.userID = userID
        item.showName = showName
        item.faceURL = faceURL
        item.gender = Gender(rawValue: gender.rawValue) ?? .male
        return item
    }
}

extension CRIMBlackInfo {
    func toBlackInfo() -> BlackInfo {
        let item = BlackInfo()
        item.operatorUserID = operatorUserID
        item.createTime = createTime
        item.addSource = addSource
        item.userID = userID
        item.faceURL = faceURL
        item.nickname = nickname
        item.attachedInfo = attachedInfo
        item.ex = ex
        return item
    }
}

extension CRIMFriendInfo {
    func toFriendInfo() -> FriendInfo {
        let item = FriendInfo()
        item.nickname = nickname
        item.faceURL = faceURL
        item.userID = userID
        item.ownerUserID = ownerUserID
        item.remark = remark
        item.createTime = createTime
        item.addSource = addSource
        item.operatorUserID = operatorUserID
        item.phoneNumber = phoneNumber
        item.birth = birth
        item.email = email
        item.attachedInfo = attachedInfo
        item.ex = ex
        return item
    }
}

extension CRIMSearchFriendsInfo {
    func toSearchFriendsInfo() -> SearchUserInfo {
        let item = SearchUserInfo()
        item.relationship = Relationship(rawValue: relationship.rawValue) ?? .friends
        item.nickname = nickname
        item.faceURL = faceURL
        item.userID = userID
        item.ownerUserID = ownerUserID
        item.remark = remark
        item.createTime = createTime
        item.addSource = addSource
        item.operatorUserID = operatorUserID
        item.phoneNumber = phoneNumber
        item.birth = birth
        item.email = email
        item.attachedInfo = attachedInfo
        item.ex = ex
        return item
    }
}

extension CRIMPublicUserInfo {
    func toPublicUserInfo() -> PublicUserInfo {
        let item = PublicUserInfo()
        item.userID = userID
        item.nickname = nickname
        item.faceURL = faceURL
        item.gender = Gender(rawValue: gender.rawValue) ?? .male
        return item
    }
}

extension CRIMSearchResultInfo {
    func toSearchResultInfo() -> SearchResultInfo {
        let item = SearchResultInfo()
        item.totalCount = totalCount
        item.searchResultItems = searchResultItems.compactMap { $0.toSearchResultItemInfo() }
        return item
    }
}

extension CRIMSearchResultItemInfo {
    func toSearchResultItemInfo() -> SearchResultItemInfo {
        let item = SearchResultItemInfo()
        item.conversationID = conversationID
        item.messageCount = messageCount
        item.conversationType = ConversationType(rawValue: conversationType.rawValue) ?? .c1v1
        item.showName = showName
        item.faceURL = faceURL
        item.messageList = messageList.compactMap { $0.toMessageInfo() }
        return item
    }
}

extension CRIMMessageRevokedInfo {
    func toMessageRevoked() -> MessageRevoked {
        let item = MessageRevoked()
        item.clientMsgID = clientMsgID
        item.revokeTime = revokeTime
        item.revokerID = revokerID
        item.revokerNickname = revokerNickname
        item.revokerRole = GroupMemberRole(rawValue: revokerRole.rawValue) ?? .member
        item.sessionType = ConversationType(rawValue: sessionType.rawValue) ?? .c1v1
        item.sourceMessageSendID = sourceMessageSendID
        item.sourceMessageSendTime = sourceMessageSendTime
        item.sourceMessageSenderNickname = sourceMessageSenderNickname
        
        return item
    }
}

// MARK: - 模型转换(From CRIMUIKit)

extension UserInfo {
    public func toCRIMUserInfo() -> CRIMUserInfo {
        let json = JsonTool.toJson(fromObject: self)
        if let item = CRIMUserInfo.mj_object(withKeyValues: json) {
            return item
        }
        return CRIMUserInfo()
    }
}

extension GroupInfo {
    func toCRIMGroupInfo() -> CRIMGroupInfo {
        let item = CRIMGroupInfo()
        item.groupID = groupID
        item.faceURL = faceURL
        item.groupName = groupName
        item.introduction = introduction
        item.notification = notification
        item.lookMemberInfo = lookMemberInfo
        item.applyMemberFriend = applyMemberFriend
        item.needVerification = CRIMGroupVerificationType(rawValue: needVerification.rawValue)!
        
        return item
    }
}

public extension SearchParam {
    public func toCRIMSearchParam() -> CRIMSearchParam {
        let item = CRIMSearchParam()
        item.conversationID = conversationID
        item.keywordList = keywordList
        item.messageTypeList = messageTypeList?.compactMap { $0.rawValue }
        item.searchTimePeriod = searchTimePeriod
        item.searchTimePosition = searchTimePosition
        item.pageIndex = pageIndex
        item.count = count
        return item
    }
}

public extension SearchUserParam {
    func toCRIMSearchUserParam() -> CRIMSearchFriendsParam {
        let item = CRIMSearchFriendsParam()
        item.keywordList = keywordList
        item.isSearchRemark = isSearchRemark
        item.isSearchNickname = isSearchNickname
        item.isSearchUserID = isSearchUserID
        
        return item
    }
}

public extension SearchGroupParam {
    func toCRIMSearchGroupParam() -> CRIMSearchGroupParam {
        let item = CRIMSearchGroupParam()
        item.keywordList = keywordList
        item.isSearchGroupID = isSearchGroupID
        item.isSearchGroupName = isSearchGroupName
        
        return item
    }
}

extension MessageRevoked {
    func toCRIMMessageRevoked() -> CRIMMessageRevokedInfo {
        let json: String = JsonTool.toJson(fromObject: self)
        if let item = CRIMMessageRevokedInfo.mj_object(withKeyValues: json) {
            return item
        }
        return CRIMMessageRevokedInfo()
    }
}

extension CardElem {
    func toCRIMCardElem() -> CRIMCardElem {
        let json: String = JsonTool.toJson(fromObject: self)
        if let item = CRIMCardElem.mj_object(withKeyValues: json) {
            return item
        }
        return CRIMCardElem()
    }
}
