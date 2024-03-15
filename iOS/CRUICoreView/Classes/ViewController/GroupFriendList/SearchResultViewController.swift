import CRUICore
import RxSwift
import SnapKit

struct SearchFriendGroupResultItem {
    var ID: String
    var showName: String
    var faceURL: String?
    var isFriend: Bool = true
}

public class SearchResultViewController: UIViewController, UISearchResultsUpdating {
    
    public var didSelectedItem: ((_ ID: String) -> Void)?
    
    private lazy var tableView: UITableView = {
        let v = UITableView()
        v.register(SearchResultCell.self, forCellReuseIdentifier: SearchResultCell.className)
        v.rowHeight = UITableView.automaticDimension
        v.dataSource = self
        v.delegate = self
        v.backgroundColor = .clear
        
        if #available(iOS 15.0, *) {
            v.sectionHeaderTopPadding = 0
        }
        return v
    }()
    
    private lazy var searchResultEmptyView: UIView = {
        let v = UIView()
        let label: UILabel = {
            let v = UILabel()
            v.text = "无法找到该".innerLocalized() + _searchType.title
            return v
        }()
        v.addSubview(label)
        label.snp.makeConstraints { make in
            make.center.equalToSuperview()
        }
        v.isHidden = true
        return v
    }()
    
    private var debounceTimer: Timer?
    var dataList = [SearchFriendGroupResultItem]() {
        willSet {
            dataList = newValue
            tableView.reloadData()
        }
    }

    private let _disposebag = DisposeBag()
    private let _searchType: SearchType
    private var userInfo: FullUserInfo?
    public init(searchType: SearchType) {
        _searchType = searchType
        super.init(nibName: nil, bundle: nil)
    }
    
    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    public override func viewDidLoad() {
        super.viewDidLoad()
        edgesForExtendedLayout = [UIRectEdge.left, .right, .bottom]
        view.backgroundColor = .viewBackgroundColor
        
        initView()
        bindData()
    }
    
    private func initView() {
        view.backgroundColor = .white
        
        view.addSubview(tableView)
        tableView.snp.makeConstraints { make in
            make.edges.equalToSuperview()
        }
        
        view.addSubview(searchResultEmptyView)
        searchResultEmptyView.snp.makeConstraints { make in
            make.top.equalToSuperview()
            make.left.right.equalToSuperview()
            make.height.equalTo(60)
        }
    }
    
    private func bindData() {
    }
    
    public enum SearchType {
        /// 群组
        case group
        /// 用户
        case user
        
        var title: String {
            switch self {
            case .group:
                return "群组".innerLocalized()
            case .user:
                return "用户".innerLocalized()
            }
        }
    }
    
    private var keyword: String = ""
    
    private func checkIsFriendship() {
        DispatchQueue.global().async { [weak self] in
            let group = DispatchGroup()
            var modifiedList: [SearchFriendGroupResultItem] = []
            
            self?.dataList.forEach { item in
                group.enter()
                
                IMController.shared.checkFriendBy(userID: item.ID).subscribe { [weak self] (result: Bool) in
                    var modifiedItem = item
                    modifiedItem.isFriend = result
                    
                    let pointer = withUnsafePointer(to: modifiedItem) {
                        "\($0)"
                    }
                    
                    modifiedList.append(modifiedItem)
                    group.leave()
                }
            }
            
            group.wait()
            DispatchQueue.main.async { [weak self] in
                self?.dataList = modifiedList
            }
        }
    }
    
    public func updateSearchResults(for searchController: UISearchController) {
        let keyword = searchController.searchBar.text?.trimmingCharacters(in: .whitespacesAndNewlines).lowercased()
        guard let keyword = keyword, !keyword.isEmpty else {
            return
        }
        
        search(keyword)
    }
    
    @objc func search(_ keyword: String) {
        
        self.keyword = keyword
        
        switch _searchType {
        case .group:
            IMController.shared.getGrpListBy(id: keyword).subscribe(onNext: { [weak self] (groupID: String?) in
                let shouldHideEmptyView = groupID != nil
                let shouldHideResultView = groupID == nil
                DispatchQueue.main.async {
                    self?.searchResultEmptyView.isHidden = shouldHideEmptyView
                    self?.tableView.isHidden = shouldHideResultView
                    if groupID != nil {
                        self?.dataList = [SearchFriendGroupResultItem(ID: groupID!, showName: groupID!, faceURL: "")]
                    }
                }
            }).disposed(by: _disposebag)
        case .user:
            // 业务层有搜索数据
            if let handler = CRIMApi.queryFriendsWithCompletionHandler {
                handler([keyword], {res in
                    let shouldHideEmptyView = !res.isEmpty
                    let shouldHideResultView = res.isEmpty
                    DispatchQueue.main.async { [weak self] in
                        guard let `self` = self else { return }
                        defer {
                            self.checkIsFriendship()
                        }
                        
                        self.searchResultEmptyView.isHidden = shouldHideEmptyView
                        self.tableView.isHidden = shouldHideResultView
                        // 输入的类型
                        let isNumber = keyword.trimmingCharacters(in: .decimalDigits).length == 0
                        let isPhone = self.isPhoneNumber(keyword)
                        let isEmail = self.isEmail(keyword)
                        
                        self.dataList = res.map { elem in
                            let faceURL = elem.faceURL ?? ""
                            if isNumber {
                                if isPhone {
                                    return SearchFriendGroupResultItem(ID: elem.userID, showName: elem.phoneNumber!, faceURL: faceURL)
                                } else {
                                    return SearchFriendGroupResultItem(ID: elem.userID, showName: "ID:" + elem.userID, faceURL: faceURL)
                                }
                            } else if isEmail {
                                return SearchFriendGroupResultItem(ID: elem.userID, showName: "邮箱:" + elem.email!, faceURL: faceURL)
                            } else {
                                return SearchFriendGroupResultItem(ID: elem.userID, showName: elem.nickname!, faceURL: faceURL)
                            }
                        }
                    }
                })
            } else {
                IMController.shared.getFriendsBy(id: keyword).subscribe { [weak self] (userInfo: FullUserInfo?) in
                    self?.userInfo = userInfo
                    let uid = userInfo?.userID
                    let shouldHideEmptyView = uid != nil
                    let shouldHideResultView = uid == nil
                    let faceURL = userInfo?.publicInfo?.faceURL ?? ""
                    DispatchQueue.main.async {
                        defer {
                            self?.checkIsFriendship()
                        }
                        self?.searchResultEmptyView.isHidden = shouldHideEmptyView
                        self?.tableView.isHidden = shouldHideResultView
                        
                        if uid != nil {
                            self?.dataList = [SearchFriendGroupResultItem(ID: uid!, showName: keyword, faceURL: faceURL)]
                        }
                    }
                }.disposed(by: _disposebag)
            }
        }
    }
    
    deinit {
        debounceTimer = nil
    }
    
    // 验证邮箱
    func isEmail(_ email: String) -> Bool {
        if email.count == 0 {
            return false
        }
        let emailRegex = "[A-Z0-9a-z._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}"
        let emailTest:NSPredicate = NSPredicate(format: "SELF MATCHES %@", emailRegex)
        return emailTest.evaluate(with: email)
    }
    
    // 验证手机号
    func isPhoneNumber(_ phoneNumber: String) -> Bool {
        if phoneNumber.count == 0 {
            return false
        }
        let mobile = "^1([358][0-9]|4[579]|66|7[0135678]|9[89])[0-9]{8}$"
        let regexMobile = NSPredicate(format: "SELF MATCHES %@",mobile)
        if regexMobile.evaluate(with: phoneNumber) == true {
            return true
        } else {
            return false
        }
    }
}

extension SearchResultViewController: UITableViewDelegate, UITableViewDataSource {
    public func tableView(_: UITableView, numberOfRowsInSection _: Int) -> Int {
        return dataList.count
    }
    
    public func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: SearchResultCell.className, for: indexPath) as! SearchResultCell
        let info = dataList[indexPath.row]
        
        let placeholderName: String = _searchType == .user ? "contact_my_friend_icon" : "contact_group_setting_icon"
        let faceURL = info.faceURL
        cell.avatarImageView.setAvatar(url: faceURL, text: nil, placeHolder: placeholderName)
        
        cell.titleLabel.text = info.showName
        
        cell.addFriendBtn.isHidden = _searchType != .user || info.isFriend
        cell.addFriendBtn.isUserInteractionEnabled = false
        /*
        cell.addFriendBtnTapHandler = { [weak self] in
            guard let self = self else { return }
            
        }
        */
        
        return cell
    }
    
    public func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let info = dataList[indexPath.row]
        didSelectedItem?(info.ID)
    }
}
