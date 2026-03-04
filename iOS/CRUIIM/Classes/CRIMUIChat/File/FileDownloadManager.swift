
import Alamofire

struct DownloadCallBack {
    typealias DownloadProgressReturnVoid = (String, Int, Int) -> Void
    typealias CompletionReturnVoid = (String, URL) -> Void
}

public struct FileDownloadRequest {
    let request: DownloadRequest
}

public class FileDownloadManager {
    public static let manager = FileDownloadManager()
    
    private let progressKey = "progressKey"
    private let completionKey = "completionKey"
    private let messageIDKey = "messageIDKey"
    private var downloader = FileDownloader()
    private var tasksHandlers: [Int: [String: Any]] = [:]
    
    public func updateSSLTrustMode(trust: Bool, host: String) {
        downloader = trust ? FileDownloader() : FileDownloader(trustMode: .disabled, host: host)
    }
        
    func downloadMessageFile(messageID: String,
                             url: URL,
                             name: String? = nil,
                             progress: DownloadCallBack.DownloadProgressReturnVoid? = nil,
                             completion: DownloadCallBack.CompletionReturnVoid? = nil) -> FileDownloadRequest? {
        
        guard let url = try? url.asURL() else { return nil }
        
        let request = downloader.download(url, name: name)
        guard let taskIdentifier = request.task?.taskIdentifier else { return nil }
        
        tasksHandlers[taskIdentifier] = [progressKey: progress,
                                       completionKey: completion,
                                        messageIDKey: messageID]
        downloader.manager.delegate.taskDidComplete = { [weak self] (session, task, error) in
            if let error {
                if let resumeData = (error as NSError).userInfo[NSURLSessionDownloadTaskResumeData] as? Data {
                    self?.downloader.resumeData = resumeData
                }
            } else {
                print("taskDidComplete: \(task)")
            }
        }
        
        downloader.manager.delegate.downloadTaskDidFinishDownloadingToURL = { [weak self] (session, downloadTask, location) in
            guard let self,
                  let completion = self.tasksHandlers[downloadTask.taskIdentifier]?[self.completionKey] as? DownloadCallBack.CompletionReturnVoid,
                  let messageID = self.tasksHandlers[downloadTask.taskIdentifier]?[self.messageIDKey] as? String else { return }
            do {
                let des = URL.init(fileURLWithPath: downloader.filePath)
                try FileManager.default.moveItem(at: location, to: des)
                completion(messageID, des)
            } catch let error {
                print("downloadTaskDidFinishDownloadingToURL - error: \(error.localizedDescription)")
            }
        }
        
        downloader.manager.delegate.downloadTaskDidWriteData = { [weak self] (session, downloadTask, bytesWritten, totalBytesWritten, totalBytesExpectedToWrite) in
            guard let self,
                  let progress = self.tasksHandlers[downloadTask.taskIdentifier]?[self.progressKey] as? DownloadCallBack.DownloadProgressReturnVoid,
                  let messageID = self.tasksHandlers[downloadTask.taskIdentifier]?[self.messageIDKey] as? String else { return }
            progress(messageID, Int(totalBytesWritten), Int(totalBytesExpectedToWrite))
        }
        
        return FileDownloadRequest(request: request)
    }
}

public enum SSLTrustMode {
    case strict
    case disabled
}

public class FileDownloader: NSObject {
    var host = ""
    var trustMode: SSLTrustMode = .strict
    var resumeData: Data?
    var request: DownloadRequest?
    public private(set) var filePath: String!
    
    init(trustMode: SSLTrustMode = .strict, host: String = "") {
        self.trustMode = trustMode
        self.host = host
        super.init()
    }
    
    lazy var manager: SessionManager = {

        let configuration: URLSessionConfiguration

        if trustMode == .disabled {
            // ⚠️ 忽略证书必须用前台 session
            configuration = .default
        } else {
            configuration = URLSessionConfiguration.background(
                withIdentifier: "com.crim.file.download.manager.session.manager"
            )
        }

        configuration.httpAdditionalHeaders = SessionManager.defaultHTTPHeaders

        let serverTrustPolicyManager: ServerTrustPolicyManager?

        if trustMode == .disabled {
            serverTrustPolicyManager = ServerTrustPolicyManager(
                policies: [
                    host: .disableEvaluation
                ]
            )
        } else {
            serverTrustPolicyManager = nil
        }

        let manager = SessionManager(
            configuration: configuration,
            serverTrustPolicyManager: serverTrustPolicyManager
        )

        manager.startRequestsImmediately = true
        return manager
    }()
    
    func download(_ url: URLConvertible, name: String? = nil) -> DownloadRequest {
        let ext = try! url.asURL().absoluteString.split(separator: ".").last!
        var r = name
        
        if r == nil {
            r = try! url.asURL().md5 + ".\(ext)"
        }

        filePath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0] + "/\(r!)"
        if FileManager.default.fileExists(atPath: filePath) {
        }
        print("下载地址:\(filePath)")
        
        let destination: DownloadRequest.DownloadFileDestination = { [weak self] _, _ in
            return (
                        URL(fileURLWithPath: self!.filePath),
                        [.removePreviousFile, .createIntermediateDirectories]
                    )
        }
        
        if self.resumeData != nil {
            request = manager.download(resumingWith: self.resumeData!)
        } else {
            if let resumeData = request?.resumeData {
                request = manager.download(resumingWith: resumeData)
            } else {
                request = manager.download(url, to: destination)
            }
        }
        return request!
    }
}
