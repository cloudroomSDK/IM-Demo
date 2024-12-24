
import AVFoundation
import Foundation
import RxSwift
import SnapKit
import ProgressHUD
import CRUICore
import CloudroomVideoSDK_IOS

public class CallingSenderController: CallingBaseController {
    
    private var signal: SignalViewController?
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
                                       isVideo: Bool = true)
    {
        if isPresented {
            return
        }
        isPresented = true
        
        signal = SignalViewController()
        signal!.inviter = inviter
        signal!.users = others
        signal!.isVideo = isVideo
        signal!.onCancel = onCancel
        signal!.onHungup = onHungup
        signal!.onDisconnect = onDisconnect
        signal!.onConnectFailure = onConnectFailure
        signal!.onInvitedOthers = onInvitedOthers
        
        signal!.modalPresentationStyle = .overCurrentContext
        UIViewController.currentViewController().present(signal!, animated: true)
    }
}

class SignalViewController: CallingBaseViewController {
    private var verStackView: UIStackView?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        playSounds()
        setupView()
    }
    
    func setupView() {
        tipsLabel.text = "waitingVoiceCallHint".innerLocalized()
        let inviter = users().first
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
            let infoStackView = UIStackView(arrangedSubviews: [tipsLabel, avatarView, nameLabel])
            infoStackView.axis = .vertical
            infoStackView.spacing = 24
            infoStackView.alignment = .center
            
            verStackView = UIStackView(arrangedSubviews: [infoStackView, UIView()])
        } else {
            let infoStackView = UIStackView(arrangedSubviews: [tipsLabel, nameLabel])
            infoStackView.axis = .vertical
            infoStackView.distribution = .equalSpacing
            infoStackView.alignment = .leading
            
            let rowStackView = UIStackView(arrangedSubviews: [SizeBox(width: 24), avatarView, infoStackView, SizeBox(width: 24)])
            rowStackView.axis = .horizontal
            rowStackView.spacing = 8
            
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
        senderPreviewFuncButtons()
    }
}
