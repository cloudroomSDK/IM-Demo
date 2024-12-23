
import AVFAudio
import Foundation
import Lottie
import ProgressHUD
import RxSwift
import UIKit
import Kingfisher
import SnapKit
import CRUICore
import CloudroomVideoSDK_IOS

public typealias UserInfoHandler = () -> [CallingUserInfo]

@objc public class CallingUserInfo: NSObject {
    public var userID: String = ""
    public var nickname: String = ""
    public var faceURL: String?
    public var ex: String?
    
    @objc public init(userID: String? = nil,
                      nickname: String? = nil,
                      faceURL: String? = nil,
                      ex: String? = nil) {
        self.userID = userID ?? ""
        self.nickname = nickname ?? ""
        self.faceURL = faceURL
        self.ex = ex
    }
}

public class CallingBaseViewController: CallingBaseController {
    var inviter: UserInfoHandler!
    var users: UserInfoHandler!
    var isVideo: Bool = true
    var groupID: String?
    var isJoinRoom: Bool = false
    var videoInfos = [UsrVideoInfo]()
    var members = [UsrVideoId]()
    
    internal let disposeBag = DisposeBag()
    internal let room: CloudroomVideoMeeting = CloudroomVideoMeeting.shareInstance()!
    internal var funcBttonsView: UIStackView?
    internal let sdk = DispatchQueue(label: "com.calling.rtc.queue", qos: .userInitiated)
    internal let ringToneQueue: OperationQueue = {
        let v = OperationQueue()
        v.name = "com.calling.ringtone.queue"
        v.maxConcurrentOperationCount = 1
        
        return v
    }()
    internal var minimizeButton: UIButton = .init()
    internal var linkedTimer: Timer? // 通话时间
    
    private var audioPlayer: AVAudioPlayer!
    
    public var linkingDuration: Int = 0 // 通话时长

    public override func connectRoom(meetID: String) {
        connectRoom(ID: meetID)
    }

    public override func dismiss() {
        linkedTimer?.invalidate()
        linkedTimer = nil
        stopSounds()
        
        room.remove(self)
        smallVideoView?.usrVideoId = nil
        bigVideoView?.usrVideoId = nil

        Task {
            await room.exitMeeting()
        }
        DispatchQueue.main.async { [self] in
            UIViewController.currentViewController().dismiss(animated: true)
            removeMiniWindow()
        }
    }
    
    var isSignal: Bool {
        return groupID == nil || groupID!.isEmpty
    }
    
    private let linkingView: AnimationView = {
        let bundle = Bundle.callingBundle()
        let v = AnimationView(name: "linking", bundle: bundle)
        v.loopMode = .loop
        v.isHidden = true
        
        return v
    }()
    
    internal var smallViewIsMe = true
    internal var remoteMuted = false
    internal var localMuted = false
    internal var smallVideoId: UsrVideoId? {
        didSet {
            smallVideoView?.removeFromSuperview()
            smallVideoView = nil
            smallVideoView = setupVideoView()
            smallVideoView?.usrVideoId = smallVideoId
            smallContentView.addSubview(smallVideoView!)
            smallVideoView?.frame = smallContentView.bounds
        }
    }
    
    internal var bigVideoId: UsrVideoId? {
        didSet {
            bigVideoView?.removeFromSuperview()
            bigVideoView = nil
            bigVideoView = setupVideoView()
            bigVideoView?.usrVideoId = bigVideoId
            bigContentView.addSubview(bigVideoView!)
            bigVideoView?.frame = bigContentView.bounds
        }
    }
    
    internal func setupSmallPlaceholerView(user: CallingUserInfo) {
        smallDisableVideoImageView.image = nil
        smallAvatarView.reset()
    
        smallVideoView?.bringSubviewToFront(smallDisableVideoImageView)
        
        if let avatar = user.faceURL, !avatar.isEmpty {
            smallDisableVideoImageView.setImage(with: avatar)
        } else {
            let nickname = user.nickname
            smallAvatarView.setAvatar(url: nil, text: nickname)
            smallAvatarView.isHidden = false
        }
    }
    
    internal func setupBigPlaceholerView(user: CallingUserInfo) {
        bigDisableVideoImageView.image = nil
        bigAvatarView.reset()
        
        bigVideoView?.bringSubviewToFront(bigDisableVideoImageView)
        
        if let avatar = user.faceURL, !avatar.isEmpty {
            bigDisableVideoImageView.setImage(with: avatar)
        } else {
            let nickname = user.nickname
            bigAvatarView.setAvatar(url: nil, text: nickname)
            bigAvatarView.isHidden = false
        }
    }
    
    internal let tipsLabel: UILabel = {
        let t = UILabel()
        t.layer.cornerRadius = 6
        t.layer.masksToBounds = true
        t.text = ""
        t.textAlignment = .center
        t.textColor = .white
        return t
    }()
    
    internal let linkedTimeLabel: UILabel = {
        let t = UILabel()
        t.layer.cornerRadius = 6
        t.layer.masksToBounds = true
        t.text = ""
        t.textAlignment = .center
        t.textColor = .white
        return t
    }()
    
    internal lazy var bigContentView: UIView = {
        let v = UIView()
        v.frame = view.bounds
        
        bigVideoView = setupVideoView()
        v.addSubview(bigVideoView!)
        bigVideoView!.frame = v.bounds

        bigDisableVideoImageView.addSubview(bigAvatarView)
        bigAvatarView.snp.makeConstraints { make in
            make.center.equalToSuperview()
        }
        
        v.addSubview(bigDisableVideoImageView)
        bigDisableVideoImageView.snp.makeConstraints { make in
            make.edges.equalToSuperview()
        }
        
        return v
    }()
    
    private var bigVideoView: CLCameraView?
    
    private let localViewTopInset = UIApplication.safeAreaInsets.top + 70
    
    internal lazy var smallContentView: UIView = {
        let v = UIView()
        v.frame = CGRectMake(CGRectGetWidth(UIScreen.main.bounds) - (120 + 12), localViewTopInset, 120, 180)
        
        smallVideoView = setupVideoView()
        
        v.addSubview(smallVideoView!)
        smallVideoView!.frame = v.bounds
        
        smallDisableVideoImageView.addSubview(smallAvatarView)
        smallAvatarView.snp.makeConstraints { make in
            make.center.equalToSuperview()
        }
        
        v.addSubview(smallDisableVideoImageView)
        smallDisableVideoImageView.snp.makeConstraints { make in
            make.edges.equalToSuperview()
        }
        
        v.isUserInteractionEnabled = true
        
        let pan = UIPanGestureRecognizer(target: self, action: #selector(movePreview))
        v.addGestureRecognizer(pan)
        
        let tap = UITapGestureRecognizer(target: self, action: #selector(handlesmallVideoViewTap(_:)))
        v.addGestureRecognizer(tap)
        
        return v
    }()
    
    private var smallVideoView: CLCameraView?
    
    private func setupVideoView() -> CLCameraView {
        let t = CLCameraView()
        t.keepAspectRatio = false
        
        return t
    }
    
    @objc private func movePreview(gesture: UIPanGestureRecognizer) {

        let moveState = gesture.state
        switch moveState {
        case .changed:
            let point = gesture.translation(in: smallContentView.superview)
            smallContentView.center = CGPoint(x: smallContentView.center.x + point.x, y: smallContentView.center.y + point.y)
            break
        case .ended:
            let point = gesture.translation(in: smallContentView.superview)
            let newPoint = CGPoint(x: smallContentView.center.x + point.x, y: smallContentView.center.y + point.y)
            
            UIView.animate(withDuration: 0.1) { [self] in
                self.smallContentView.center = self.resetPosition(point: newPoint)
            }
            break
        default: break
        }

        gesture.setTranslation(.zero, in: smallContentView.superview!)
    }
    
    private func resetPosition(point: CGPoint) -> CGPoint {
        var newPoint = point
        let limitMargin = 20.0
        let bottomMargin = CGRectGetMaxY(smallContentView.superview!.frame) - 200

        if point.x <= (CGRectGetWidth(smallContentView.superview!.frame) / 2) {
            newPoint.x = (CGRectGetWidth(smallContentView.frame) / 2.0) + limitMargin
        } else {
            newPoint.x = CGRectGetWidth(smallContentView.superview!.frame) - (CGRectGetWidth(smallContentView.frame) / 2) - limitMargin
        }
        
        if point.y <= localViewTopInset {
            newPoint.y = localViewTopInset
        } else if point.y > bottomMargin {
            newPoint.y = bottomMargin
        }
        
        return newPoint
    }
    
    @objc
    private func handlesmallVideoViewTap(_ sender: UITapGestureRecognizer) {
        smallViewIsMe = !smallViewIsMe
        smallDisableVideoImageView.isHidden = true
        bigDisableVideoImageView.isHidden = true
        
        let temp = smallVideoId

        smallVideoId = bigVideoId
        bigVideoId = temp
        
        if let user = users().first, let me = inviter().first {
            if smallViewIsMe {
                smallDisableVideoImageView.isHidden = !localMuted
                bigDisableVideoImageView.isHidden = !remoteMuted
                setupBigPlaceholerView(user: user)
                setupSmallPlaceholerView(user: me)
            } else {
                smallDisableVideoImageView.isHidden = !remoteMuted
                bigDisableVideoImageView.isHidden = !localMuted
                setupBigPlaceholerView(user: me)
                setupSmallPlaceholerView(user: user)
            }
        }
    }
    
    internal let bigAvatarView: AvatarView = {
        let v = AvatarView()
        v.isHidden = true
        
        return v
    }()
    
    internal let bigDisableVideoImageView: UIImageView = {
        let v = UIImageView()
        v.backgroundColor = .gray
        v.isHidden = true
        v.contentMode = .scaleAspectFit
        
        return v
    }()
    
    internal let smallAvatarView: AvatarView = {
        let v = AvatarView()
        v.isHidden = true
        
        return v
    }()
    
    internal let smallDisableVideoImageView: UIImageView = {
        let v = UIImageView()
        v.backgroundColor = .gray
        v.isHidden = true
        v.contentMode = .scaleAspectFit
        
        return v
    }()
    
    override open func viewDidLoad() {
        view.backgroundColor = .init(red: 38 / 255, green: 38 / 255, blue: 38 / 255, alpha: 1)
        setupTopFuncButtons()
        UIApplication.shared.isIdleTimerDisabled = true
        
        view.insertSubview(bigContentView, belowSubview: minimizeButton)
        view.insertSubview(smallContentView, belowSubview: minimizeButton)
        
        room.registerCallback(self)
    }
    
    deinit {
        UIApplication.shared.isIdleTimerDisabled = false
        linkedTimer = nil
        NotificationCenter.default.removeObserver(self)
    }
    
    @objc func scale() {
        self.suspend(coverImageName: "contact_my_friend_icon", tips: self.linkingDuration > 0 ? "calling".innerLocalized() : nil)
    }
    
    private lazy var micButton: UIButton = {
        let v = UIButton(type: .custom)

        v.setImage(UIImage(nameInBundle: "mic_open"), for: .normal)
        v.setImage(UIImage(nameInBundle: "mic_close"), for: .selected)
        v.titleLabel?.textAlignment = .center
        
        v.rx.tap
            .throttle(.seconds(1), scheduler: MainScheduler.instance)
            .subscribe(onNext: { [weak self] _ in
                
            self?.micButtonAction(sender: v)
        }).disposed(by: disposeBag)
        
        return v
    }()
    
    @objc func micButtonAction(sender: UIButton) {
        print("\(#function)")

        sender.isSelected = !sender.isSelected
        sender.isEnabled = false
        
        Task {
            await toggleMicrophoneEnabled(forceEnable: !sender.isSelected)
            sender.isEnabled = true
        }
    }
    
    private lazy var cancelButton: UIButton = {
        let v = UIButton(type: .custom)

        v.setImage(UIImage(nameInBundle: "hang_up"), for: .normal)
        v.rx.tap
            .throttle(.seconds(1), scheduler: MainScheduler.instance)
            .subscribe(onNext: { [weak self] _ in
                
            self?.cancelButtonAction(sender: v)
        }).disposed(by: disposeBag)
        
        return v
    }()
    
    
    private lazy var thirdButton: UIButton = {
        let v = UIButton(type: .custom)
        v.setImage(.init(nameInBundle: "speaker_open"), for: .normal)
        v.setImage(.init(nameInBundle: "speaker_close"), for: .selected)
        
        v.rx.tap
            .throttle(.seconds(1), scheduler: MainScheduler.instance)
            .subscribe(onNext: { [weak self] _ in
                
            self?.thirdButtonAction(sender: v)
        }).disposed(by: disposeBag)
        
        return v
    }()
    
    @objc func cancelButtonAction(sender: UIButton) {
        print("\(#function)")

        sender.isSelected = !sender.isSelected

        if self.linkingDuration > 0 {
            self.onHungup?(self.linkingDuration)
        } else {
            self.onCancel?()
        }
    }
    
    @objc func thirdButtonAction(sender: UIButton) {
        print("\(#function)")

        sender.isSelected = !sender.isSelected
        toggleSpeakerphoneEnabled(enabled: !sender.isSelected)
    }
    
    @objc func rejectButtonAction(sender: UIButton) {
        print("\(#function)")

        sender.isSelected = !sender.isSelected
        self.stopSounds()
        self.onRejected?()
    }
    
    @objc func acceptButtonAction(sender: UIButton) {
        print("\(#function)")

        sender.isSelected = !sender.isSelected
        ProgressHUD.animate(interaction: false)
        self.onAccepted?()
        self.stopSounds()
        self.onTapAccepted()
    }
    
    private lazy var cameraEnabledButton: UIButton = {
        let v = UIButton(type: .custom)
        v.setImage(UIImage(nameInBundle: "video_close"), for: .normal)
        v.setImage(UIImage(nameInBundle: "video_open"), for: .selected)
        
        v.rx.tap.throttle(.seconds(1), scheduler: MainScheduler.instance)
            .subscribe(onNext: { [weak self] _ in
                guard let self else { return }
                
                cameraEnabledButtonAction(sender: v)
            }).disposed(by: disposeBag)
        
        return v
    }()
    
    @objc func cameraEnabledButtonAction(sender: UIButton) {
        print("\(#function)")
        
        sender.isSelected = !sender.isSelected
        sender.isEnabled = false
        switchCameraButton.isEnabled = !sender.isSelected
        
        Task {
            await toggleCameraEnabled(forceEnable: sender.isSelected)
            sender.isEnabled = true
        }
    }
    
    private lazy var switchCameraButton: UIButton = {
        let v = UIButton(type: .custom)
        v.setImage(UIImage(nameInBundle: "trun_camera_flag"), for: .normal)

        v.rx.tap.throttle(.seconds(1), scheduler: MainScheduler.instance)
            .subscribe(onNext: { [weak self] _ in
                guard let self else { return }
                
                cameraPositionButtonAction(sender: v)
            }).disposed(by: disposeBag)
        
        return v
    }()
    
    @objc func cameraPositionButtonAction(sender: UIButton) {
        print("\(#function)")
        
        sender.isSelected = !sender.isSelected
        sender.isEnabled = false
        
        Task {
            await switchCameraPosition()
            sender.isEnabled = true
        }
    }
    
    internal func setupVideoInfos() {
        videoInfos.removeAll()
        let userId = room.getMyUserID()
        guard let allVideoInfo = room.getAllVideoInfo(userId) else {
            return
        }
        
        for item in allVideoInfo {
            videoInfos.append(item as! UsrVideoInfo)
        }
        
        switchCameraPosition()
    }
    
    internal func setupTopFuncButtons() {
        minimizeButton.setImage(UIImage(nameInBundle: "minimize"), for: .normal)
        minimizeButton.addTarget(self, action: #selector(scale), for: .touchUpInside)
        
        view.addSubview(minimizeButton)
        
        minimizeButton.snp.makeConstraints { make in
            make.top.equalTo(view.safeAreaLayoutGuide.snp.top).offset(24)
            make.leading.equalToSuperview().offset(24)
            make.width.equalTo(33)
            make.height.equalTo(30)
        }
        minimizeButton.isHidden = true
    }
    
    func onlineTopMoreFuncButtons() {
        guard isVideo else { return }
        
        view.addSubview(cameraEnabledButton)
        cameraEnabledButton.snp.makeConstraints { make in
            
            make.centerY.equalTo(minimizeButton)
            make.width.height.equalTo(minimizeButton)
        }
        
        view.addSubview(switchCameraButton)
        switchCameraButton.snp.makeConstraints { make in
            
            make.centerY.equalTo(minimizeButton)
            make.leading.equalTo(cameraEnabledButton.snp.trailing).offset(8)
            make.trailing.equalToSuperview().inset(24)
            make.width.height.equalTo(minimizeButton)
        }
    }
    
    internal func onlineFuncButtons() {
        funcBttonsView?.removeFromSuperview()
        funcBttonsView = UIStackView(arrangedSubviews: [micButton, cancelButton, thirdButton])
        funcBttonsView!.axis = .horizontal
        funcBttonsView!.distribution = .fillEqually
        view.addSubview(funcBttonsView!)
        
        funcBttonsView!.snp.makeConstraints { make in
            make.bottom.equalTo(view.safeAreaLayoutGuide.snp.bottom).offset(-48)
            make.leading.trailing.equalToSuperview()
        }
    }

    internal func senderPreviewFuncButtons() {
        onlineFuncButtons()
    }
    
    internal func previewFuncButtons() {
        let cancelButton = UIButton()
        cancelButton.setImage(UIImage(nameInBundle: "hang_up"), for: .normal)
        cancelButton.addTarget(self, action: #selector(rejectButtonAction), for: .touchUpInside)
        
        let pickUpButton = UIButton()
        pickUpButton.setImage(.init(nameInBundle: "pick_up"), for: .normal)
        pickUpButton.addTarget(self, action: #selector(acceptButtonAction), for: .touchUpInside)
        
        funcBttonsView?.removeFromSuperview()
        funcBttonsView = UIStackView(arrangedSubviews: [cancelButton, pickUpButton])
        funcBttonsView!.axis = .horizontal
        funcBttonsView!.distribution = .fillEqually
        view.addSubview(funcBttonsView!)
        
        funcBttonsView!.snp.makeConstraints { make in
            make.bottom.equalTo(view.safeAreaLayoutGuide.snp.bottom).offset(-48)
            make.leading.trailing.equalToSuperview()
        }
    }
    
    internal func playSounds() {
        ringToneQueue.cancelAllOperations()
        
        ringToneQueue.addBarrierBlock { [self] in
            
            if let path = Bundle.callingBundle().path(forResource: "call_ring", ofType: "mp3") {
                do {
                    let session = AVAudioSession.sharedInstance()
                    try session.setCategory(.playback)
                    try session.setActive(true)
                    
                    let url = URL(fileURLWithPath: path)
                    audioPlayer = try? AVAudioPlayer(contentsOf: url)
                    audioPlayer!.play()
                    audioPlayer!.numberOfLoops = 99
                    
                } catch {
                    print(error)
                }
            }
        }
    }
    
    internal func stopSounds() {
        ringToneQueue.addOperation { [self] in
            audioPlayer?.pause()
        }
    }
    
    internal func publishMicrophone() {
        Task {
            await self.toggleMicrophoneEnabled(forceEnable: true)
        }
    }
    
    internal func publishVideo() {
        guard isVideo else { return }
        
        Task {
            await self.toggleCameraEnabled(forceEnable: true)
            cameraEnabledButton.isSelected = true
        }
    }
    
    internal func toggleMicrophoneEnabled(forceEnable: Bool? = nil) async -> Bool {
        let enable = forceEnable ?? false
        
        let userId = room.getMyUserID()
        if enable {
            room.openMic(userId)
        } else {
            room.closeMic(userId)
        }
        return true
    }
    
    internal func switchCameraPosition() -> Bool {
        let userId = room.getMyUserID()
        let currentVideo = room.getDefaultVideo(userId)
        for item in videoInfos {
            if item.videoID != currentVideo {
                room.setDefaultVideo(item.userId, videoID: item.videoID)
            }
        }
        return true
    }
    
    internal func toggleCameraEnabled(forceEnable: Bool? = nil) async -> Bool  {
        let enable = forceEnable ?? false
        
        let userId = room.getMyUserID()
        if enable {
            room.openVideo(userId)
        } else {
            room.closeVideo(userId)
        }
        return true
    }
    
    internal func toggleSpeakerphoneEnabled(enabled: Bool = true) {
        print("toggleSpeakerphoneEnabled:\(enabled)")
        do {
            room.setSpeakerOut(enabled)
        } catch let error {
            print(error.localizedDescription)
        }
    }
    
    private func connectRoom(ID: String) {
        showLinkingView()
        stopSounds()
        
        guard let meetIDInt = Int32(ID) else {
            showLinkingView(show: false)
            ProgressHUD.dismiss()
            return
        }
        
        room.enter(meetIDInt)
    }
        
    internal func insertLinkingViewAbove(aboveView: UIView) {
        view.insertSubview(linkingView, aboveSubview: aboveView)
        
        linkingView.snp.makeConstraints { make in
            make.center.equalToSuperview()
            make.width.equalTo(50)
        }
    }
    
    internal func showLinkingView(show: Bool = true) {
        if show {
            linkingView.isHidden = false
            linkingView.play()
        } else {
            linkingView.isHidden = true
            linkingView.stop()
        }
    }
    
    internal func linkingTimer(fire: Bool = true) {
        if linkedTimer != nil {
            return
        }
        
        linkedTimer = Timer.scheduledTimer(withTimeInterval: 1,
                                           repeats: true) { [weak self] _ in
            
            guard let wself = self else { return }
            
            wself.linkingDuration += 1
            let m = wself.linkingDuration / 60
            let s = wself.linkingDuration % 60
            
            var timeline = ""
            
            if m > 99 {
                timeline = String(format: "%d:%02d", m, s)
            } else {
                timeline = String(format: "%02d:%02d", m, s)
            }
            
            wself.tipsLabel.text = timeline
            wself.linkedTimeLabel.text = timeline
            wself.updateSuspendTips(text: timeline)
        }
    }
    
    internal func onTapAccepted() {}
}

extension CallingBaseViewController: CloudroomVideoMeetingCallBack {
    public func enterMeetingRslt(_ code: CRVIDEOSDK_ERR_DEF) {
        print("\(#function) code \(code)")
        
        showLinkingView(show: false)
        ProgressHUD.dismiss()
        
        if code == CRVIDEOSDK_NOERR {
            if !isSignal {
                onlineFuncButtons()
            }
            setupVideoInfos()
            publishMicrophone()
            publishVideo()
            onlineTopMoreFuncButtons()
            //toggleSpeakerphoneEnabled()
            onlineFuncButtons()
            linkingTimer()
            updateVideoWall()
        }
    }
    
    func updateVideoWall() {
        guard isVideo else {
            return
        }
            
        guard let watchableVideos = room.getWatchableVideos() as? [UsrVideoId] else { return }
        
        if members.count == 0 {
            members.append(contentsOf: watchableVideos)
        } else {

            // 原来存在的
            let same = watchableVideos.filter { (item) -> Bool in
                for obj2 in members {
                    if item.userId == obj2.userId, item.videoID == obj2.videoID {
                        return true
                    }
                }
                return false
            }
            
            // 变化的
            let diff = watchableVideos.filter { (item) -> Bool in
                for obj2 in members {
                    if item.userId == obj2.userId, item.videoID == obj2.videoID {
                        return false
                    }
                }
                return true
            }
            
            guard diff.count > 0 else {
                return
            }
            
            members.removeAll()
            members.append(contentsOf: same)
            members.append(contentsOf: diff)
        }
        
        for videoId in members {
            if videoId.userId == room.getMyUserID() {
                smallVideoId = videoId
            } else {
                bigVideoId = videoId
            }
        }
    }
    
    public func userLeftMeeting(_ userID: String!) {
        updateVideoWall()
        
        if room.getMyUserID() != userID && groupID == nil {
            self.onHungup?(self.linkingDuration)
        }
    }
    
    public func userEnterMeeting(_ userID: String!) {
        updateVideoWall()
    }
    
    public func videoStatusChanged(_ userID: String!, oldStatus: VIDEO_STATUS, newStatus: VIDEO_STATUS) {
        updateVideoWall()
    }
    
    public func videoDevChanged(_ userID: String!) {
        updateVideoWall()
    }
}
