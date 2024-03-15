//
//  FilePreviewController.swift
//  CRUICoreView
//
//  Created by YunWu01 on 2024/3/8.
//

import UIKit
import QuickLook

class PreviewItem: NSObject, QLPreviewItem {
    var previewItemURL: URL?
    var previewItemTitle: String?
    init(url: URL? = nil, title: String? = nil) {
        previewItemURL = url
        previewItemTitle = title
    }
}

public class FilePreviewController: QLPreviewController, QLPreviewControllerDataSource {
    var previewItem: PreviewItem!
    
    public convenience init(fileURL: URL, fileName: String?) {
        self.init()
        self.dataSource = self
        
        previewItem = PreviewItem(url: fileURL, title: fileName)
    }
    
    // QLPreviewControllerDataSource methods
    public func numberOfPreviewItems(in controller: QLPreviewController) -> Int {
        return 1
    }
    
    public func previewController(_ controller: QLPreviewController, previewItemAt index: Int) -> QLPreviewItem {
        return previewItem
    }
}


//public class FilePreviewController: UIViewController, UIDocumentInteractionControllerDelegate {
//    var documentController: UIDocumentInteractionController?
//    
//    public convenience init(fileURL: URL) {
//        self.init()
//        documentController = UIDocumentInteractionController(url: fileURL)
//        documentController?.delegate = self
//    }
//    
//    public override func viewDidLoad() {
//        super.viewDidLoad()
//        
//        if let interactionController = documentController {
//            interactionController.presentPreview(animated: true)
//        }
//    }
//    
//    // UIDocumentInteractionControllerDelegate methods
//    public func documentInteractionControllerViewControllerForPreview(_ controller: UIDocumentInteractionController) -> UIViewController {
//        return self
//    }
//}
