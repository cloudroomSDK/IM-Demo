
import Foundation
import CRIMSDK
import RxSwift
import RxCocoa
import ProgressHUD
import CloudroomVideoSDK_IOS

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

public typealias ValueChangedHandler<T> = (_ value: T) -> Void
public typealias RTCSDKHandler<T, U, V> = (_ value: T, _ value: U, _ value: V) -> Void

public class CallingManager: NSObject, CloudroomVideoMgrCallBack, CloudroomVideoMeetingCallBack {
    private let disposeBag = DisposeBag()
    private var signalingInfo: CRIMSignalingInfo?
    
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
    public var username = ""
    public var roomParticipantChangedHandler: ValueChangedHandler<CRIMParticipantConnectedInfo>?
    public var endCallingHandler: ValueChangedHandler<CRIMMessageInfo>?
    public var endCreateMeetingHandler: RTCSDKHandler<CRVIDEOSDK_ERR_DEF?, MeetInfo?, String>?
    
    public func setup(appID: String? = nil, appSecret: String? = nil, token: String? = nil) {
        self.appID = appID
        self.appSecret = appSecret
        self.token = token
    }
    
    public func setupVideoSDK(sdkServer: String) {
        if CloudroomVideoSDK.shareInstance().isInitSuccess() {
            CloudroomVideoSDK.shareInstance().uninit()
        }
        
        let sdkPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true).first
        
        let sdkInitData:SdkInitDat = SdkInitDat()
        sdkInitData.sdkDatSavePath = sdkPath
        sdkInitData.showSDKLogConsole = true
        sdkInitData.noCall = false
        sdkInitData.noQueue = true
        sdkInitData.noMediaDatToSvr = false
        
        let isHttps = sdkServer.hasPrefix("https://")
        sdkInitData.datEncType = isHttps ? "1" : "0"
        if isHttps {
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
        
        let error = CloudroomVideoSDK.shareInstance().initSDK(sdkInitData)
        guard error == CRVIDEOSDK_NOERR else {
            print("CloudroomVideoSDK init error!")
            CloudroomVideoSDK.shareInstance().uninit()
            return
        }
        
        let SDKVer = CloudroomVideoSDK.getVer()
        print("GetCloudroomVideoSDKVer:" + SDKVer!)
        
        var server = sdkServer.replacingOccurrences(of: "https://", with: "")
        server = server.replacingOccurrences(of: "http://", with: "")
        
        CloudroomVideoSDK.shareInstance().setServerAddr(server)
    }
    
    public func unInitVideoSDK() {
        CloudroomVideoSDK.shareInstance().uninit()
    }
    
    public func start(nickname: String) {
        NotificationCenter.default.addObserver(self, selector: #selector(willTerminate), name: UIApplication.willTerminateNotification, object: nil)
        
        CRIMManager.callbacker.addAdvancedMsgListener(listener: self)
        CloudroomVideoMgr.shareInstance().registerCallback(self)
        CloudroomVideoMeeting.shareInstance().registerCallback(self)
        
        username = nickname
        login(nickname: username)
    }
    
    public func end() {
        timeoutTimer?.invalidate()
        timeoutTimer = nil
        
        CRIMManager.callbacker.removeAdvancedMsgListener(listener: self)
        CloudroomVideoMgr.shareInstance().removeCallback(self)
        CloudroomVideoMeeting.shareInstance().remove(self)
        
        hadLoginSuccess = false
        CloudroomVideoMgr.shareInstance().logout()
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
            if let signalingInfo = self?.signalingInfo {
                
                let message = CRIMMessageInfo.createSignalingCancel(signalingInfo.invitation)
                let recvID = signalingInfo.invitation.inviteeUserIDList.first ?? ""
                self?.sendSignalingMessage(message: message, recvID: recvID)
            }
            
            self?.update(state: .cancel)
        }
        
        senderViewController!.onHungup = { [weak self] duration in
            if let signalingInfo = self?.signalingInfo {
                
                signalingInfo.invitation.duration = duration
                let message = CRIMMessageInfo.createSignalingHungUp(signalingInfo.invitation)
                let recvID = signalingInfo.invitation.inviteeUserIDList.first ?? ""
                self?.sendSignalingMessage(message: message, recvID: recvID)            }
            
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
            if let signalingInfo = self?.signalingInfo {
                
                let message = CRIMMessageInfo.createSignalingAccept(signalingInfo.invitation)
                self?.sendSignalingMessage(message: message, recvID: signalingInfo.invitation.inviterUserID)
                
                Task { [self] in
                    await self?.reciverViewController?.connectRoom(meetID: signalingInfo.invitation.roomID)
                }
            }
            self?.update(state: .beAccepted)
        }
        
        reciverViewController!.onRejected = { [weak self] in
            if let signalingInfo = self?.signalingInfo {
                
                let message = CRIMMessageInfo.createSignalingReject(signalingInfo.invitation)
                self?.sendSignalingMessage(message: message, recvID: signalingInfo.invitation.inviterUserID)
            }
            
            self?.update(state: .reject)
        }
        
        reciverViewController!.onHungup = { [weak self] duration in
            /*
            if let signalingInfo = self?.signalingInfo {
                
                signalingInfo.invitation.duration = duration
                let message = CRIMMessageInfo.createSignalingHungUp(signalingInfo.invitation)
                self?.sendSignalingMessage(message: message, recvID: signalingInfo.invitation.inviterUserID)
            }
            
            */
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
            info.inviterUserID = CRIMManager.manager.getLoginUserID()
            info.inviteeUserIDList = othersID
            info.mediaType = isVideo ? "video" : "audio"
            info.roomID = String(format: "%d", meetID)
            
            let message = CRIMMessageInfo.createSignalingInvite(info)
            
            var offlinePushInfo = CRIMOfflinePushInfo()
            
            if let groupID, !groupID.isEmpty {
                offlinePushInfo.title = "Someone invited you to a group chat."
            }
            
            self.signalingInfo = CRIMSignalingInfo()
            self.signalingInfo?.opUserID = CRIMManager.manager.getLoginUserID()
            self.signalingInfo?.invitation = info
            self.sendSignalingMessage(message: message, recvID: othersID[0])
            completion(true)
        }
    }
    
    private func getUsersInfo(_ usersID: [String], callback: @escaping ([CallingUserInfo]) -> Void) {
        
        CRIMManager.manager.getUsersInfo(usersID) { (infos: [CRIMUserInfo]?) in
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
        login(nickname: username)
    }
    
    private func login(nickname: String) {
        if let token {
            CloudroomVideoMgr.shareInstance().login(byToken: token, nickName: nickname, userID: nickname, userAuthCode: "", cookie: "")
        } else if let appID, let appSecret {
            CloudroomVideoMgr.shareInstance().login(appID, appSecret: appSecret, nickName: nickname, userID: nickname, userAuthCode: "", cookie: "")
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
        CloudroomVideoMgr.shareInstance().createMeeting("CallingCreateRoom")
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
                    await senderViewController?.connectRoom(meetID: signalingInfo!.invitation.roomID)
                }
            }
            return
        }
        
        reciverViewController?.dismiss()
        reciverViewController = nil
        senderViewController?.dismiss()
        senderViewController = nil
        signalingInfo = nil
    }
}

// MARK: Listener

extension CallingManager: CRIMAdvancedMsgListener {
    
    public func onRecvNewMsg(_ message: CRIMMessageInfo) {
        if message.contentType == .custom, let dataStr = message.customElem?.data {
            var data = try! JSONSerialization.jsonObject(with: dataStr.data(using: .utf8)!) as! [String: Any]
            let customType = data["customType"] as! Int
            let userID = data["userID"] as? String
            
            data = data["data"] as! [String: Any]
            
            
            if (customType == 300 ||
                customType == 301 ||
                customType == 302 ||
                customType == 303 ||
                customType == 304) {
                
                let invitation = CRIMInvitationInfo()
                invitation.inviterUserID = data["inviterUserID"] as! String
                invitation.inviteeUserIDList = data["inviteeUserIDList"] as? [String] ?? [String]()
                invitation.roomID = data["roomID"] as? String ?? ""
                invitation.timeout = data["timeout"] as? Int ?? 0
                invitation.mediaType = data["mediaType"] as! String
                invitation.sessionType = CRIMConversationType(rawValue: data["sessionType"] as? Int ?? 0) ?? .c1v1
//                invitation.platformID = CRIMPlatform(rawValue: (data["platformID"] as! NSNumber).intValue)!
                
                let signaling = CRIMSignalingInfo()
                signaling.opUserID = userID ?? ""
                signaling.invitation = invitation

                switch (customType) {
                case 300:
                    CallingManager.manager.onReceiveNewInvitation(signaling)
                    break;
                case 301:
                    CallingManager.manager.onInviteeAccepted(signaling)
                    break;
                case 302:
                    CallingManager.manager.onInviteeRejected(signaling)
                    break;
                case 303:
                    CallingManager.manager.onInvitationCancelled(signaling)
                    break;
                case 304:
                    CallingManager.manager.onHunguUp(signaling)
                    break;
                default:
                    break
                }
            }
        }
    }
    
    public func onReceiveNewInvitation(_ signalingInfo: CRIMSignalingInfo) {
        guard senderViewController == nil && reciverViewController == nil else {
            print("already in calling")
            return
        }
        
        self.signalingInfo = signalingInfo
        startLiveChat(inviterID: signalingInfo.invitation.inviterUserID,
                      othersID: signalingInfo.invitation.inviteeUserIDList,
                      isVideo: signalingInfo.isVideo,
                      incoming: true)
    }
    
    public func onInviteeAccepted(_ signalingInfo: CRIMSignalingInfo) {
        print("Accepted：\(signalingInfo)")
        self.signalingInfo = signalingInfo
        update(state: .beAccepted)
    }
    
    public func onInviteeRejected(_ signalingInfo: CRIMSignalingInfo) {
        print("Rejected：\(signalingInfo)")
        self.signalingInfo = signalingInfo

        update(state: .beRejected)
    }
    
    public func onInvitationCancelled(_ signalingInfo: CRIMSignalingInfo) {
        print("Cancelled：\(signalingInfo)")
        self.signalingInfo = signalingInfo
        update(state: .beCanceled)
    }
    
    public func onInvitationTimeout(_ signalingInfo: CRIMSignalingInfo) {
        print("Timeout：\(signalingInfo)")
        self.signalingInfo = signalingInfo
       
        update(state: .noReply)
    }
    
    public func onHunguUp(_ signalingInfo: CRIMSignalingInfo) {
        print("HunguUp：\(signalingInfo)")
        self.signalingInfo = signalingInfo

        var duration = (senderViewController?.duration ?? reciverViewController?.duration) ?? 0
        update(state: .beHangup, duration: duration)
    }
}

extension CRIMSignalingInfo {
    var isSignal: Bool {
        true
    }
    
    var isVideo: Bool {
        return invitation.isVideo()
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
