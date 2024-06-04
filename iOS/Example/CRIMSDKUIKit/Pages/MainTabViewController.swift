
import CRUIIM
import CRUICore
import CRIMSDK
import RxSwift
import RxCocoa
import ProgressHUD
import Localize_Swift
import MJExtension
import GTSDK
import Toast_Swift

class MainTabViewController: UITabBarController {
    private let _disposeBag = DisposeBag()
    private var _contactNav: UINavigationController?
    private var isKickLineOff = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        var controllers: [UIViewController] = []
        
        let chatNav = UINavigationController.init(rootViewController: ChatListViewController())
        chatNav.tabBarItem.title = "消息"
        chatNav.tabBarItem.image = UIImage.init(named: "tab_home_icon_normal")?.withRenderingMode(.alwaysOriginal)
        chatNav.tabBarItem.selectedImage = UIImage.init(named: "tab_home_icon_selected")?.withRenderingMode(.alwaysOriginal)
        controllers.append(chatNav)
        IMController.shared.totalUnreadSubject.map({ (unread: Int) -> String? in
            var badge: String?
            if unread == 0 {
                badge = nil
            } else if unread > 99 {
                badge = "99+"
            } else {
                badge = String(unread)
            }
            return badge
        }).bind(to: chatNav.tabBarItem.rx.badgeValue).disposed(by: _disposeBag)
        
        IMController.shared.totalUnreadSubject.asObserver().subscribe(onNext: {
            UIApplication.shared.applicationIconBadgeNumber = $0
            GeTuiSdk.setBadge(UInt($0))
        }).disposed(by: _disposeBag)
        
        let contactVC = ContactsViewController()
        contactVC.viewModel.dataSource = self
        let contactNav = UINavigationController.init(rootViewController: contactVC)
        contactNav.tabBarItem.title = "通讯录".localized()
        contactNav.tabBarItem.image = UIImage.init(named: "tab_contact_icon_normal")?.withRenderingMode(.alwaysOriginal)
        contactNav.tabBarItem.selectedImage = UIImage.init(named: "tab_contact_icon_selected")?.withRenderingMode(.alwaysOriginal)
        controllers.append(contactNav)
        IMController.shared.contactUnreadSubject.map({ (unread: Int) -> String? in
            var badge: String?
            if unread == 0 {
                badge = nil
            } else {
                badge = String(unread)
            }
            return badge
        }).bind(to: contactNav.tabBarItem.rx.badgeValue).disposed(by: _disposeBag)
        _contactNav = contactNav
        
        let mineNav = UINavigationController.init(rootViewController: MineViewController())
        mineNav.tabBarItem.title = "我的".localized()
        mineNav.tabBarItem.image = UIImage.init(named: "tab_me_icon_normal")?.withRenderingMode(.alwaysOriginal)
        mineNav.tabBarItem.selectedImage = UIImage.init(named: "tab_me_icon_selected")?.withRenderingMode(.alwaysOriginal)
        controllers.append(mineNav)
        
        self.viewControllers = controllers
        self.tabBar.isTranslucent = false
        self.tabBar.backgroundColor = .white;
        
        self.tabBar.layer.shadowColor = UIColor.black.cgColor;
        self.tabBar.layer.shadowOpacity = 0.08;
        self.tabBar.layer.shadowOffset = CGSize.init(width: 0, height: 0);
        self.tabBar.layer.shadowRadius = 5;
        
        self.tabBar.backgroundImage = UIImage.init()
        self.tabBar.shadowImage = UIImage.init()
        
        if let uid = UserDefaults.standard.object(forKey: AccountViewModel.IMUidKey) as? String,
           let token = UserDefaults.standard.object(forKey: AccountViewModel.IMTokenKey) as? String,
           let chatToken = UserDefaults.standard.object(forKey: AccountViewModel.bussinessTokenKey) as? String {
            ProgressHUD.show()
            AccountViewModel.loginIM(uid: uid, imToken: token, chatToken: chatToken) {[weak self] (errCode, errMsg) in
                if errMsg != nil {
                    ProgressHUD.showError(errMsg)
                    self?.presentLoginController()
                } else {
                    ProgressHUD.dismiss()
                }
            }
        } else {
            DispatchQueue.main.async {
                self.presentLoginController()
            }
        }
        
        NotificationCenter.default.addObserver(self, selector: #selector(presentLoginController), name: .init("logout"), object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(kickLineOffAccount), name: .init("kickLineOff"), object: nil)
    }
    
    @objc private func kickLineOffAccount() {
        isKickLineOff = true
        presentLoginController()
    }
    
    @objc private func presentLoginController() {
        self.selectedIndex = 0
        _contactNav?.tabBarItem.badgeValue = nil
        let vc = LoginViewController()
        vc.loginBtn.rx.tap.subscribe(onNext: { [weak vc, weak self] in
            guard let controller = vc else { return }
            
            let enableToken = UserDefaults.standard.object(forKey: useTokenKey) == nil
            ? false : UserDefaults.standard.bool(forKey: useTokenKey)
            
            var appID = ""
            var appSecret = ""
            var token = ""
            
            if enableToken {
                token = UserDefaults.standard.string(forKey: sdkTokenKey) ?? defaultToken
                guard !token.isEmpty else {
                    ProgressHUD.showError("请配置token")
                    controller.showConfigViewController()
                    return
                }
            } else {
                appID = UserDefaults.standard.string(forKey: sdkAPPIDKey) ?? defaultAppID
                appSecret = UserDefaults.standard.string(forKey: sdkAPPSecretKey) ?? defaultAppSecret
                guard !appID.isEmpty, !appSecret.isEmpty else {
                    ProgressHUD.showError("请配置appID/appSecret")
                    controller.showConfigViewController()
                    return
                }
                
                appSecret = appSecret.md5()
            }
            
            guard let phone = controller.phone, !phone.isEmpty, controller.validatePhoneNumber(phone) else {
                ProgressHUD.showError("填写正确的手机号码".localized())
                return
            }
            
            let psw = ""
            let code = controller.verificationCode
            
            guard code?.isEmpty == false else {
                ProgressHUD.showError("填写正确的验证码".localized())
                return
            }
            var account: String?

            ProgressHUD.show()
            AccountViewModel.loginDemo(phone: phone,
                                       account: account,
                                       psw: code != nil ? nil : psw,
                                       verificationCode: code,
                                       areaCode: controller.areaCode) {[weak self] (errCode, errMsg) in
                if errMsg != nil {
                    switch errCode {
                    case 1005:
                        ProgressHUD.showError("用户已注册".localized())
                    case 20006, 20007:
                        ProgressHUD.showError("验证码已过期".localized())
                    case 20012:
                        ProgressHUD.showError("该账号已注销".localized())
                    default:
                        ProgressHUD.showError(errMsg)
                    }
                    self?.presentLoginController()
                } else {
                    ProgressHUD.dismiss()
                    self?.dismiss(animated: true)
                }
            }
        }).disposed(by: _disposeBag)
        
        vc.modalPresentationStyle = .fullScreen
        let nav = UINavigationController.init(rootViewController: vc)
        nav.modalPresentationStyle = .fullScreen
        
        self.present(nav, animated: false)
        
        if isKickLineOff {
            nav.view.makeToast("账号已在其他地方登录，当前设备被踢下线!".innerLocalized(), duration: 3.0, position: .bottom)
        }
        isKickLineOff = false
    }
    
}

extension MainTabViewController: ContactsDataSource {
    func getFrequentUsers() -> [CRIMUserInfo] {
        guard let uid = AccountViewModel.userID else { return [] }
        guard let usersJson = UserDefaults.standard.object(forKey: uid) as? String else { return [] }
        
        guard let users = JsonTool.fromJson(usersJson, toClass: [UserEntity].self) else {
            return []
        }
        let current = Int(Date().timeIntervalSince1970)
        let oUsers: [CRIMUserInfo] = users.compactMap { (user: UserEntity) in
            if current - user.savedTime <= 7 * 24 * 3600 {
                return user.toCRIMUserInfo()
            }
            return nil
        }
        return oUsers
    }
    
    func setFrequentUsers(_ users: [CRIMUserInfo]) {
        guard let uid = AccountViewModel.userID else { return }
        let saveTime = Int(Date().timeIntervalSince1970)
        let before = getFrequentUsers()
        var mUsers: [CRIMUserInfo] = before
        mUsers.append(contentsOf: users)
        let ret = mUsers.deduplicate(filter: {$0.userID})
        
        let uEntities: [UserEntity] = ret.compactMap { (user: CRIMUserInfo) in
            var uEntity = UserEntity.init(user: user)
            uEntity.savedTime = saveTime
            return uEntity
        }
        let json = JsonTool.toJson(fromObject: uEntities)
        UserDefaults.standard.setValue(json, forKey: uid)
        UserDefaults.standard.synchronize()
    }
    
    struct UserEntity: Codable {
        var userID: String?
        var nickname: String?
        var faceURL: String?
        var savedTime: Int = 0
        
        init(user: CRIMUserInfo) {
            self.userID = user.userID
            nickname = user.nickname
            faceURL = user.faceURL
        }
        
        func toCRIMUserInfo() -> CRIMUserInfo {
            let item = CRIMUserInfo.init()
            item.userID = userID
            item.nickname = nickname
            item.faceURL = faceURL
            return item
        }
    }
}
