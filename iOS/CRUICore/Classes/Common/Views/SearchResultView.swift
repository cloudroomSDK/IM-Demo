
import UIKit
import RxSwift
import SnapKit

public class SearchResultCell: UITableViewCell {
    
    public let avatarImageView: AvatarView = {
        let v = AvatarView()
        return v
    }()
    
    public let titleLabel: UILabel = {
        let v = UILabel()
        v.font = .f17
        v.textColor = .c0089FF
        
        return v
    }()
    
    public let addFriendBtn: UIButton = {
        let v = UIButton(type: .system)
        v.setTitle("  加好友  ".innerLocalized(), for: .normal)
        v.tintColor = .white
        v.backgroundColor = UIColor.c5AD439
        v.titleLabel?.font = UIFont.f14
        v.layer.cornerRadius = 6
        v.isHidden = true

        return v
    }()
    
    public var addFriendBtnTapHandler: (() -> Void)?

    override init(style: UITableViewCell.CellStyle, reuseIdentifier: String?) {
        super.init(style: style, reuseIdentifier: reuseIdentifier)
        
        selectionStyle = .none
        backgroundColor = .cellBackgroundColor
        
        contentView.addSubview(avatarImageView)
        avatarImageView.snp.makeConstraints { make in
            make.left.equalToSuperview().offset(StandardUI.margin_22)
            make.centerY.equalToSuperview()
            make.top.equalToSuperview().offset(10)
        }

        contentView.addSubview(titleLabel)
        titleLabel.snp.makeConstraints { make in
            make.left.equalTo(avatarImageView.snp.right).offset(6)
            make.centerY.equalToSuperview()
        }
        
        contentView.addSubview(addFriendBtn)
        addFriendBtn.snp.makeConstraints { make in
            make.left.greaterThanOrEqualTo(titleLabel.snp.right).offset(10)
            make.centerY.equalToSuperview()
            make.right.equalToSuperview().offset(-18)
        }
        
        addFriendBtn.addTarget(self, action: #selector(addFriendBtnTapAction), for: .touchUpInside)
    }
    
    @objc
    private func addFriendBtnTapAction() {
        addFriendBtnTapHandler?()
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    public override func prepareForReuse() {
        super.prepareForReuse()
        addFriendBtn.isHidden = true
    }
}
