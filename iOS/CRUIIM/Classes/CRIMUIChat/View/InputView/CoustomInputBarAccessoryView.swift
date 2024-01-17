
import InputBarAccessoryView
import UIKit
import CRUICore
import Photos
import MobileCoreServices

enum CustomAttachment {
    case image(String, String)
    case video(String, String, String, Int)
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
        configRightButton()
        
        inputPlugins.append(attachmentManager)
    }
    
    // 设置右边的键盘
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
        default:
            break
        }
    }
}
