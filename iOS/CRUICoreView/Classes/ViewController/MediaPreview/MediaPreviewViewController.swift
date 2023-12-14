
import Lantern
import AVKit
import Kingfisher
import ProgressHUD
import ZLPhotoBrowser

public enum PreviewMediaType {
    case image
    case video
    case gif
}

public struct MediaResource {
    let thumbUrl: String?
    let url: String
    let type: PreviewMediaType
    
    public init(thumbUrl: String? = nil, url: String, type: PreviewMediaType = .image) {
        self.thumbUrl = thumbUrl
        self.url = url
        self.type = type
    }
}

public class MediaPreviewViewController: UIViewController {
    
    public init(resources: [MediaResource]) {
        super.init(nibName: nil, bundle: nil)
        self.dataSource = resources
    }
    
    private let lantern = Lantern()
    private var dataSource: [MediaResource]!
        
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    public override func viewDidLoad() {
        super.viewDidLoad()
        
        
        
        lantern.numberOfItems = { [weak self] in
            self?.dataSource.count ?? 0
        }
        lantern.cellClassAtIndex = { [weak self] index in
            guard let self else { return ImageZoomCell.self }
            let resource = self.dataSource[index]
            return resource.type == .video ? VideoZoomCell.self : ImageZoomCell.self
        }
        lantern.reloadCellAtIndex = { [weak self] context in
            LanternLog.high("reload cell!")
            guard let resource = self?.dataSource[context.index] else { return }
            if resource.type == .video {
                let lanternCell = context.cell as? VideoZoomCell
                let thumbFileURL = self?.convertToLocalFileURL(resource.thumbUrl ?? "")
                lanternCell?.imageView.setImage(with: thumbFileURL)
                lanternCell?.frameChangedHandler = { frame in
                    self?.view.frame = frame
                }
                lanternCell?.dismissHandler = {
                    self?.view.removeFromSuperview()
                    self?.removeFromParent()
                }
                
                // 这里特别注意下，目前SDK给到的地址需要获取重定向地址
                let fileURL = self?.convertToLocalFileURL(resource.url)
                let url = URL(string: fileURL ?? "")
                lanternCell?.videoURL = url
                
            } else {
                let lanternCell = context.cell as? ImageZoomCell
                let fileURL = self?.convertToLocalFileURL(resource.url)
                lanternCell?.imageView.setImage(with: fileURL)
                lanternCell?.frameChangedHandler = { frame in
                    self?.view.frame = frame
                }
                lanternCell?.dismissHandler = {
                    self?.view.removeFromSuperview()
                    self?.removeFromParent()
                }
                lanternCell?.longPressedAction = { imageCell, _ in
                    guard let image = imageCell.imageView.image else { return }
                    self?.showSaveImageAlert(image)
                }
            }
        }
        lantern.cellWillAppear = { cell, index in
            (cell as? VideoZoomCell)?.play()
        }
        lantern.cellWillDisappear = { cell, index in
            (cell as? VideoZoomCell)?.pause()
        }
        
        lantern.show()
    }
    
    public override var shouldAutorotate: Bool {
        return true
    }
    
    public override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .all
    }
    
    func convertToLocalFileURL(_ path: String) -> String {
        let fileManager = FileManager.default
        
        if fileManager.fileExists(atPath: path) {
            let fileURL = URL(fileURLWithPath: path)
            return fileURL.absoluteString
        } else {
            return path
        }
    }

    
    func showSaveImageAlert(_ image: UIImage) {
        func saveImage() {
            let hud = ZLProgressHUD(style: ZLPhotoConfiguration.default().hudStyle)
            hud.show()
            ZLPhotoManager.saveImageToAlbum(image: image) { [weak self] (suc, _) in
                hud.hide()
                if !suc {
                    ProgressHUD.showSucceed("保存图片成功".innerLocalized(), delay: 1)
                }
            }
        }
        
        let alert = UIAlertController(title: nil, message: nil, preferredStyle: .actionSheet)
        let save = UIAlertAction(title: "保存".innerLocalized(), style: .default) { (_) in
            saveImage()
        }
        let cancel = UIAlertAction(title: "取消".innerLocalized(), style: .cancel, handler: nil)
        alert.addAction(save)
        alert.addAction(cancel)
        lantern.present(alert, animated: true)
    }
}
