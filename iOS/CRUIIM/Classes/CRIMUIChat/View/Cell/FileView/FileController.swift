//
//  FileController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/7.
//

import Foundation
import UIKit
import CRUICore
import RxSwift

extension DocumentType {
    func getIconImageName() -> String {
        switch self {
        case .excel:
            return "chat_msg_file_excel_normal_icon"
        case .word:
            return "chat_msg_file_word_normal_icon"
        case .ppt:
            return "chat_msg_file_ppt_normal_icon"
        case .pdf:
            return "chat_msg_file_pdf_normal_icon"
        case .zip:
            return "chat_msg_file_zip_normal_icon"
        default:
            return "chat_msg_file_unknown_normal_icon"
        }
    }
}

final class FileController {

    weak var view: FileView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: FileQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var isQuoted: Bool = false
    
    let senderNickname: String
    
    let fileName: String
    
    let fileSizeStr: String
    
    var progress: Int = 0
    
    let status: MessageStatus
    let messageType: MessageType
    
    var fileExist: Bool = false

    private let messageId: String

    private let source: MediaMessageSource

    private let bubbleController: BubbleController
    
    private let _disposeBag = DisposeBag()

    init(source: MediaMessageSource, messageId: String, status: MessageStatus, messageType: MessageType = .incoming, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false) {
        self.source = source
        self.fileName = source.fileName ?? ""
        self.fileSizeStr = FileHelper.formatLength(length: source.fileSize)
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.senderNickname = (senderNickname ?? "") + ":" + " [文件] "
        self.isQuoted = isQuoted
        self.status = status
        self.messageType = messageType
        
        let relativeFilePath = try? fileCache.getEntity(for: CacheableFileKey(url: source.source.url))
        if let relativeFilePath, let url = URL(string: relativeFilePath) {
            self.fileExist = FileHelper.shared.exsit(path: relativeFilePath, name: url.lastPathComponent) != nil
        }
        
        bindData()
    }
    
    private func bindData() {
        guard !isQuoted else { return }
        
        IMController.shared.msgSendProgressSubject.subscribe { [weak self] (messageId, progress) in
            guard messageId == self?.messageId else { return }
            self?.progress = progress
            self?.view?.updateProgress()
        }.disposed(by: _disposeBag)
    }
    
    private func loadImage() {
       
    }

    func action() {
        guard fileExist else {
            FileDownloadManager.manager.downloadMessageFile(messageID: messageId, url: source.source.url, name: source.source.url.lastPathComponent) { messageId, totalBytesWritten, totalBytesExpectedToWrite in
                DispatchQueue.main.async { [weak self] in
                    guard let self else { return }
                    
                    guard messageId == self.messageId else { return }
                    self.progress = Int(Float(totalBytesWritten)/Float(totalBytesExpectedToWrite)*100)
                    print("FileController downloadMessageFile progress: \(self.progress)")
                    self.view?.updateProgress()
                }
            } completion: { [weak self] messageId, url in
                guard let self else { return }
                
                print("FileController completion url: \(url)")
                let r = FileHelper.shared.saveFile(from: url.path)
                var relativeFilePath = r.relativeFilePath
                var fullPath = r.fullPath
                if !r.isSuccess, let filePath = FileHelper.shared.exsit(path: url.path), let url = URL(string: filePath) {
                    relativeFilePath = fileDirecotory + url.lastPathComponent
                    fullPath = filePath
                }
                do {
                    try fileCache.store(entity: relativeFilePath, for: CacheableFileKey(url: self.source.source.url))
                    try FileManager.default.removeItem(at: url)
                    print("cache file success relativeFilePath: \(relativeFilePath)")
                } catch (let e) {
                    print("cache file failure:\(e)")
                }
                self.delegate?.reloadMessage(with: self.messageId)
            }
            
            return
        }
        
        delegate?.didTapContent(with: messageId, data: .file(source, isLocallyStored: true))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .file(source, isLocallyStored: false))
    }
}

