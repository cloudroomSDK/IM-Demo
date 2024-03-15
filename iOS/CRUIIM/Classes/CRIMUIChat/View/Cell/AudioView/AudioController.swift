//
//  AudioController.swift
//  CRUIIM
//
//  Created by YunWu01 on 2024/3/1.
//

import Foundation
import UIKit
import CRUICore

final class AudioController {
    weak var view: AudioView? {
        didSet {
            view?.reloadData()
        }
    }
    
    weak var quoteView: AudioQuoteView? {
        didSet {
            quoteView?.reloadData()
        }
    }

    weak var delegate: ReloadDelegate?
    
    var state: VideoViewState {
        guard let image else {
            return .loading
        }
        return .image(image)
    }
    
    var isQuoted: Bool = false
    
    let senderNickname: String?
    let messageType: MessageType
    
    var isPlaying: Bool = false

    private var image: UIImage?

    private let messageId: String
    
    var duration: Int = 0

    private let source: MediaMessageSource

    private let bubbleController: BubbleController

    init(source: MediaMessageSource, messageId: String, bubbleController: BubbleController, senderNickname: String? = nil, isQuoted: Bool = false, messageType: MessageType = .incoming) {
        self.source = source
        self.messageId = messageId
        self.bubbleController = bubbleController
        self.duration = source.duration ?? 0
        self.senderNickname = (senderNickname ?? "") + ":"
        self.isQuoted = isQuoted
        self.isPlaying = source.isPlaying
        self.messageType = messageType
        loadAudio()
    }
    
    private func loadAudio() {
        let relativeFilePath = try? fileCache.getEntity(for: CacheableFileKey(url: source.source.url))
        if let relativeFilePath, let url = URL(string: relativeFilePath), let filePath = FileHelper.shared.exsit(path: relativeFilePath, name: url.lastPathComponent) {
            // exist
            return
        }
        
        FileDownloadManager.manager.downloadMessageFile(messageID: messageId, url: source.source.url, name: source.source.url.lastPathComponent) { [weak self] messageId, url in
            guard let self else { return }
            
            let r = FileHelper.shared.saveAudio(from: url.path)
            var relativeFilePath = r.relativeFilePath
            var fullPath = r.fullPath
            if !r.isSuccess, let filePath = FileHelper.shared.exsit(path: url.path), let url = URL(string: filePath) {
                relativeFilePath = audioDirectory + url.lastPathComponent
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
    }

    func action() {
        delegate?.didTapContent(with: messageId, data: .audio(source, isLocallyStored: true))
    }
    
    func longPressAction() {
        guard let bubbleView = view?.superview?.superview?.superview else { return }
        delegate?.didLongPressContent(with: messageId, bubbleView: bubbleView, data: .audio(source, isLocallyStored: false))
    }
}
