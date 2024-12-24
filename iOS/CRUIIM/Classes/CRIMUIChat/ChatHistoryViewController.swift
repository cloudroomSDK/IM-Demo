
import ChatLayout
import DifferenceKit
import Foundation
import UIKit
import CRUICore
import CRUICoreView
import ProgressHUD
import Photos
import RxSwift

final class ChatHistoryViewController: UIViewController {
    
    private enum ReactionTypes {
        case delayedUpdate
    }
    
    private enum InterfaceActions {
        case changingKeyboardFrame
        case changingContentInsets
        case changingFrameSize
        case sendingMessage
        case scrollingToTop
        case scrollingToBottom
        case showingPreview
        case showingAccessory
        case updatingCollectionInIsolation
    }
    
    private enum ControllerActions {
        case loadingInitialMessages
        case loadingPreviousMessages
        case updatingCollection
    }
    
    override var canBecomeFirstResponder: Bool {
        false
    }
    
    private var currentInterfaceActions: SetActor<Set<InterfaceActions>, ReactionTypes> = SetActor()
    private var currentControllerActions: SetActor<Set<ControllerActions>, ReactionTypes> = SetActor()
    private var collectionView: UICollectionView!
    private var chatLayout = CollectionViewChatLayout()
    
    private var oldLeftBarButtonItem: UIBarButtonItem?
    
    private let _disposeBag = DisposeBag()
    
    private let chatController: ChatHistoryController
    private let dataSource: ChatCollectionDataSource
    private var animator: ManualAnimator?
    
    private var translationX: CGFloat = 0
    private var currentOffset: CGFloat = 0
    
    private var documentInteractionController: UIDocumentInteractionController!
        
    lazy var settingButton: UIBarButtonItem = {
        let v = UIBarButtonItem(image: UIImage(nameInBundle: "common_more_btn_icon"), style: .done, target: self, action: #selector(settingButtonAction))
        v.tintColor = .black
        
        return v
    }()
    
    @objc
    private func settingButtonAction() {
        
    }

    init(chatController: ChatHistoryController,
         dataSource: ChatCollectionDataSource) {
        self.chatController = chatController
        self.dataSource = dataSource
        
        super.init(nibName: nil, bundle: nil)
    }

    @available(*, unavailable, message: "Use init(messageController:) instead")
    override convenience init(nibName nibNameOrNil: String?, bundle nibBundleOrNil: Bundle?) {
        fatalError()
    }
    
    @available(*, unavailable, message: "Use init(messageController:) instead")
    required init?(coder: NSCoder) {
        fatalError()
    }
    
    deinit {
        print("chat view controller - deinit")
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        if #available(iOS 13.0, *) {
            view.backgroundColor = .systemBackground
        } else {
            view.backgroundColor = .white
        }
        setupTitle()
        updateLeftButtons()
        
        chatLayout.settings.interItemSpacing = 8
        chatLayout.settings.interSectionSpacing = 8
        chatLayout.settings.additionalInsets = UIEdgeInsets(top: 8, left: 5, bottom: 8, right: 5)
        chatLayout.keepContentOffsetAtBottomOnBatchUpdates = true
        chatLayout.processOnlyVisibleItemsOnAnimatedBatchUpdates = false
        
        collectionView = UICollectionView(frame: view.frame, collectionViewLayout: chatLayout)
        view.addSubview(collectionView)
        collectionView.alwaysBounceVertical = true
        collectionView.dataSource = dataSource
        chatLayout.delegate = dataSource
        collectionView.keyboardDismissMode = .interactive
        
        /// https://openradar.appspot.com/40926834
        collectionView.isPrefetchingEnabled = false
        
        collectionView.contentInsetAdjustmentBehavior = .always
        if #available(iOS 13.0, *) {
            collectionView.automaticallyAdjustsScrollIndicatorInsets = true
        }
        
        collectionView.translatesAutoresizingMaskIntoConstraints = false
        collectionView.frame = view.bounds
        NSLayoutConstraint.activate([
            collectionView.topAnchor.constraint(equalTo: view.topAnchor, constant: 0),
            collectionView.bottomAnchor.constraint(equalTo: view.bottomAnchor, constant: 0),
            collectionView.leadingAnchor.constraint(equalTo: view.leadingAnchor, constant: 0),
            collectionView.trailingAnchor.constraint(equalTo: view.trailingAnchor, constant: 0)
        ])
        collectionView.backgroundColor = .clear
        collectionView.showsHorizontalScrollIndicator = false
        dataSource.prepare(with: collectionView)
        
        currentControllerActions.options.insert(.loadingInitialMessages)
        chatController.loadInitialMessages { [weak self] sections in
            self?.processUpdates(with: sections, animated: false, requiresIsolatedProcess: false)
            self?.currentControllerActions.options.remove(.loadingInitialMessages)
        }
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        collectionView.collectionViewLayout.invalidateLayout()
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        guard isViewLoaded else {
            return
        }
        currentInterfaceActions.options.insert(.changingFrameSize)
        let positionSnapshot = chatLayout.getContentOffsetSnapshot(from: .bottom)
        collectionView.collectionViewLayout.invalidateLayout()
        collectionView.setNeedsLayout()
        coordinator.animate(alongsideTransition: { _ in
            // Gives nicer transition behaviour
            // self.collectionView.collectionViewLayout.invalidateLayout()
            self.collectionView.performBatchUpdates(nil)
        }, completion: { _ in
            if let positionSnapshot,
               !self.isUserInitiatedScrolling {
                // As contentInsets may change when size transition has already started. For example, `UINavigationBar` height may change
                // to compact and back. `CollectionViewChatLayout` may not properly predict the final position of the element. So we try
                // to restore it after the rotation manually.
                self.chatLayout.restoreContentOffset(with: positionSnapshot)
            }
            self.collectionView.collectionViewLayout.invalidateLayout()
            self.currentInterfaceActions.options.remove(.changingFrameSize)
        })
        super.viewWillTransition(to: size, with: coordinator)
    }
    
    private func setupTitle() {
        navigationItem.title = chatController.getMergerMessageTitle()
    }
    
    private func setupGroupTitle() {
        
    }
    
    func updateLeftButtons() {
        navigationItem.leftBarButtonItem = UIBarButtonItem(title: "   ") { [weak self] in
            self?.navigationController?.popViewController(animated: true)
        }
    }
    
    private func setRightButtons(show: Bool) {
        if show {
            navigationItem.rightBarButtonItems = [settingButton]
        } else {
            navigationItem.rightBarButtonItems = nil
        }
    }
}

extension ChatHistoryViewController: UIScrollViewDelegate {
    
    public func scrollViewShouldScrollToTop(_ scrollView: UIScrollView) -> Bool {
        guard scrollView.contentSize.height > 0,
              !currentInterfaceActions.options.contains(.showingAccessory),
              !currentInterfaceActions.options.contains(.showingPreview),
              !currentInterfaceActions.options.contains(.scrollingToTop),
              !currentInterfaceActions.options.contains(.scrollingToBottom) else {
            return false
        }
        // Blocking the call of loadPreviousMessages() as UIScrollView behaves the way that it will scroll to the top even if we keep adding
        // content there and keep changing the content offset until it actually reaches the top. So instead we wait until it reaches the top and initiate
        // the loading after.
        currentInterfaceActions.options.insert(.scrollingToTop)
        return true
    }
    
    public func scrollViewDidScrollToTop(_ scrollView: UIScrollView) {
        guard !currentControllerActions.options.contains(.loadingInitialMessages),
              !currentControllerActions.options.contains(.loadingPreviousMessages) else {
            return
        }
        currentInterfaceActions.options.remove(.scrollingToTop)
        loadPreviousMessages()
    }
    
    func scrollViewDidScroll(_ scrollView: UIScrollView) {
        if currentControllerActions.options.contains(.updatingCollection), collectionView.isDragging {
            // Interrupting current update animation if user starts to scroll while batchUpdate is performed. It helps to
            // avoid presenting blank area if user scrolls out of the animation rendering area.
            UIView.performWithoutAnimation {
                self.collectionView.performBatchUpdates({}, completion: { _ in
                    let context = ChatLayoutInvalidationContext()
                    context.invalidateLayoutMetrics = false
                    self.collectionView.collectionViewLayout.invalidateLayout(with: context)
                })
            }
        }
        guard !currentControllerActions.options.contains(.loadingInitialMessages),
              !currentControllerActions.options.contains(.loadingPreviousMessages),
              !currentInterfaceActions.options.contains(.scrollingToTop),
              !currentInterfaceActions.options.contains(.scrollingToBottom) else {
            return
        }
        
        if scrollView.contentOffset.y <= -(scrollView.adjustedContentInset.top + 50) {
            loadPreviousMessages()
        }
    }
    
    private func loadPreviousMessages() {
        // Blocking the potential multiple call of that function as during the content invalidation the contentOffset of the UICollectionView can change
        // in any way so it may trigger another call of that function and lead to unexpected behaviour/animation
        currentControllerActions.options.insert(.loadingPreviousMessages)
        
    }
    
    fileprivate var isUserInitiatedScrolling: Bool {
        collectionView.isDragging || collectionView.isDecelerating
    }
    
    func scrollToBottom(completion: (() -> Void)? = nil) {
        // I ask content size from the layout because on IOs 12 collection view contains not updated one
        let contentOffsetAtBottom = CGPoint(x: collectionView.contentOffset.x,
                                            y: chatLayout.collectionViewContentSize.height - collectionView.frame.height + collectionView.adjustedContentInset.bottom)
        
        guard contentOffsetAtBottom.y > collectionView.contentOffset.y else {
            completion?()
            return
        }
        
        let initialOffset = collectionView.contentOffset.y
        let delta = contentOffsetAtBottom.y - initialOffset
        if abs(delta) > chatLayout.visibleBounds.height {
            // See: https://dasdom.dev/posts/scrolling-a-collection-view-with-custom-duration/
            animator = ManualAnimator()
            animator?.animate(duration: TimeInterval(0.25), curve: .easeInOut) { [weak self] percentage in
                guard let self else {
                    return
                }
                self.collectionView.contentOffset = CGPoint(x: self.collectionView.contentOffset.x, y: initialOffset + (delta * percentage))
                if percentage == 1.0 {
                    self.animator = nil
                    let positionSnapshot = ChatLayoutPositionSnapshot(indexPath: IndexPath(item: 0, section: 0), kind: .footer, edge: .bottom)
                    self.chatLayout.restoreContentOffset(with: positionSnapshot)
                    self.currentInterfaceActions.options.remove(.scrollingToBottom)
                    completion?()
                }
            }
        } else {
            currentInterfaceActions.options.insert(.scrollingToBottom)
            UIView.animate(withDuration: 0.25, animations: { [weak self] in
                self?.collectionView.setContentOffset(contentOffsetAtBottom, animated: true)
            }, completion: { [weak self] _ in
                self?.currentInterfaceActions.options.remove(.scrollingToBottom)
                completion?()
            })
        }
    }
    
}

// MARK: ChatControllerDelegate

extension ChatHistoryViewController: ChatControllerDelegate {

    func isInGroup(with isIn: Bool) {
        if isIn {
            navigationItem.rightBarButtonItems = [settingButton]
        } else {
            navigationItem.rightBarButtonItems = nil
        }
    }
    
    func didTapAvatar(with id: String) {
        
    }
    
    func didTapContent(with id: String, data: Message.Data) {
        
        switch data {
        case .text(let string):
            print("点击纯文本消息")
            break
        case .attributeText(let nSAttributedString):
            print("点击富文本消息")
            break
        case .url(let uRL, let isLocallyStored):
            print("点击url消息")
            break
        case .image(let source, let isLocallyStored):
            print("didTapContent image \(source.source.url.relativeString)")
            let vc = MediaPreviewViewController(resources: [MediaResource(thumbUrl: source.source.url.relativeString,
                                                                          url: source.source.url.relativeString)])
            vc.hidesBottomBarWhenPushed = true
            navigationController?.addChild(vc)
            navigationController?.view.addSubview(vc.view)
        case .video(let source, let isLocallyStored):
            let vc = MediaPreviewViewController(resources: [MediaResource(thumbUrl: source.thumb?.url.relativeString,
                                                                          url: source.source.url.relativeString,
                                                                          type: .video)])
            vc.hidesBottomBarWhenPushed = true
            navigationController?.addChild(vc)
            navigationController?.view.addSubview(vc.view)
            
        case .file(let source, let isLocallyStored):
            let fileManager = FileManager.default
            var fileURL: URL?
            if let url = source.source.url, fileCache.isEntityCached(for: CacheableFileKey(url: url)), let filePath = try? fileCache.getEntity(for: CacheableFileKey(url: url)) {
                fileURL = URL(fileURLWithPath: documents + filePath)
            } else if let path = source.source.relativePath, let url = URL(string: path), fileCache.isEntityCached(for: CacheableFileKey(url: url)), let filePath = try? fileCache.getEntity(for: CacheableFileKey(url: url)) {
                fileURL = URL(fileURLWithPath: documents + filePath)
            }
            guard let fileURL = fileURL, fileManager.fileExists(atPath: fileURL.path) else {
                debugPrint("preview file failed, file path not exist")
                return
            }
            
            let vc = FilePreviewController(fileURL: fileURL, fileName: source.fileName)
            navigationController?.pushViewController(vc, animated: true)
            
        case .card(let source):
            let vc = UserDetailTableViewController(userId: source.user.id)
            navigationController?.pushViewController(vc, animated: true)
            
        case .location(let source):
            let vc = LocationViewController(locationFor: .locationForShow, latitude: source.latitude, longitude: source.longitude, name: source.desc?.name, addr: source.desc?.addr)
            navigationController?.pushViewController(vc, animated: true)
            
        default:
            break
        }
        print("点击内容")
    }
    
    func didLongPressContent(with id: String, bubbleView: UIView, data: Message.Data) {
        print("长按气泡")
        let messageInfo = chatController.getMessageInfo(ids: [id]).first(where: { $0.clientMsgID == id })
        var toolItems: [ChatToolController.ToolItem] = []
        let isMyMessage = messageInfo?.sendID == IMController.shared.userID
        switch data {
        case .text(let string):
            print("点击纯文本消息")
            toolItems = [.copy, .forward]
            break
        case .attributeText(let nSAttributedString):
            print("点击富文本消息")
            toolItems = [.copy, .forward]
            break
        case .url(let uRL, let isLocallyStored):
            print("点击url消息")
            break
        case .image(let source, let isLocallyStored):
            toolItems = [.forward]
            break
        case .video(let source, let isLocallyStored):
            toolItems = [.forward]
            break
        case .audio(let source, let isLocallyStored):
            break
        case .file(let source, let isLocallyStored):
            toolItems = [.forward]
            break
        case .card(let source):
            toolItems = [.forward]
        case .location(let source):
            toolItems = [.forward]
        case .merger(let source):
            toolItems = [.forward]
            break
        case .quote(let source):
            toolItems = [.copy, .forward]
        case .custom(let source):
            break
        }
        
        if toolItems.isEmpty { return }
        let menu = ChatToolController(sourceView: bubbleView, items: toolItems)

        menu.collectionView.rx.itemSelected.subscribe(onNext: { [weak self, weak menu] (indexPath: IndexPath) in
            let menuItem = toolItems[indexPath.item]
            switch menuItem {
            case .forward:
                guard let self else { return }
                let vc = SelectContactsViewController(maxSelectionLimit: true)
                vc.selectedContact() { [weak self] r in
                    guard let self else { return }
                    
                    self.presentForwardConfirmAlert(contacts: r, abstruct: messageInfo?.getAbstruct() ?? "") { [weak self] _ in
                        guard let self else { return }
                        self.navigationController?.popViewController(animated: true)
                        self.chatController.defaultSelecteMessage(with: id)
                        self.scrollToBottom(completion: {
                            self.chatController.sendForwardMsg(r) { _ in
                            }
                        })
                    }
                }
                vc.hidesBottomBarWhenPushed = true
                self.navigationController?.pushViewController(vc, animated: true)
            case .copy:
                var content: String?
                switch data {
                case .text(let string):
                    content = string.text
                case .attributeText(let nSAttributedString):
                    content = nSAttributedString.string
                default:
                    content = nil
                }
                UIPasteboard.general.string = content
                ProgressHUD.success("复制成功".innerLocalized())
            default:
                break
            }
            menu?.dismiss(animated: true)
        }).disposed(by: menu.disposeBag)

        present(menu, animated: true, completion: nil)
    }
    
    func reeditMessage(with id: String) {
        
        
    }
    
    func update(with sections: [Section], requiresIsolatedProcess: Bool) {
        processUpdates(with: sections, animated: true, requiresIsolatedProcess: requiresIsolatedProcess)
    }
    
    private func processUpdates(with sections: [Section], animated: Bool = true, requiresIsolatedProcess: Bool, completion: (() -> Void)? = nil) {
        guard isViewLoaded else {
            dataSource.sections = sections
            return
        }
        
        guard currentInterfaceActions.options.isEmpty else {
            let reaction = SetActor<Set<InterfaceActions>, ReactionTypes>.Reaction(type: .delayedUpdate,
                                                                                   action: .onEmpty,
                                                                                   executionType: .once,
                                                                                   actionBlock: { [weak self] in
                guard let self else {
                    return
                }
                self.processUpdates(with: sections, animated: animated, requiresIsolatedProcess: requiresIsolatedProcess, completion: completion)
            })
            currentInterfaceActions.add(reaction: reaction)
            return
        }
        
        func process() {
            // If there is a big amount of changes, it is better to move that calculation out of the main thread.
            // Here is on the main thread for the simplicity.
            var changeSet = StagedChangeset(source: dataSource.sections, target: sections).flattenIfPossible()
            guard !changeSet.isEmpty else {
                completion?()
                return
            }
            
            if requiresIsolatedProcess {
                chatLayout.processOnlyVisibleItemsOnAnimatedBatchUpdates = true
                currentInterfaceActions.options.insert(.updatingCollectionInIsolation)
            }
            currentControllerActions.options.insert(.updatingCollection)
            collectionView.reload(using: changeSet,
                                  interrupt: { changeSet in
                guard changeSet.sectionInserted.isEmpty else {
                    return true
                }
                return false
            },
                                  onInterruptedReload: {
                let positionSnapshot = ChatLayoutPositionSnapshot(indexPath: IndexPath(item: 0, section: sections.count - 1), kind: .footer, edge: .bottom)
                self.collectionView.reloadData()
                // We want so that user on reload appeared at the very bottom of the layout
                self.chatLayout.restoreContentOffset(with: positionSnapshot)
            },
                                  completion: { _ in
                DispatchQueue.main.async {
                    self.chatLayout.processOnlyVisibleItemsOnAnimatedBatchUpdates = false
                    if requiresIsolatedProcess {
                        self.currentInterfaceActions.options.remove(.updatingCollectionInIsolation)
                    }
                    completion?()
                    self.currentControllerActions.options.remove(.updatingCollection)
                }
            },
                                  setData: { data in
                self.dataSource.sections = data
            })
        }
        
        if animated {
            process()
        } else {
            UIView.performWithoutAnimation {
                process()
            }
        }
    }
    
}




