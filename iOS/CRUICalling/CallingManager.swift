
import Foundation
import CRUICore
import CRIMSDK
import RxSwift
import RxCocoa
import ProgressHUD
import RTCSDK_IOS

enum CallingState: String {
    case normal = "normal"
    case call = "call"
    case beCalled = "beCalled"
    case reject = "reject"
    case beRejected = "beRejected"
    case calling = "calling"
    case beAccepted = "beAccepted"
    case hangup = "hangup"
    case beHangup = "beHangup"
    case connecting = "connecting"
    case disConnect = "disConnect"
    case connectFailure = "connectFailure"
    case noReply = "noReply"
    case cancel = "cancel"
    case beCanceled = "beCanceled"
    case timeout = "timeout"
    case join = "join"
    case accessByOther = "accessByOther"
    case rejectedByOther = "rejectedByOther"
}

enum Cookie: String {
    case login = ""
}

public enum CRIM_CustomMsgType: Int {
    case unknow = 0

    case newinvitation = 300                /// 新邀请
    case inviteeaccept = 301                /// 邀请接受
    case inviteereject = 302                /// 邀请拒绝
    case invitationcancel = 303             /// 邀请取消
    case invitationhangup = 304             /// 邀请挂断
}

public enum CRInvitationOpType: Int {
    case cancel = 0    //主叫取消
    case timeout       //被叫超时无应答
    case busy          //被叫忙
    case reject        //被叫拒接
    case butt
}

public class CRIMInvitationInfo: Encodable {
    public var invitationMsgId: String = ""      //邀请的唯一标识，拒绝/接受/取消/挂断时应填写对应的邀请标识
    public var inviterUserID: String = ""        //邀请的初始发起者
    public var groupID: String?                  //被邀请的群id，单聊时为空
    public var inviteeUserIDList: [String] = []  //被邀请者UserID列表,如果目标是群且为全部成员，列表可为空
    public var roomID: String = ""               //会议号
    public var mediaType: String = ""            //video/audio
    public var initiateTime: Int64 = 0           //发起时间（秒）
    public var timeout: Int = 30                //邀请超时时间（秒）
    public var acceptTime: Int64 = 0             //接受邀请的时间（秒）
    public var duration: Int = 0                //通话时长（秒）,通话接通后挂断者，依据acceptTime计算此值
    public var opType: Int?                     //操作类型：CRInvitationOpType
    public var opDesc: String?                   //操作附加描述
}

public typealias ValueChangedHandler<T> = (_ value: T) -> Void
public typealias RTCSDKHandler<T, U, V> = (_ value: T, _ value: U, _ value: V) -> Void

public class CallingManager: NSObject, RTCMgrCallBack, RTCMeetingCallBack {
    private let disposeBag = DisposeBag()
    private var invitationInfo: CRIMInvitationInfo?
    
    private var senderViewController: CallingSenderController?
    private var reciverViewController: CallingReceiverController?
    private var inviter: CallingUserInfo?
    private var others: [CallingUserInfo]?
    private var liveURL: String?
    private var token: String?
    private var appID: String?
    private var appSecret: String?
    private var hadLoginSuccess = false
    private var timeoutTimer: Timer? // 超时时间
    public var remainingTime: Int = 0 // 最大超时时间

    public static let manager: CallingManager = CallingManager()
    public var userID = ""
    public var username = ""
    public var endCallingHandler: ValueChangedHandler<CRIMMessageInfo>?
    public var endCreateMeetingHandler: RTCSDKHandler<CRVIDEOSDK_ERR_DEF?, MeetInfo?, String>?
    
    public func setup(appID: String? = nil, appSecret: String? = nil, token: String? = nil) {
        self.appID = appID
        self.appSecret = appSecret
        self.token = token
    }
    
    public func setupVideoSDK(sdkServer: String, verifyHttpsCert: Bool) {
        if RTCSDK.shareInstance().isInitSuccess() {
            RTCSDK.shareInstance().uninit()
        }
        
        let sdkPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true).first
        
        let sdkInitData:SdkInitDat = SdkInitDat()
        sdkInitData.sdkDatSavePath = sdkPath
        sdkInitData.showSDKLogConsole = true
        
        let isHttps = sdkServer.hasPrefix("https://")
        sdkInitData.datEncType = isHttps ? "1" : "0"
        if verifyHttpsCert {
            sdkInitData.params.setValue("1", forKey: "VerifyHttpsCert")
        } else {
            sdkInitData.params.setValue("0", forKey: "VerifyHttpsCert")
        }
        let rsaPublicKey = ""
        if ((rsaPublicKey != nil) && rsaPublicKey.count > 0) {
            sdkInitData.params.setValue("1", forKey: "HttpDataEncrypt")
            sdkInitData.params.setValue(rsaPublicKey, forKey: "RsaPublicKey")
        } else {
            sdkInitData.params.setValue("0", forKey: "HttpDataEncrypt")
        }
        
        let error = RTCSDK.shareInstance().initSDK(sdkInitData)
        guard error == CRVIDEOSDK_NOERR else {
            print("RTCSDK init error!")
            RTCSDK.shareInstance().uninit()
            return
        }
        
        let SDKVer = RTCSDK.getVer()
        print("GetRTCSDKVer:" + SDKVer!)
        
        var server = sdkServer.replacingOccurrences(of: "https://", with: "")
        server = server.replacingOccurrences(of: "http://", with: "")
        
        RTCSDK.shareInstance().setServerAddr(server)
    }
    
    public func unInitVideoSDK() {
        RTCSDK.shareInstance().uninit()
    }
    
    public func call(userID: String, nickname: String) {
        NotificationCenter.default.addObserver(self, selector: #selector(willTerminate), name: UIApplication.willTerminateNotification, object: nil)
        
        CRIMManager.callbacker.addAdvancedMsgListener(listener: self)
        RTCMgr.shareInstance().registerCallback(self)
        RTCMeeting.shareInstance().registerCallback(self)
        
        self.username = nickname
        self.userID = userID
        login(userID: userID, nickname: username)
    }
    
    public func end() {
        timeoutTimer?.invalidate()
        timeoutTimer = nil
        
        CRIMManager.callbacker.removeAdvancedMsgListener(listener: self)
        RTCMgr.shareInstance().removeCallback(self)
        RTCMeeting.shareInstance().remove(self)
        
        hadLoginSuccess = false
        RTCMgr.shareInstance().logout()
    }

    public func forceDismiss() {
        if let senderViewController {
            senderViewController.removeMiniWindow()
        }
        
        if let reciverViewController {
            reciverViewController.removeMiniWindow()
        }
    }
    
    @objc private func willTerminate() {
        update(state: .hangup)
    }
    
    private func timeoutTimer(fire: Bool = true) {
        if timeoutTimer != nil {
            return
        }
        
        // 剩余时间
        remainingTime = 60
        
        timeoutTimer = Timer.scheduledTimer(withTimeInterval: 1,
                                           repeats: true) { [weak self] _ in
            
            guard let wself = self else { return }
            
            wself.remainingTime -= 1
            // 检查是否时间耗尽
            if wself.remainingTime <= 0 {
                // 停止定时器
                wself.timeoutTimer?.invalidate()
            }
        }
    }
    
    private func setupSenderViewController() {
        senderViewController = CallingSenderController()

        senderViewController!.onDisconnect = { [weak self] in
            self?.update(state: .disConnect)
        }
        
        senderViewController!.onConnectFailure = { [weak self] in
            self?.update(state: .connectFailure)
        }
        
        senderViewController!.onCancel = { [weak self] in
            if let invitationInfo = self?.invitationInfo {
                
                invitationInfo.opType = CRInvitationOpType.cancel.rawValue
                if let message = self?.createCallMessage(invitationInfo, callType:.invitationcancel) {
                    let recvID = invitationInfo.inviteeUserIDList.first ?? ""
                    self?.sendSignalingMessage(message: message, recvID: recvID)
                }
            }
            
            self?.update(state: .cancel)
        }
        
        senderViewController!.onHungup = { [weak self] duration in
            if let invitationInfo = self?.invitationInfo {
                
                invitationInfo.duration = duration
                if let message = self?.createCallMessage(invitationInfo, callType:.invitationhangup) {
                    let recvID = invitationInfo.inviteeUserIDList.first ?? ""
                    self?.sendSignalingMessage(message: message, recvID: recvID)            }
                }
            
            self?.update(state: .hangup, duration: duration)
        }
    }
    
    private func setupReciverViewController() {
        reciverViewController = CallingReceiverController()
        reciverViewController!.onDisconnect = { [weak self] in
            self?.update(state: .disConnect)
        }
        
        reciverViewController!.onConnectFailure = { [weak self] in
            self?.update(state: .connectFailure)
        }
        
        reciverViewController!.onAccepted = { [weak self] in
            if let invitationInfo = self?.invitationInfo {
                
                invitationInfo.acceptTime = Int64(Date().timeIntervalSince1970)
                if let message = self?.createCallMessage(invitationInfo, callType:.inviteeaccept) {
                    self?.sendSignalingMessage(message: message, recvID: invitationInfo.inviterUserID)
                    
                    Task { [self] in
                        await self?.reciverViewController?.connectRoom(meetID: invitationInfo.roomID, nickname: self?.username ?? "")
                    }
                }
            }
            self?.update(state: .beAccepted)
        }
        
        reciverViewController!.onRejected = { [weak self] in
            if let invitationInfo = self?.invitationInfo {
                
                if let message = self?.createCallMessage(invitationInfo, callType:.inviteereject) {
                    self?.sendSignalingMessage(message: message, recvID: invitationInfo.inviterUserID)
                }
            }
            
            self?.update(state: .reject)
        }
        
        reciverViewController!.onHungup = { [weak self] duration in
            self?.update(state: .hangup, duration: duration)
        }
        
        reciverViewController!.onBeHungup = { [weak self] duration in
            self?.update(state: .beHangup, duration: duration)
        }
    }
    
    deinit {
    }
    
    public func startLiveChat(inviterID: String = CRIMManager.manager.getLoginUserID(),
                              othersID: [String],
                              isVideo: Bool = true,
                              incoming: Bool = false) {
        
        if !incoming {
            setupSenderViewController()
            
            invite(othersID: othersID, isVideo: isVideo) { [weak self] canStart in
                guard let self, canStart else { return }
                
                getUsersInfo([inviterID] + othersID) { [weak self] r in
                    guard let `self` else { return }
                    
                    self.inviter = r.first
                    self.others = r.suffix(r.endIndex)
                    self.senderViewController!.startLiveChat(inviter: { [weak self] in
                        
                        guard let `self` else { return [] }
                        return [self.inviter!]
                    }, others: { [weak self] in
                        
                        guard let `self` else { return [] }
                        return self.others!
                    }, isVideo: isVideo)
                }
            }
        } else {
            setupReciverViewController()
            getUsersInfo([inviterID] + othersID) { [weak self] r in
                guard let `self` else { return }
                self.inviter = r.first
                self.others = Array(r.dropFirst())
                self.reciverViewController?.startLiveChat(inviter: { [weak self] in
                    
                    guard let `self` else { return [] }
                    return [self.inviter!]
                }, others: { [weak self] in
                    
                    guard let `self` else { return [] }
                    return self.others!
                }, isVideo: isVideo)
            }
            
        }
    }
    
    public func startLiveChat(inviter: CallingUserInfo = CallingUserInfo(userID: CRIMManager.manager.getLoginUserID()),
                              others: [CallingUserInfo],
                              isVideo: Bool = true,
                              incoming: Bool = false) {

        self.inviter = inviter
        self.others = others
        
        if !incoming {
            setupSenderViewController()
            
            invite(othersID: others.map({$0.userID}), isVideo: isVideo) { [weak self] canStart in
                guard let self, canStart else { return }
                
                senderViewController!.startLiveChat(inviter: {
                    return [inviter]
                }, others: { [weak self] in
                    guard let self else { return [] }
                    return self.others!
                }, isVideo: isVideo)
            }
        } else {
            setupReciverViewController()
            self.reciverViewController!.startLiveChat(inviter: {
                return [inviter]
            }, others: {
                return others
            }, isVideo: isVideo)
        }
    }
    
    private func invite(othersID: [String], isVideo: Bool, groupID: String? = nil, completion: @escaping ((Bool) -> Void)) {
        createRoom { [weak self] sdkErr, meetInfo, cookie in
            
            guard let self, let meetID = meetInfo?.id else {
                completion(false)
                return
            }
            
            let info = CRIMInvitationInfo()
            info.invitationMsgId = UUID().uuidString
            info.inviterUserID = CRIMManager.manager.getLoginUserID()
            info.inviteeUserIDList = othersID
            info.mediaType = isVideo ? "video" : "audio"
            info.roomID = String(format: "%d", meetID)
            info.initiateTime = Int64(Date().timeIntervalSince1970)            
            
            guard let message = self.createCallMessage(info, callType:.newinvitation) else {
                completion(false)
                return
            }
            self.invitationInfo = info
            self.sendSignalingMessage(message: message, recvID: othersID[0])
            completion(true)
        }
    }
    
    private func getUsersInfo(_ usersID: [String], callback: @escaping ([CallingUserInfo]) -> Void) {
        
        CRIMManager.manager.getUsersInfo(usersID) { (infos: [CRIMPublicUserInfo]?) in
            guard let infos else {
                callback([])
                return
            }
            
            let us = infos.map { info in
                var u = CallingUserInfo()
                u.nickname = info.nickname!
                u.faceURL = info.faceURL
                u.userID = info.userID!
                return u
            }
            
            callback(us)
        }
    }
    
    private func sendSignalingMessage(message: CRIMMessageInfo, recvID: String) {
        let offlinePush = CRIMOfflinePushInfo()
        offlinePush.title = "Someone invited you."
        offlinePush.desc = "Someone invited you."
        
        CRIMManager.manager.sendMsg(message,
                                       recvID: recvID,
                                       groupID: nil,
                                       offlinePushInfo: offlinePush) { msg in
            print("msg:\(msg)")
            ProgressHUD.dismiss()

        } onProgress: { _ in
            
        } onFailure: { code, msg in
            print("code:\(code), msg:\(msg)")
            ProgressHUD.dismiss()
        }
    }
}

extension CallingManager {
    
    public func lineOff(_ sdkErr: CRVIDEOSDK_ERR_DEF) {
        print("\(#function) sdkErr:\(sdkErr)")
        
        hadLoginSuccess = false
        login(userID: userID, nickname: username)
    }
    
    private func login(userID: String, nickname: String) {
        if let token {
            RTCMgr.shareInstance().login(byToken: token, userID: userID, userAuthCode: "", cookie: "")
        } else if let appID, let appSecret {
            RTCMgr.shareInstance().login(appID, appSecret: appSecret, userID: userID, userAuthCode: "", cookie: "")
        }
    }
    
    public func loginSuccess(_ usrID: String!, cookie: String!) {
        print("\(#function) usrID:\(usrID)")
        hadLoginSuccess = true
    }
    
    public func loginFail(_ sdkErr: CRVIDEOSDK_ERR_DEF, cookie: String!) {
        print("\(#function) sdkErr:\(sdkErr)")
    }
    
    private func createRoom(completion: @escaping RTCSDKHandler<CRVIDEOSDK_ERR_DEF?, MeetInfo?, String>) {
        endCreateMeetingHandler = completion
        RTCMgr.shareInstance().createMeeting("CallingCreateRoom")
    }
    
    public func createMeetingSuccess(_ meetInfo: MeetInfo!, cookie: String!) {
        guard let endCreateMeetingHandler else { return }
        endCreateMeetingHandler(CRVIDEOSDK_NOERR, meetInfo, cookie)
    }
    
    public func createMeetingFail(_ sdkErr: CRVIDEOSDK_ERR_DEF, cookie: String!) {
        print("\(#function) sdkErr:\(sdkErr)")
        guard let endCreateMeetingHandler else { return }
        endCreateMeetingHandler(sdkErr, nil, cookie)
    }
    
}

extension CallingManager {
    private func update(state: CallingState, duration: Int = 0) {
        if state == .beAccepted || state == .disConnect {
            if state == .beAccepted {
                Task {
                    await senderViewController?.connectRoom(meetID: invitationInfo!.roomID, nickname: username)
                }
            }
            return
        }
        
        reciverViewController?.dismiss()
        reciverViewController = nil
        senderViewController?.dismiss()
        senderViewController = nil
        invitationInfo = nil
    }
}

extension CRIMInvitationInfo {
    convenience init(from dictionary: [String: Any]) {
        self.init()
        
        self.invitationMsgId = dictionary["invitationMsgId"] as? String ?? ""
        self.inviterUserID = dictionary["inviterUserID"] as? String ?? ""
        self.groupID = dictionary["groupID"] as? String
        self.inviteeUserIDList = dictionary["inviteeUserIDList"] as? [String] ?? []
        self.roomID = dictionary["roomID"] as? String ?? ""
        self.mediaType = dictionary["mediaType"] as? String ?? ""
        self.initiateTime = dictionary["initiateTime"] as? Int64 ?? 0
        self.timeout = dictionary["timeout"] as? Int ?? 30
        self.acceptTime = dictionary["acceptTime"] as? Int64 ?? 0
        self.duration = dictionary["duration"] as? Int ?? 0
        self.opType = dictionary["opType"] as? Int
        self.opDesc = dictionary["opDesc"] as? String
    }
}

// MARK: Listener

extension CallingManager: CRIMAdvancedMsgListener {
    
    public func onRecvNewMsg(_ message: CRIMMessageInfo) {
        if message.contentType == .custom, let dataStr = message.customElem?.data {
            var data = try! JSONSerialization.jsonObject(with: dataStr.data(using: .utf8)!) as! [String: Any]
            let customType = data["customType"] as! Int
            let callType = CRIM_CustomMsgType(rawValue: customType) ?? .unknow
            let customData = data["data"] as! [String: Any]
            
            if (callType == .newinvitation ||
                callType == .inviteeaccept ||
                callType == .inviteereject ||
                callType == .invitationcancel ||
                callType == .invitationhangup) {
                
                let invitation = CRIMInvitationInfo(from: customData)

                switch (callType) {
                case .newinvitation:
                    CallingManager.manager.onReceiveNewInvitation(invitation)
                    break;
                case .inviteeaccept:
                    CallingManager.manager.onInviteeAccepted(invitation)
                    break;
                case .inviteereject:
                    CallingManager.manager.onInviteeRejected(invitation)
                    break;
                case .invitationcancel:
                    CallingManager.manager.onInvitationCancelled(invitation)
                    break;
                case .invitationhangup:
                    CallingManager.manager.onHunguUp(invitation)
                    break;
                default:
                    break
                }
            }
        }
    }
    
    public func onReceiveNewInvitation(_ invitation: CRIMInvitationInfo) {
        guard senderViewController == nil && reciverViewController == nil else {
            print("already in calling")
            return
        }
        
        self.invitationInfo = invitation
        startLiveChat(inviterID: invitation.inviterUserID,
                      othersID: invitation.inviteeUserIDList,
                      isVideo: invitation.isVideo,
                      incoming: true)
    }
    
    public func onInviteeAccepted(_ invitation: CRIMInvitationInfo) {
        print("Accepted：\(invitation)")
        self.invitationInfo = invitation
        update(state: .beAccepted)
    }
    
    public func onInviteeRejected(_ invitation: CRIMInvitationInfo) {
        print("Rejected：\(invitation)")
        self.invitationInfo = invitation

        update(state: .beRejected)
    }
    
    public func onInvitationCancelled(_ invitation: CRIMInvitationInfo) {
        print("Cancelled：\(invitation)")
        self.invitationInfo = invitation
        update(state: .beCanceled)
    }
    
    public func onInvitationTimeout(_ invitation: CRIMInvitationInfo) {
        print("Timeout：\(invitation)")
        self.invitationInfo = invitation
       
        update(state: .noReply)
    }
    
    public func onHunguUp(_ invitation: CRIMInvitationInfo) {
        print("HunguUp：\(invitation)")
        self.invitationInfo = invitation

        var duration = (senderViewController?.duration ?? reciverViewController?.duration) ?? 0
        update(state: .beHangup, duration: duration)
    }
        
    public func createCallMessage(_ invitation: CRIMInvitationInfo, callType: CRIM_CustomMsgType) -> CRIMMessageInfo? {
        if let data = JsonTool.toMap(fromObject: invitation) as? [String: Any] {
            let dataStr = String.init(data: try! JSONSerialization.data(withJSONObject: ["customType": callType.rawValue,
                                                                                         "data": data]),
                                      encoding: .utf8)!
            return CRIMMessageInfo.createCustomMsg(dataStr, extension: "")
        }
        return nil
    }
}

extension CRIMInvitationInfo {
    
    var isVideo: Bool {
        return mediaType == "video"
    }
}

extension Array {
    static func toJson<T: Encodable>(fromObject: T) -> String {
        let encoder = JSONEncoder()
        do {
            let data = try encoder.encode(fromObject)
            guard let json = String(data: data, encoding: .utf8) else {
                fatalError("check your data is encodable from utf8!")
            }
            return json
        } catch let err {
            return ""
        }
    }
}
