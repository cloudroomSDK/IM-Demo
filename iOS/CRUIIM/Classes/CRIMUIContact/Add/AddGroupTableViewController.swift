//
//  AddGroupTableViewController.swift
//  Pods
//
//  Created by YunWu01 on 2023/12/1.
//

import UIKit
import CRUICore
import CRUICoreView
import ProgressHUD

class AddGroupTableViewController: UITableViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        replaceSystemBackBarButtonItem()
        configureTableView()
    }

    private func configureTableView() {
        tableView.rowHeight = 74
        if #available(iOS 15.0, *) {
            tableView.sectionHeaderTopPadding = 0
        }
        tableView.sectionHeaderHeight = 12
        tableView.backgroundColor = .viewBackgroundColor
        tableView.separatorColor = .viewBackgroundColor
        tableView.separatorInset = UIEdgeInsets(top: 0, left: 71, bottom: 0, right: 22)
        tableView.register(AddEntranceTableViewCell.self, forCellReuseIdentifier: AddEntranceTableViewCell.className)
    }
    
    private let rowTypes: [EntranceType] = [
        .scanQrcode,
        .searchGroup,
    ]

    override open func tableView(_: UITableView, numberOfRowsInSection section: Int) -> Int {
        rowTypes.count
    }

    override open func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: AddEntranceTableViewCell.className) as! AddEntranceTableViewCell
        let item = rowTypes[indexPath.row]
        cell.avatarImageView.image = item.iconImage
        cell.titleLabel.text = item.title
        cell.subtitleLabel.text = item.subtitle
        return cell
    }

    override open func tableView(_: UITableView, didSelectRowAt indexPath: IndexPath) {
        let item = rowTypes[indexPath.row]
        switch item {
        case .searchGroup:
            let vc = SearchGroupViewController()
            navigationController?.pushViewController(vc, animated: true)
            vc.didSelectedItem = { [weak self] id in
                let vc = GroupDetailViewController(groupId: id, joinSource: .search)
                self?.navigationController?.pushViewController(vc, animated: true)
            }
        case .scanQrcode:
            let vc = ScanViewController()
            vc.scanDidComplete = { [weak self] (result: String) in
                if result.contains(IMController.addFriendPrefix) {
                    let uid = result.replacingOccurrences(of: IMController.addFriendPrefix, with: "")
                    let vc = UserDetailTableViewController(userId: uid, groupId: nil)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: true)
                } else if result.contains(IMController.joinGrpPrefix) {
                    let groupID = result.replacingOccurrences(of: IMController.joinGrpPrefix, with: "")
                    let vc = GroupDetailViewController(groupId: groupID, joinSource: .QRCode)
                    vc.hidesBottomBarWhenPushed = true
                    self?.navigationController?.pushViewController(vc, animated: true)
                    self?.dismiss(animated: true)
                } else {
                    ProgressHUD.error(result)
                }
            }
            vc.modalPresentationStyle = .fullScreen
            present(vc, animated: true)
        }
    }

    struct SectionModel {
        let name: String
        let items: [EntranceType]
    }

    enum EntranceType {
        case scanQrcode
        case searchGroup

        var iconImage: UIImage? {
            switch self {
            case .scanQrcode:
                return UIImage(nameInBundle: "add_scan_group_icon")
            case .searchGroup:
                return UIImage(nameInBundle: "add_search_group_icon")
            }
        }

        var title: String {
            switch self {
            case .scanQrcode:
                return "扫码加入".innerLocalized()
            case .searchGroup:
                return "群ID号加入".innerLocalized()
            }
        }

        var subtitle: String {
            switch self {
            case .scanQrcode:
                return "扫描二维码名片加入群聊".innerLocalized()
            case .searchGroup:
                return "向管理员或群成员询问编号".innerLocalized()
            }
        }
    }
}
