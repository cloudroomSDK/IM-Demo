//
//  SendConfirmationAlertView.swift
//  CRUICore
//
//  Created by YunWu01 on 2024/4/18.
//

import UIKit

public class SendConfirmationAlertView: UIView {
    private let titleLabel: UILabel = {
        let v = UILabel()
        v.textColor = .c353535
        
        return v
    }()
    
    lazy var collectionView: UICollectionView = {
        let layout = UICollectionViewFlowLayout()
        layout.scrollDirection = .horizontal
        let v = UICollectionView(frame: .zero, collectionViewLayout: layout)
        v.register(SelectedImageVerCollectionViewCell.self, forCellWithReuseIdentifier: SelectedImageVerCollectionViewCell.className)
        v.register(SelectedImageHorCollectionViewCell.self, forCellWithReuseIdentifier: SelectedImageHorCollectionViewCell.className)
        v.delegate = self
        v.dataSource = self
        v.backgroundColor = .clear
        v.showsHorizontalScrollIndicator = false
        v.showsVerticalScrollIndicator = false
        
        return v
    }()
    
    private let messageLabel: UILabel = {
        let v = UILabel()
        v.textColor = .c666666
        v.numberOfLines = 2
        
        return v
    }()
    
    private let textField: UITextField = {
        let textField = UITextField()
        textField.borderStyle = .none
        textField.layer.cornerRadius = 6
        textField.placeholder = " " + "给朋友留言".innerLocalized()
        textField.backgroundColor = .cF1F1F1
        return textField
    }()
    
    private let separatorView: UIView = {
        let view = UIView()
        view.backgroundColor = .cDCDCDC
        return view
    }()
    
    private let stackView: UIStackView = {
        let stackView = UIStackView()
        stackView.axis = .vertical
        stackView.spacing = 10
        return stackView
    }()
    
    private var dataSource = [ContactInfo]()
    var collectionViewheightConstraint: NSLayoutConstraint?
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        setupUI()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        setupUI()
    }
    
    private func setupUI() {
        translatesAutoresizingMaskIntoConstraints = false
        widthAnchor.constraint(equalToConstant: kScreenWidth - 70*2).isActive = true
        
        // 添加控件到自定义视图中
        addSubview(stackView)
        
        stackView.translatesAutoresizingMaskIntoConstraints = false
        
        NSLayoutConstraint.activate([
            stackView.topAnchor.constraint(equalTo: topAnchor, constant: 20),
            stackView.leadingAnchor.constraint(equalTo: leadingAnchor, constant: 15),
            stackView.trailingAnchor.constraint(equalTo: trailingAnchor, constant: -15),
            stackView.bottomAnchor.constraint(equalTo: bottomAnchor, constant: -20)
        ])
        
        // 添加控件到垂直堆栈视图中
        stackView.addArrangedSubview(titleLabel)
        stackView.addArrangedSubview(collectionView)
        stackView.addArrangedSubview(separatorView)
        stackView.addArrangedSubview(messageLabel)
        stackView.addArrangedSubview(textField)
        
        textField.translatesAutoresizingMaskIntoConstraints = false
        textField.heightAnchor.constraint(equalToConstant: 44).isActive = true
        
        // 设置分割线的高度约束
        separatorView.translatesAutoresizingMaskIntoConstraints = false
        separatorView.heightAnchor.constraint(equalToConstant: 1.0/UIScreen.main.scale).isActive = true
        
        collectionView.translatesAutoresizingMaskIntoConstraints = false
        collectionViewheightConstraint = collectionView.heightAnchor.constraint(equalToConstant: 50)
        collectionViewheightConstraint?.priority = UILayoutPriority(999)
        collectionViewheightConstraint?.isActive = true
    }
    
    public func reloadCollectionView(contacts: [ContactInfo], msgAbstract: String) {
        if contacts.count > 1 {
            collectionViewheightConstraint?.constant = 70
            setNeedsLayout()
            layoutIfNeeded()
        }
        
        titleLabel.text = contacts.count > 1 ? "分别发送给:".innerLocalized() : "发送给:".innerLocalized()
        messageLabel.text = msgAbstract
        
        dataSource.removeAll()
        dataSource.append(contentsOf: contacts)
        collectionView.reloadData()
    }
    
    class SelectedImageVerCollectionViewCell: UICollectionViewCell {
        
        let avatarView = AvatarView()
        
        let nameLabel: UILabel = {
            let v = UILabel()
            v.font = .f12
            v.textColor = .c3D3D3D
            
            return v
        }()
        
        lazy var stackView: UIStackView = {
            let v = UIStackView()
            v.axis = .vertical
            v.spacing = 4
            v.alignment = .center
            
            return v
        }()

        override init(frame: CGRect) {
            super.init(frame: frame)
            
            stackView.addArrangedSubview(avatarView)
            stackView.addArrangedSubview(nameLabel)
            
            contentView.addSubview(stackView)
            stackView.snp.makeConstraints { make in
                make.edges.equalToSuperview()
            }
        }

        @available(*, unavailable)
        required init?(coder _: NSCoder) {
            fatalError("init(coder:) has not been implemented")
        }
        
        override func prepareForReuse() {
            super.prepareForReuse()
            nameLabel.text = nil
            avatarView.reset()
        }
    }
    
    class SelectedImageHorCollectionViewCell: SelectedImageVerCollectionViewCell {
        override init(frame: CGRect) {
            super.init(frame: frame)
            
            self.stackView.axis = .horizontal
        }
    }
}

extension SendConfirmationAlertView: UICollectionViewDelegateFlowLayout {
    public func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, insetForSectionAt section: Int) -> UIEdgeInsets {
        if collectionView.numberOfItems(inSection: section) == 1 {
            let flowLayout = collectionViewLayout as! UICollectionViewFlowLayout

            return UIEdgeInsets(top: 0, left: 8, bottom: 0, right: collectionView.frame.width - 30)
        }

        return UIEdgeInsets(top: 0, left: 8, bottom: 0, right: 8)
    }
    
    public func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        return collectionView.numberOfItems(inSection: 0) == 1 ? CGSize(width: 150, height: 50) : CGSize(width: 42, height: 70)
    }
}

extension SendConfirmationAlertView: UICollectionViewDataSource {
    public func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return dataSource.count
    }
    
    public func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let item = dataSource[indexPath.item]
        
        if collectionView.numberOfItems(inSection: 0) == 1 {
            let cell = collectionView.dequeueReusableCell(withReuseIdentifier: SelectedImageHorCollectionViewCell.className, for: indexPath) as! SelectedImageHorCollectionViewCell
            cell.avatarView.setAvatar(url: item.faceURL, text: nil, placeHolder: "contact_my_friend_icon")
            cell.nameLabel.text = item.name
            
            return cell
        }
        
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: SelectedImageVerCollectionViewCell.className, for: indexPath) as! SelectedImageVerCollectionViewCell
        cell.avatarView.setAvatar(url: item.faceURL, text: nil, placeHolder: "contact_my_friend_icon")
        cell.nameLabel.text = item.name
        
        return cell
    }
}
