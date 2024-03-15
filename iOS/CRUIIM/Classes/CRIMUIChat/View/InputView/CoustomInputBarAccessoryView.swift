
import InputBarAccessoryView
import AVFAudio
import UIKit
import CRUICore
import Photos
import MobileCoreServices
import RxCocoa
import RxSwift

enum CustomAttachment {
    case image(String, String)
    case video(String, String, String, Int)
    case file(String, String, String)
    case audio(String, String, Int)
}

// MARK: - CameraInputBarAccessoryViewDelegate
protocol CoustomInputBarAccessoryViewDelegate: InputBarAccessoryViewDelegate {
    func inputBar(_ inputBar: InputBarAccessoryView, didPressSendButtonWith attachments: [CustomAttachment])
    func inputBar(_ inputBar: InputBarAccessoryView, didPressPadItemWith type: PadItemType)
    func clearSelectQuoteMessage(_ inputBar: InputBarAccessoryView)
    func deleteMessages(_ inputBar: InputBarAccessoryView)
    func forwardMessages(_ inputBar: InputBarAccessoryView)
}

extension CoustomInputBarAccessoryViewDelegate {
    func inputBar(_: InputBarAccessoryView, didPressSendButtonWith _: [CustomAttachment]) { }
    func inputBar(_: InputBarAccessoryView, didPressPadItemWith _: PadItemType) {}
    func clearSelectQuoteMessage(_: InputBarAccessoryView) {}
    func deleteMessages(_: InputBarAccessoryView) {}
    func forwardMessages(_: InputBarAccessoryView) {}
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
        v.setTitleColor(StandardUI.color_333333, for: .normal)
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
    
    func weakReferenceParentView(superView: UIView) {
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
        let views = [space_1, deleteBtn, space_2, space_3]
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
                let quoteLabel: UILabel = {
                    let v = UILabel()
                    v.textColor = .c999999
                    v.font = UIFont.preferredFont(forTextStyle: .footnote)
                    v.numberOfLines = 2
                    v.lineBreakMode = .byTruncatingTail
                    v.text = text
                    
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
            
            let containerLeft: UIView = {
                let v = UIView()
                v.translatesAutoresizingMaskIntoConstraints = false
                NSLayoutConstraint.activate([
                    v.widthAnchor.constraint(equalToConstant: 5)
                ])
                
                return v
            }()
            
            let containerRight: UIView = {
                let v = UIView()
                v.translatesAutoresizingMaskIntoConstraints = false
                NSLayoutConstraint.activate([
                    v.widthAnchor.constraint(equalToConstant: 5)
                ])
                
                return v
            }()

            middleContentStackView.arrangedSubviews.forEach { v in
                v.removeFromSuperview()
            }
            let views = [containerLeft, hStack, containerRight]
            views.forEach { v in
                middleContentStackView.addArrangedSubview(v)
            }
   
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
    
    @objc private func clearQuoteMessageAction() {
        updateMiddleContentView(false)
    }
    
    @objc private func deleteMessagesAction() {
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .deleteMessages(self)
    }
    
    @objc private func forwardMessagesAction() {
        (self.delegate as? CoustomInputBarAccessoryViewDelegate)?
            .forwardMessages(self)
    }
    
    private func setupSubViews() {
        separatorLine.backgroundColor = .white
        backgroundView.backgroundColor = UIColor(hex: 0xF5F5F5)
        inputTextView.backgroundColor = .white
        inputTextView.layer.cornerRadius = 8
        inputTextView.textColor = .c0C1C33
        inputTextView.font = .preferredFont(forTextStyle: .body)
        inputTextView.placeholder = nil
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
        setRightStackViewWidthConstant(to: 2 * (buttonSize + 10), animated: false)
        setStackViewItems([moreButton, sendButton], forStack: .right, animated: false)
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
                // TODO: Toast弹窗提示开启权限
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
