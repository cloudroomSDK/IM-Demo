
import CRUICore
import Localize_Swift
import RxSwift
import GTSDK
import Bugly
import CRUICalling
import IQKeyboardManagerSwift

let kGtAppId = "AKO73DOTtmAIsCcEnTWS58"
let kGtAppKey = "f4bEG4a8dE9fdCdoZHfw88"
let kGtAppSecret = "VaQHJiYBuDATVC2zn9kRr1"

// 默认使用的IP或者域名
let defaultHost = "demo.cloudroom.com" // 填入host

// 设置页用到的默认IP或域名，在设置页保存以后，defaultHost将失效
let defaultIP = "demo.cloudroom.com"
let defaultDomain = "demo.cloudroom.com"

let bussinessPort = ":8018"
//let adminPort = ":8019"

let httpsBussinessPort = ":8218"

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate, UNUserNotificationCenterDelegate, GeTuiSdkDelegate {
    
    var backgroundTaskIdentifier: UIBackgroundTaskIdentifier?
    
    var window: UIWindow?
    private let _disposeBag = DisposeBag();
    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        
        IQKeyboardManager.shared.shouldResignOnTouchOutside = true
        
        UINavigationBar.appearance().tintColor = .c0C1C33
        
        let protocolType = UserDefaults.standard.integer(forKey: protocolKey)
        let httpScheme = protocolType == 0 ? "http://" : "https://"
        
        let severAddress = UserDefaults.standard.string(forKey: severAddressKey) ?? defaultHost
            
        // 设置登录注册等 - AccountViewModel
        UserDefaults.standard.setValue(httpScheme + severAddress + (protocolType == 0 ? bussinessPort : httpsBussinessPort), forKey: bussinessSeverAddrKey)
        
        GeTuiSdk.runBackgroundEnable(false)
        GeTuiSdk.start(withAppId: kGtAppId, appKey: kGtAppKey, appSecret: kGtAppSecret, delegate: self)
        GeTuiSdk.registerRemoteNotification([.alert, .badge, .sound])

        Bugly.start(withAppId: "326fea6cd9")
        debugPrint("HomeDirectory: \(NSHomeDirectory())")
    
        return true
    }
    
    class func setupIMSDK(sdkSvr: String) {
        let protocolType = UserDefaults.standard.integer(forKey: protocolKey)
        // 设置对象存储
        let sdkObjectStorage = "minio"
        
        // 初始化SDK
        IMController.shared.setup(sdkAPIAdrr: sdkSvr,
                                  skipVerifyCert: protocolType == 3,
                                  sdkOS: sdkObjectStorage) {
            IMController.shared.currentUserRelay.accept(nil)
            GeTuiSdk.unbindAlias(IMController.shared.userID, andSequenceNum: "crim", andIsSelf: true)
            AccountViewModel.saveUser(uid: nil, imToken: nil, chatToken: nil)
            NotificationCenter.default.post(name: .init("kickLineOff"), object: nil)
        }
        
        CRIMSessionManagerWrapper.shared.updateCertificateValidation()
    }
    
    class func setupVideoSDK(sdkSvr: String, sdkToken: String? = nil, sdkAppId: String? = nil, sdkSecret: String? = nil) {
        CRUICalling.CallingManager.manager.setupVideoSDK(sdkServer: sdkSvr)
        CRUICalling.CallingManager.manager.setup(appID: sdkAppId, appSecret: sdkSecret, token: sdkToken)
    }
    
    func applicationWillResignActive(_ application: UIApplication) {
    }
    
    func applicationDidEnterBackground(_ application: UIApplication) {
//        application.applicationIconBadgeNumber = 0
        self.backgroundTaskIdentifier = UIApplication.shared.beginBackgroundTask(withName: "taskname", expirationHandler: {
            
            if (self.backgroundTaskIdentifier != .invalid) {
                UIApplication.shared.endBackgroundTask(self.backgroundTaskIdentifier!);
                self.backgroundTaskIdentifier = .invalid;
            }
        });
    }
    
    func applicationWillEnterForeground(_ application: UIApplication) {
        //application.applicationIconBadgeNumber = 0
        UIApplication.shared.endBackgroundTask(self.backgroundTaskIdentifier!);
    }
    
    func applicationDidBecomeActive(_ application: UIApplication) {
        
    }
    
    func applicationWillTerminate(_ application: UIApplication) {
        
    }
    func application(_ application: UIApplication, didRegisterForRemoteNotificationsWithDeviceToken deviceToken: Data) {
        
    }
    
    func application(_ application: UIApplication, didFailToRegisterForRemoteNotificationsWithError error: Error) {
        print("did Fail To Register For Remote Notifications With Error: %@", error)
    }
    
    // MARK: - GeTuiSdkDelegate
    /// [ GTSDK回调 ] SDK启动成功返回cid
    func geTuiSdkDidRegisterClient(_ clientId: String) {
        let msg = "[ TestDemo ] \(#function):\(clientId)"
        //IMController.shared.updateAPNsToken(token: clientId)
        print(msg)
    }
    
    /// [ GTSDK回调 ] SDK运行状态通知
//    func geTuiSDkDidNotifySdkState(_ aStatus: SdkStatus) {
//    }
    
    /// [ GTSDK回调 ] SDK错误反馈
    func geTuiSdkDidOccurError(_ error: Error) {
        let msg = "[ TestDemo ] \(#function) \(error.localizedDescription)"
        print(msg)
    }
    
    //MARK: - 通知回调
    func getuiSdkGrantAuthorization(_ granted: Bool, error: Error?) {
        let msg = "[ TestDemo ] \(#function) \(granted ? "Granted":"NO Granted")"
        print(msg)
    }
    
    /// [ 系统回调 ] iOS 10及以上  APNs通知将要显示时触发
    @available(iOS 10.0, *)
    func geTuiSdkNotificationCenter(_ center: UNUserNotificationCenter, willPresent notification: UNNotification, completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        completionHandler([.badge, .sound, .alert])
    }
    
    @available(iOS 10.0, *)
    func geTuiSdkDidReceiveNotification(_ userInfo: [AnyHashable : Any], notificationCenter center: UNUserNotificationCenter?, response: UNNotificationResponse?, fetchCompletionHandler completionHandler: ((UIBackgroundFetchResult) -> Void)? = nil) {
        completionHandler?(.noData)
    }
    
    func pushLocalNotification(_ title: String, _ userInfo:[AnyHashable:Any]) {
        if #available(iOS 10.0, *) {
            let content = UNMutableNotificationContent()
            content.title = title
            content.body = title
            let req = UNNotificationRequest.init(identifier: "id1", content: content, trigger: nil)
            
            UNUserNotificationCenter.current().add(req) { _ in
                print("addNotificationRequest added")
            }
        }
    }
    
    func geTuiSdkDidReceiveSlience(_ userInfo: [AnyHashable : Any], fromGetui: Bool, offLine: Bool, appId: String?, taskId: String?, msgId: String?, fetchCompletionHandler completionHandler: ((UIBackgroundFetchResult) -> Void)? = nil) {
        let msg = "[ TestDemo ] \(#function) fromGetui:\(fromGetui ? "个推消息" : "APNs消息") appId:\(appId ?? "") offLine:\(offLine ? "离线" : "在线") taskId:\(taskId ?? "") msgId:\(msgId ?? "") userInfo:\(userInfo)"
        //本地通知UserInfo参数
        var dic: [AnyHashable : Any] = [:]
        if fromGetui {
            //个推在线透传
            dic = ["_gmid_":"\(String(describing: taskId)):\(String(describing: msgId))"]
        } else {
            //APNs静默通知
            dic = userInfo;
        }
        if fromGetui && !offLine {
            //个推通道+在线，发起本地通知
//            pushLocalNotification(userInfo["payload"] as! String, dic)
        }
        print(msg)
    }
    
    @available(iOS 10.0, *)
    func geTuiSdkNotificationCenter(_ center: UNUserNotificationCenter, openSettingsFor notification: UNNotification?) {
        // [ 参考代码，开发者注意根据实际需求自行修改 ]
    }
    
    //MARK: - 发送上行消息
    
    /// [ GTSDK回调 ] SDK收到sendMsg消息回调
    func geTuiSdkDidSendMessage(_ messageId: String, result: Int32) {
        let msg = "[ TestDemo ] \(#function) \(String(describing: messageId)), result=\(result)"
        print(msg)
    }
    //MARK: - 别名设置
    func geTuiSdkDidAliasAction(_ action: String, result isSuccess: Bool, sequenceNum aSn: String, error aError: Error?) {
        /*
         参数说明
         isSuccess: YES: 操作成功 NO: 操作失败
         aError.code:
         30001：绑定别名失败，频率过快，两次调用的间隔需大于 5s
         30002：绑定别名失败，参数错误
         30003：绑定别名请求被过滤
         30004：绑定别名失败，未知异常
         30005：绑定别名时，cid 未获取到
         30006：绑定别名时，发生网络错误
         30007：别名无效
         30008：sn 无效 */
        
        var msg = ""
//        if action == kGtResponseBindType {
//            msg = "[ TestDemo ] \(#function) bind alias result sn = \(String(describing: aSn)), error = \(String(describing: aError))"
//        }
//        if action == kGtResponseUnBindType {
//            msg = "[ TestDemo ] \(#function) unbind alias result sn = \(String(describing: aSn)), error = \(String(describing: aError))"
//        }
        print(msg)
    }
    
    
    //MARK: - 标签设置
    func geTuiSdkDidSetTagsAction(_ sequenceNum: String, result isSuccess: Bool, error aError: Error?) {
        /*
         参数说明
         sequenceNum: 请求的序列码
         isSuccess: 操作成功 YES, 操作失败 NO
         aError.code:
         20001：tag 数量过大（单次设置的 tag 数量不超过 100)
         20002：调用次数超限（默认一天只能成功设置一次）
         20003：标签重复
         20004：服务初始化失败
         20005：setTag 异常
         20006：tag 为空
         20007：sn 为空
         20008：离线，还未登陆成功
         20009：该 appid 已经在黑名单列表（请联系技术支持处理）
         20010：已存 tag 数目超限
         20011：tag 内容格式不正确
         */
        let msg = "[ TestDemo ] \(#function)  sequenceNum:\(sequenceNum) isSuccess:\(isSuccess) error: \(String(describing: aError))"
        
        print(msg)
    }
}

