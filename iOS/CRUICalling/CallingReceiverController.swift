
import AVFoundation
import Foundation
import Lottie
import RxSwift
import SnapKit
import CRUICore
import ProgressHUD
import CloudroomVideoSDK_IOS

public class CallingReceiverController: CallingBaseController {
    
    private var signal: ReceiverSignalViewController?
    private var isPresented: Bool = false

    public var duration: Int {
        signal?.linkingDuration ?? 0
    }
    
    public override func connectRoom(meetID: String) {
        signal?.connectRoom(meetID: meetID)
    }
    
    public override func dismiss() {
        isPresented = false
        signal?.dismiss()
    }
    
    public override func startLiveChat(inviter: @escaping UserInfoHandler,
                                       others: @escaping UserInfoHandler,
                                       isVideo: Bool = true) {
        if isPresented {
            return
        }
        isPresented = true
        
        signal = ReceiverSignalViewController()
        signal!.inviter = inviter
        signal!.users = others
        signal!.isVideo = isVideo
        signal!.onAccepted = onAccepted
        signal!.onRejected = onRejected
        signal!.onHungup = onHungup
        signal!.onDisconnect = onDisconnect
        signal!.onConnectFailure = onConnectFailure
        signal!.onBeHungup = onBeHungup
        
        signal!.modalPresentationStyle = .overCurrentContext
        UIViewController.currentViewController().present(signal!, animated: true)
    }
}

class ReceiverSignalViewController: CallingBaseViewController {
    private var verStackView: UIStackView?
    
    override open func viewDidLoad() {
        super.viewDidLoad()
        playSounds()
        setupView()
    }
    
    func setupView() {
        let inviter = inviter().first

        let avatarLenght = 70.0
        let avatarView = AvatarView()
        avatarView.layer.cornerRadius = avatarLenght/2
        avatarView.setAvatar(url: inviter?.faceURL, text: inviter?.nickname)
        avatarView.snp.makeConstraints { make in
            make.width.height.equalTo(avatarLenght)
        }
        
        let nameLabel = UILabel()
        nameLabel.layer.cornerRadius = 6
        nameLabel.layer.masksToBounds = true
        nameLabel.text = inviter?.nickname
        nameLabel.font = .systemFont(ofSize: 28)
        nameLabel.textAlignment = .center
        nameLabel.textColor = .white
        
        if !isVideo {
            let infoStackView = UIStackView(arrangedSubviews: [tipsLabel, avatarView])
            infoStackView.axis = .vertical
            infoStackView.spacing = 24
            infoStackView.alignment = .center
            
            tipsLabel.text = "invitedVoiceCallHint".innerLocalized()
            verStackView = UIStackView(arrangedSubviews: [infoStackView, UIView()])
        } else {
            let infoStackView = UIStackView(arrangedSubviews: [tipsLabel, nameLabel])
            infoStackView.axis = .vertical
            infoStackView.distribution = .equalSpacing
            infoStackView.alignment = .leading
            
            let rowStackView = UIStackView(arrangedSubviews: [SizeBox(width: 24), avatarView, SizeBox(width: 8), infoStackView, SizeBox(width: 24)])
            rowStackView.axis = .horizontal
            rowStackView.spacing = 8
            
            tipsLabel.text = "invitedVideoCallHint".innerLocalized()
            verStackView = UIStackView(arrangedSubviews: [rowStackView, UIView()])
        }
        
        verStackView!.axis = .vertical
        verStackView!.distribution = .equalSpacing
        view.addSubview(verStackView!)
        
        verStackView!.snp.makeConstraints { make in
            make.left.right.equalToSuperview()
            make.top.equalTo(minimizeButton.snp_bottom).offset(24)
            make.bottom.equalTo(view.safeAreaLayoutGuide.snp.bottom)
        }
        
        insertLinkingViewAbove(aboveView: verStackView!)
        
        if !isJoinRoom {
            previewFuncButtons()
        }
    }
}
