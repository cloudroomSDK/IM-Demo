
import InputBarAccessoryView
import AVFAudio
import UIKit
import CRUICore
import Photos
import MobileCoreServices
import RxCocoa
import RxSwift
import Toast_Swift

enum CustomAttachment {
    case image(String, String)
    case video(String, String, String, Int)
    case file(String, String, String)
    case audio(String, String, Int)
}

enum MessageHandlerType {
    case clearQuote
    case deleteSelected
    case forward
    case atMember
}

// MARK: - CameraInputBarAccessoryViewDelegate
protocol CoustomInputBarAccessoryViewDelegate: InputBarAccessoryViewDelegate {
    func inputBar(_ inputBar: InputBarAccessoryView, didPressSendButtonWith attachments: [CustomAttachment])
    func inputBar(_ inputBar: InputBarAccessoryView, didPressPadItemWith type: PadItemType)
    func inputBar(_ inputBar: InputBarAccessoryView, messageHandlerWith type: MessageHandlerType)
}

extension CoustomInputBarAccessoryViewDelegate {
    func inputBar(_: InputBarAccessoryView, didPressSendButtonWith _: [CustomAttachment]) { }
    func inputBar(_: InputBarAccessoryView, didPressPadItemWith _: PadItemType) {}
    func inputBar(_: InputBarAccessoryView, messageHandlerWith _: MessageHandlerType) {}
}

// MARK: - CameraInputBarAccessoryView
let buttonSize = 35.0

class CoustomInputBarAccessoryView: InputBarAccessoryView {
    
    private let _disposeBag = DisposeBag()
    
    private lazy var _photoHelper: PhotoHelper = {
        let v = PhotoHelper()
        v.didPhotoSelected = { [weak self, weak v] (images: [UIImage], assets: [PHAsset], _: Bool) in
            guard let self else { return }
            sendButton.startAnimating()
            
            for (index, asset) in assets.enumerated() {
                switch asset.mediaType {
                case .video:
                    PhotoHelper.compressVideoToMp4(asset: asset, thumbnail: images[index]) { main, thumb, duration in
                        self.sendAttachments(attachments: [.video(thumb.relativeFilePath,
                                                                  thumb.fullPath,
                                                                  main.fullPath,
                                                                  duration)])
                    }
                case .image:
                    let r = FileHelper.shared.saveImage(image: images[index])
                    self.sendAttachments(attachments: [.image(r.relativeFilePath,
                                                              r.fullPath)])
                default:
                    break
                }
            }
        }

        v.didCameraFinished = { [weak self] (photo: UIImage?, videoPath: URL?) in
            guard let self else { return }
            sendButton.startAnimating()
            
            if let photo {
                let r = FileHelper.shared.saveImage(image: photo)
                self.sendAttachments(attachments: [.image(r.relativeFilePath,
                                                          r.fullPath)])
            }

            if let videoPath {
                PhotoHelper.getVideoAt(url: videoPath) { main, thumb, duration in
                    self.sendAttachments(attachments: [.video(thumb.relativeFilePath,
                                                              thumb.fullPath,
                                                              main.fullPath,
                                                              duration)])
                }
            }
        }
        return v
    }()
        
    lazy var audioButton: InputBarButtonItem = {
        let v = InputBarButtonItem()
            .configure {
                $0.spacing = .fixed(8)
                $0.image = UIImage(nameInBundle: "inputbar_audio_btn_normal_icon")
                $0.setImage(UIImage(nameInBundle: "inputbar_keyboard_btn_icon"), for: .selected)
                $0.setImage(UIImage(nameInBundle: "inputbar_audio_btn_disable_icon"), for: .disabled)
                $0.setSize(CGSize(width: buttonSize, height: buttonSize), animated: false)
            }.onTouchUpInside { [weak self] item in
                print("Item Tapped:\(item.isSelected)")
                guard let self else { return }
                item.isSelected = !item.isSelected
                self.showRecordView(item.isSelected)
            }
        
        return v
    }()
    
    lazy var emojiButton: InputBarButtonItem = {
        let v = InputBarButtonItem()
            .configure {
                $0.spacing = .fixed(8)
                $0.image = UIImage(nameInBundle: "inputbar_emoji_btn_normal_icon")
                $0.setImage(UIImage(nameInBundle: "inputbar_keyboard_btn_icon"), for: .selected)
                $0.setImage(UIImage(nameInBundle: "inputbar_emoji_btn_disable_icon"), for: .disabled)
                $0.setSize(CGSize(width: buttonSize, height: buttonSize), animated: false)
            }.onTouchUpInside { [weak self] item in
                print("Item Tapped:\(item.isSelected)")
                guard let self else { return }
                item.isSelected = !item.isSelected
                self.showEmojiView(item.isSelected)
            }
        
        return v
    }()
    
    lazy var moreButton: InputBarButtonItem = {
        let v = InputBarButtonItem()
            .configure {
                $0.spacing = .fixed(8)
                $0.image = UIImage(nameInBundle: "inputbar_more_normal_icon")
                $0.setImage(UIImage(nameInBundle: "inputbar_keyboard_btn_icon"), for: .selected)
                $0.setImage(UIImage(nameInBundle: "inputbar_more_disable_icon"), for: .disabled)
                $0.setSize(CGSize(width: buttonSize, height: buttonSize), animated: false)
            }.onTouchUpInside { [weak self] item in
                print("Item Tapped:\(item.isSelected)")
                guard let self else { return }
                item.isSelected = !item.isSelected
                self.showPadView(item.isSelected)
            }
        
        return v
    }()
    
    private lazy var _recorder: AVAudioRecorder = {
        let voicePath: String = NSHomeDirectory() + "/Documents/voice.wav"
        let fileUrl = URL(fileURLWithPath: voicePath)
        let settings: [String: Any] = [
            AVFormatIDKey: kAudioFormatLinearPCM,
            AVSampleRateKey: 8000,
            AVNumberOfChannelsKey: 2,
        ]
        do {
            let v = try AVAudioRecorder(url: fileUrl, settings: settings)
            v.delegate = self
            //        v.isMeteringEnabled = true
            return v
        } catch {
            print("初始化Recorder失败：\(error.localizedDescription)")
        }
        return AVAudioRecorder()
    }()

    private var _recordingTimer: Timer?
    private var _seconds: Int = 0
    
    lazy var voiceInputBtn: NoIntrinsicSizeButton = {
        let v = NoIntrinsicSizeButton()
        v.setTitle("按住 说话".innerLocalized(), for: .normal)
        v.setTitle("松开 结束".innerLocalized(), for: .highlighted)
        v.titleLabel?.font = .boldSystemFont(ofSize: 15)
        v.titleLabel?.textAlignment = .center
        v.setTitleColor(StandardUI.color_0089FF, for: .normal)
        v.backgroundColor = .white
        v.layer.cornerRadius = 8
        v.layer.masksToBounds = true
        v.isHidden = true
        v.rx.controlEvent(.touchDown)
            .subscribe(onNext: { [weak self] in
                guard let sself = self else { return }
                self?.parentView.addSubview(sself.recordView)
                sself.recordView.snp.makeConstraints { make in
                    make.edges.equalToSuperview()
                }
                self?.startRecord()
            }).disposed(by: _disposeBag)

        v.rx.controlEvent(.touchUpInside)
            .subscribe(onNext: { [weak self] in
                print("touchUpInside")
                self?.recordView.removeFromSuperview()
                print("录制完毕，发送语音消息")
                self?.stopRecord()
            }).disposed(by: _disposeBag)

        v.rx.controlEvent(.touchUpOutside)
            .subscribe(onNext: { [weak self] in
                guard let sself = self else { return }
                if sself.recordView.cancelBtn.isSelected {
                    self?.stopRecord()
                    self?.recordView.removeFromSuperview()
                    return
                }
                if sself.recordView.convertBtn.isSelected {
                    print("开始语音转换文字")
                    self?.recordView.removeFromSuperview()
                    return
                }
                self?.stopRecord()
                self?.recordView.removeFromSuperview()
            }).disposed(by: _disposeBag)
        v.delegate = self.recordView
        return v
    }()

    let recordView: ChatRecordView = {
        let v = ChatRecordView()
        v.backgroundColor = .black.withAlphaComponent(0.5)
        return v
    }()
    
    lazy var middleContentStackView: UIStackView = {
        let v = UIStackView()
        v.backgroundColor = .white
        v.layer.cornerRadius = 6
        v.spacing = 0
        v.alignment = .fill
        
        return v
    }()
    
    lazy var multipleMenuStackView: UIStackView = {
        let v = UIStackView()
        v.spacing = 5
        v.distribution = .fill
        v.alignment = .center
        v.backgroundColor = UIColor(hex: 0xF5F5F5)
        
        return v
    }()
    
    private lazy var emojiView: ChatEmojiView = {
        let v =  ChatEmojiView(frame: CGRect(x: 0, y: 0, width: kScreenWidth, height: 200))
        v.backgroundColor = .white
        v.delegate = self
        return v
    }()
    
    private var conversation: ConversationInfo!
    private weak var parentView: UIView!
            
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupSubViews()
    }
    
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    deinit {
        NotificationCenter.default.removeObserver(self)
    }
    
    lazy var attachmentManager: AttachmentManager = { [unowned self] in
        let manager = AttachmentManager()
        manager.delegate = self
        
        return manager
    }()
    
    func weakReferenceParentView(superView: UIView, conversation: ConversationInfo) {
        self.conversation = conversation
        parentView = superView
    }
    
    func showChatToolMultipleMenu(_ forceEnd: Bool = false) {
        guard !forceEnd else {
            multipleMenuStackView.removeFromSuperview()
            return
        }
        
        let space_1: UIView = {
            let v = UIView()
            
            return v
        }()
        space_1.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            space_1.widthAnchor.constraint(equalToConstant: 15)
        ])
        
        let space_2: UIView = {
            let v = UIView()
            
            return v
        }()
        space_2.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            space_2.widthAnchor.constraint(equalToConstant: 15)
        ])
        
        let space_3: UIView = {
            let v = UIView()
            
            return v
        }()
        
        let deleteBtn: UIButton = {
            let v = UIButton()
            v.setImage(UIImage(nameInBundle: "inputbar_multiple_delete_icon"), for: .normal)
            
            return v
        }()
        deleteBtn.addTarget(self, action: #selector(deleteMessagesAction), for: .touchUpInside)
        deleteBtn.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            deleteBtn.widthAnchor.constraint(equalToConstant: 20)
        ])
        
        let forwardBtn: UIButton = {
            let v = UIButton()
            v.setImage(UIImage(nameInBundle: "inputbar_multiple_forward_icon"), for: .normal)
            
            return v
        }()
        forwardBtn.addTarget(self, action: #selector(forwardMessagesAction), for: .touchUpInside)
        forwardBtn.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            forwardBtn.widthAnchor.constraint(equalToConstant: 20)
        ])
        
        multipleMenuStackView.arrangedSubviews.forEach { v in
            v.removeFromSuperview()
        }
        let views = [space_1, deleteBtn, space_2, forwardBtn, space_3]
        views.forEach { v in
            multipleMenuStackView.addArrangedSubview(v)
        }
        
        addSubview(multipleMenuStackView)
        multipleMenuStackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            multipleMenuStackView.leadingAnchor.constraint(equalTo: leftStackView.leadingAnchor),
            multipleMenuStackView.topAnchor.constraint(equalTo: leftStackView.topAnchor),
            multipleMenuStackView.trailingAnchor.constraint(equalTo: rightStackView.trailingAnchor),
            multipleMenuStackView.bottomAnchor.constraint(equalTo: rightStackView.bottomAnchor),
        ])
    }
    
    func updateMiddleContentView(_ quote: Bool = false, _ text: String? = nil, _ attributedText: NSAttributedString? = nil) {
        if quote {
            let hStack: UIStackView = {
                let quoteLabel: PaddedUILabel = {
                    let v = PaddedUILabel()
                    v.textColor = .c353535
                    v.font = UIFont.preferredFont(forTextStyle: .footnote)
                    v.numberOfLines = 2
                    v.lineBreakMode = .byTruncatingTail
                    v.text = text
                    v.textInsets = UIEdgeInsets(top: 5, left: 5, bottom: 5, right: 5)
                    
                    return v
                }()
                if text != nil {
                    quoteLabel.text = text
                }
                if attributedText != nil {
                    quoteLabel.attributedText = attributedText
                }
                
                let closeButton: UIButton = {
                    let v = UIButton()
                    v.setImage(UIImage(nameInBundle: "inputbar_quote_close_icon"), for: .normal)
                    v.addTarget(self, action: #selector(clearQuoteMessageAction), for: .touchUpInside)
                    
                    return v
                }()
                closeButton.translatesAutoresizingMaskIntoConstraints = false
                NSLayoutConstraint.activate([
                    closeButton.widthAnchor.constraint(equalToConstant: 16)
                ])
                
                let hStack = UIStackView(arrangedSubviews: [quoteLabel, closeButton])
                hStack.spacing = 8
//                hStack.alignment = .center
                
                return hStack
            }()

            middleContentStackView.arrangedSubviews.forEach { v in
                v.removeFromSuperview()
            }
            middleContentStackView.addArrangedSubview(hStack)
   
            setMiddleContentView(nil, animated: false) // 注意设置前先清理老的
            let vStack = UIStackView(arrangedSubviews: [inputTextView, middleContentStackView])
            vStack.axis = .vertical
            vStack.alignment = .fill
            vStack.spacing = 8
            setMiddleContentView(vStack, animated: false)
        } else {
            middleContentStackView.removeFromSuperview()
        }
    }
    
    func hideBottomButtons() {
        guard !inputTextView.isFirstResponder else {
            inputTextView.resignFirstResponder()
            return
        }
        configBottomButtons(false)
    }
    
    func updateInputBar(groupInfo: GroupInfo, memberInfo: GroupMemberInfo? = nil) {
        guard groupInfo.status != .ok else { return }
        if let memberInfo, memberInfo.isOwnerOrAdmin, groupInfo.status == .muted {
            return
        }

        if inputTextView.isFirstResponder {
            inputTextView.resignFirstResponder()
        }
        configBottomButtons(false)
        
        // 去除左右的按钮
        setLeftStackViewWidthConstant(to: 0, animated: false)
        setStackViewItems([], forStack: .left, animated: false)
        setRightStackViewWidthConstant(to: 0, animated: false)
        setStackViewItems([], forStack: .right, animated: false)
        
        let maskLabel: PaddedUILabel = {
            let v = PaddedUILabel()
            v.textColor = .c0C1C33
            v.font = UIFont.preferredFont(forTextStyle: .body)
            v.textAlignment = .center
            v.backgroundColor = .white
            v.layer.cornerRadius = 8
            v.layer.masksToBounds = true
            v.textInsets = UIEdgeInsets(top: 10, left: 0, bottom: 10, right: 0)
            
            return v
        }()
        
        switch groupInfo.status {
        case .dismissed:
            maskLabel.text = "此群已解散"
        case .muted:
            maskLabel.text = "全员禁言中"
        default:
            break
        }
        setMiddleContentView(maskLabel, animated: false)
    }
    
    @objc private func clearQuoteMessageAction() {
        updateMiddleContentView(false)
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .inputBar(self, messageHandlerWith: .clearQuote)
    }
    
    @objc private func deleteMessagesAction() {
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .inputBar(self, messageHandlerWith: .deleteSelected)
    }
    
    @objc private func forwardMessagesAction() {
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .inputBar(self, messageHandlerWith: .forward)
    }
    
    private func setupSubViews() {
        separatorLine.backgroundColor = .white
        backgroundView.backgroundColor = UIColor(hex: 0xF5F5F5)
        inputTextView.backgroundColor = .white
        inputTextView.layer.cornerRadius = 8
        inputTextView.textColor = .c0C1C33
        inputTextView.font = .preferredFont(forTextStyle: .body)
        inputTextView.placeholder = nil
        inputTextView.delegate = self
        topStackViewPadding = UIEdgeInsets(top: 8, left: 5, bottom: 8, right: 5)
        leftStackView.alignment = .center
        rightStackView.alignment = .center
        configLeftButton()
        configRightButton()
        configVoiceInputButton()
        
        inputPlugins.append(attachmentManager)
    }
    
    // 设置左边的语音
    private func configLeftButton() {
        setLeftStackViewWidthConstant(to: buttonSize + 10, animated: false)
        setStackViewItems([audioButton], forStack: .left, animated: false)
    }
    
    // 设置右边的键盘、发送
    private func configRightButton() {
        sendButton.configure {
            $0.title = "发送".innerLocalized()
            $0.setTitleColor(.white, for: .normal)
            $0.setTitleColor(.c666666, for: .disabled)
            $0.setBackgroundImage(UIImage(nameInBundle: "inputbar_pad_send_normal_icon"), for: .normal)
            $0.setBackgroundImage(UIImage(nameInBundle: "inputbar_pad_send_disable_icon"), for: .disabled)
            $0.layer.cornerRadius = 6.0
            $0.layer.masksToBounds = true
        }
        setRightStackViewWidthConstant(to: 3 * (buttonSize + 10), animated: false)
        setStackViewItems([emojiButton, moreButton, sendButton], forStack: .right, animated: false)
    }
    
    private func configVoiceInputButton() {
        inputTextView.addSubview(voiceInputBtn)
        voiceInputBtn.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            voiceInputBtn.leadingAnchor.constraint(equalTo: inputTextView.leadingAnchor),
            voiceInputBtn.topAnchor.constraint(equalTo: inputTextView.topAnchor),
            voiceInputBtn.trailingAnchor.constraint(equalTo: inputTextView.trailingAnchor),
            voiceInputBtn.bottomAnchor.constraint(equalTo: inputTextView.bottomAnchor),
            voiceInputBtn.widthAnchor.constraint(equalTo: inputTextView.widthAnchor),
            voiceInputBtn.heightAnchor.constraint(equalTo: inputTextView.heightAnchor)
        ])
    }
    
    // 设置底部的按钮
    private func configBottomButtons(_ show: Bool) {
        if show {
            let pad = InputPadView()
            pad.delegate = self
            setStackViewItems([pad], forStack: .bottom, animated: false)
        } else {
            setStackViewItems([], forStack: .bottom, animated: false)
        }
    }
    // 展示更多的pad
    private func showPadView(_ show: Bool) {
        print("点击按钮：\(show)")
        
        if show {
            inputTextView.resignFirstResponder()
        } else {
            inputTextView.becomeFirstResponder()
            moreButton.isSelected = false
        }
        configBottomButtons(show)
        
        audioButton.isSelected = false
        voiceInputBtn.isHidden = true
    }
    
    // 切换到录制语音
    private func showRecordView(_ show: Bool) {
        print("点击按钮：\(show)")
        
        if show {
            inputTextView.resignFirstResponder()
        } else {
            inputTextView.becomeFirstResponder()
            audioButton.isSelected = false
        }
        configBottomButtons(false)
        
        voiceInputBtn.isHidden = !show
        moreButton.isSelected = false
    }
    
    // 切换到表情选择
    private func showEmojiView(_ show: Bool) {
        print("点击按钮：\(show)")
        
        if show {
            inputTextView.inputView = emojiView
        } else {
            inputTextView.inputView = nil
            inputTextView.font = .preferredFont(forTextStyle: .body)
            emojiButton.isSelected = false
        }
        configBottomButtons(false)
        inputTextView.reloadInputViews()
        if !inputTextView.isFirstResponder {
            inputTextView.becomeFirstResponder()
        }
        
        voiceInputBtn.isHidden = true
        moreButton.isSelected = false
    }
    
    // 发送
    private func sendAttachments(attachments: [CustomAttachment]) {
        DispatchQueue.main.async { [self] in
            if attachments.count > 0 {
                (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
                    .inputBar(self, didPressSendButtonWith: attachments)
            }
        }
    }
    
    private func showImagePickerController(sourceType: UIImagePickerController.SourceType) {
        if case .camera = sourceType {
            _photoHelper.presentCamera(byController: currentViewController())
        } else {
            _photoHelper.presentPhotoLibrary(byController: currentViewController())
        }
    }
    
    private func showSystemFilePickerController() {
        let documentPicker = UIDocumentPickerViewController(documentTypes: ["public.data"], in: .import)
        documentPicker.delegate = self
        documentPicker.allowsMultipleSelection = false
        currentViewController().present(documentPicker, animated: true, completion: nil)
    }
    
    private func currentViewController() -> UIViewController {
        var rootViewController: UIViewController?
        for window in UIApplication.shared.windows {
            if window.rootViewController != nil {
                rootViewController = window.rootViewController
                break
            }
        }
        var viewController = rootViewController
        if viewController?.presentedViewController != nil {
            viewController = viewController!.presentedViewController
        }
        return viewController!
    }
    
    private func startRecord() {
        AVAudioSession.sharedInstance().requestRecordPermission { [weak self] granted in
            guard let sself = self else { return }
            if !granted {
                sself.recordView.removeFromSuperview()
                sself.stopRecord()
                
                // TODO: Toast弹窗提示开启权限
                let sheet = SPAlertController.alertController(withTitle:"麦克风权限未开启".innerLocalized(), message: "无法录制声音，前往「设置 > 微信」中打开麦克风权限".innerLocalized(), preferredStyle: .alert)
                sheet.titleColor = .c353535
                sheet.messageColor = .c666666
                let cancelAction = SPAlertAction.action(withTitle: "我知道了".innerLocalized(), style: .default) { [weak self] (action) in
                    
                }
                let toSettingAction = SPAlertAction.action(withTitle: "前往设置".innerLocalized(), style: .default) { [weak self] (action) in
                    UIApplication.shared.open(URL(string: UIApplication.openSettingsURLString)!)
                }
                
                cancelAction.titleColor = .c353535
                toSettingAction.titleColor = .c0584FE
                sheet.addAction(action: cancelAction)
                sheet.addAction(action: toSettingAction)
                sself.currentViewController().present(sheet, animated: true, completion: nil)
                return
            }
            let session = AVAudioSession.sharedInstance()
            do {
                try session.setCategory(AVAudioSession.Category.record)
                try session.setActive(true)
            } catch {
                print("AudioSession初始化失败：", error.localizedDescription)
            }
            if !sself._recorder.prepareToRecord() {
                print("prepare record failed")
                return
            }
            if !sself._recorder.record() {
                print("start record failed")
                return
            }
            sself._seconds = 0
            sself._recordingTimer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true, block: { [weak self] _ in
                guard let sself = self else { return }
                self?._seconds += 1
                let countdown = 60 - sself._seconds
                if countdown <= 0 {
                    self?.recordView.removeFromSuperview()
                    self?.stopRecord()
                }
            })
        }
    }

    private func stopRecord() {
        if _recorder.isRecording {
            _recorder.stop()
            _recordingTimer?.invalidate()
            _recordingTimer = nil
            try? AVAudioSession.sharedInstance().setActive(false)
        }
    }

    private func cancelRecord() {
        stopRecord()
    }
    
    // 重写父类函数
    override func inputTextViewDidBeginEditing() {
        moreButton.isSelected = false
        configBottomButtons(false)
    }
}

// MARK: AttachmentManagerDelegate

extension CoustomInputBarAccessoryView: AttachmentManagerDelegate {
    func attachmentManager(_ manager: AttachmentManager, shouldBecomeVisible: Bool) {
        
    }
}

// MARK: UIAdaptivePresentationControllerDelegate

extension CoustomInputBarAccessoryView: UIAdaptivePresentationControllerDelegate {
    // Swipe to dismiss image modal
    public func presentationControllerWillDismiss(_: UIPresentationController) {
        isHidden = false
    }
}

extension CoustomInputBarAccessoryView: InputPadViewDelegate {
    func didSelect(type: PadItemType) {
        print("chat plugin did select: \(type)")
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .inputBar(self, didPressPadItemWith: type)
        switch type {
        case .album:
            showImagePickerController(sourceType: .photoLibrary)
        case .camera:
            showImagePickerController(sourceType: .camera)
        case .file:
            showSystemFilePickerController()
        default:
            break
        }
    }
}

// MARK: - AVAudioRecorderDelegate

extension CoustomInputBarAccessoryView: AVAudioRecorderDelegate {
    func audioRecorderDidFinishRecording(_: AVAudioRecorder, successfully flag: Bool) {
        if !flag { return }
        if _seconds < 1 {
            // TODO: Toast提示录音时间太短
            return
        }
        let audioName = FileHelper.shared.getAudioName()
        let relativeFilePath = audioDirectory + audioName
        let path = documents + relativeFilePath
        AudioFileConverter.convertAudioToM4a(inputUrlString: _recorder.url.path, outputUrlString: path) { [weak self] (error: Error?) in
            print("语音路径：\(NSHomeDirectory())")
            guard let sself = self else { return }
            if let error = error {
                print("转换格式错误：\(error.localizedDescription)")
            } else {
                sself.sendAttachments(attachments: [.audio(relativeFilePath,
                                                           path,
                                                           _seconds)])
            }
        }
        try? FileManager.default.removeItem(at: _recorder.url)
    }
}

extension CoustomInputBarAccessoryView: ChatEmojiViewDelegate {
    func emojiViewDidSelect(emojiStr: String) {
        let selectedRange = inputTextView.selectedRange
        let emojiAttrString = NSMutableAttributedString(string: emojiStr)
        EmojiHelper.shared.markReplaceableRange(inAttributedString: emojiAttrString, withString: emojiStr)

        let attrText = NSMutableAttributedString(attributedString: inputTextView.attributedText)
        attrText.replaceCharacters(in: selectedRange, with: emojiAttrString)
        inputTextView.attributedText = attrText
        inputTextView.selectedRange = NSRange(location: selectedRange.location + emojiAttrString.length, length: 0)
        refreshDisplayText()
        //sendBtn.isHidden = false
    }

    private func refreshDisplayText() {
        if inputTextView.text.isEmpty {
            return
        }

        if let markedRange = inputTextView.markedTextRange {
            let position = inputTextView.position(from: markedRange.start, offset: 0)
            if position == nil {
                // 处于输入拼音还未确定的中间状态
                return
            }
        }

        let selectedRange = inputTextView.selectedRange
        let plainText = EmojiHelper.shared.getPlainTextIn(attributedString: inputTextView.attributedText, atRange: NSRange(location: 0, length: inputTextView.attributedText.length))

        let attr: [NSAttributedString.Key: Any] = [
            NSAttributedString.Key.font: UIFont.preferredFont(forTextStyle: .body),
            NSAttributedString.Key.foregroundColor: StandardUI.color_333333,
        ]
        let attributedContent = EmojiHelper.shared.replaceTextWithEmojiIn(attributedString: NSAttributedString(string: plainText as String, attributes: attr))

        let offset = inputTextView.attributedText.length - attributedContent.length
        inputTextView.attributedText = attributedContent
        inputTextView.selectedRange = NSRange(location: selectedRange.location - offset, length: 0)
    }
}

// MARK: - UITextViewDelegate

extension CoustomInputBarAccessoryView: UITextViewDelegate {
    func textView(_: UITextView, shouldChangeTextIn _: NSRange, replacementText text: String) -> Bool {

        var needUpdateText = false
        if conversation.conversationType == .group {
            if text == "@" {
                print("弹出选择群成员窗口")
                (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
                    .inputBar(self, messageHandlerWith: .atMember)
            }

            if text.isEmpty {
                print("执行删除@联系人的逻辑")
            }
        }
        return true
    }

    func textViewDidChange(_: UITextView) {
        refreshDisplayText()
    }
}

// MARK: - UIDocumentPickerDelegate

extension CoustomInputBarAccessoryView: UIDocumentPickerDelegate {
    
    func documentPicker(_ controller: UIDocumentPickerViewController, didPickDocumentsAt urls: [URL]) {
        guard let selectedFileURL = urls.first else {
            return
        }
        
        let fileName = selectedFileURL.lastPathComponent
        let r = FileHelper.shared.saveFile(from: selectedFileURL.path)
        var relativeFilePath = r.relativeFilePath
        var fullPath = r.fullPath
        if !r.isSuccess, let filePath = FileHelper.shared.exsit(path: selectedFileURL.path), let url = URL(string: filePath) {
            relativeFilePath = fileDirecotory + url.lastPathComponent
            fullPath = filePath
        }
        sendAttachments(attachments: [.file(relativeFilePath,
                                            fullPath,
                                            fileName)])
    }
    
    func documentPickerWasCancelled(_ controller: UIDocumentPickerViewController) {
        // 用户取消选择文件时的处理
    }
    
}
