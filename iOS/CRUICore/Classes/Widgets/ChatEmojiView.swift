

    
    

import UIKit

public protocol ChatEmojiViewDelegate: AnyObject {
    func emojiViewDidSelect(emojiStr: String)
}

public class ChatEmojiView: UIView {
    public weak var delegate: ChatEmojiViewDelegate?

    private let emojis: [String] = [
        "1F31B",
        "1F494",
        "1F604",
        "1F60D",
        "1F62D",
        "1F644",
        "1F929",
        "1F97A",
        "1F44B",
        "1F496",
        "1F606",
        "1F60E",
        "1F62F",
        "1F910",
        "1F970",
        "1f61c",
        "1F44D",
        "1F4A4",
        "1F608",
        "1F621",
        "1F631",
        "1F914",
        "1F971",
        "02639",
        "1F44E",
        "1F601",
        "1F609",
        "1F624",
        "1F633",
        "1F91D",
        "1F973",
        "0263A",
        "1F44F",
        "1F602",
        "1F60C",
        "1F629",
        "1F634",
        "1F923",
        "1F974",
        "0270C"
    ]

    public override var backgroundColor: UIColor? {
        didSet {
            collectionView.backgroundColor = backgroundColor
        }
    }

    private lazy var collectionView: UICollectionView = {
        let layout: UICollectionViewFlowLayout = {
            let v = UICollectionViewFlowLayout()
            v.minimumInteritemSpacing = 28
            v.minimumLineSpacing = 28
            v.scrollDirection = .vertical
            v.sectionInset = .zero
            v.itemSize = CGSize(width: 32, height: 32)
            return v
        }()
        let v = UICollectionView(frame: .zero, collectionViewLayout: layout)
        v.showsVerticalScrollIndicator = false
        v.showsHorizontalScrollIndicator = false
        v.register(EmojiCell.self, forCellWithReuseIdentifier: EmojiCell.className)
        v.contentInset = UIEdgeInsets(top: StandardUI.margin_22, left: StandardUI.margin_22, bottom: kSafeAreaBottomHeight, right: StandardUI.margin_22)
        v.backgroundColor = .white
        v.dataSource = self
        v.delegate = self
        return v
    }()

    let deleteBtn: UIButton = {
        let v = UIButton()
        v.setImage(UIImage(nameInBundle: "inputbar_pad_delete_btn_icon"), for: .normal)
        return v
    }()

    public override init(frame: CGRect) {
        super.init(frame: frame)
        addSubview(collectionView)
        collectionView.snp.makeConstraints { make in
            make.edges.equalToSuperview()
        }

        addSubview(deleteBtn)
        deleteBtn.snp.makeConstraints { make in
            make.right.equalToSuperview().inset(StandardUI.margin_22)
            make.bottom.equalToSuperview().inset(kSafeAreaBottomHeight)
            make.size.equalTo(32)
        }
        deleteBtn.isHidden = true
    }

    @available(*, unavailable)
    required init?(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    class EmojiCell: UICollectionViewCell {
        let imageView: UIImageView = {
            let v = UIImageView()
            return v
        }()
        
        let contentLabel: UILabel = {
            let v = UILabel()
            v.font = UIFont.preferredFont(forTextStyle: .title1)
            return v
        }()

        override init(frame: CGRect) {
            super.init(frame: frame)
            contentView.addSubview(contentLabel)
            contentLabel.snp.makeConstraints { make in
                make.center.equalToSuperview()
            }
        }

        @available(*, unavailable)
        required init?(coder _: NSCoder) {
            fatalError("init(coder:) has not been implemented")
        }
    }
}

extension ChatEmojiView: UICollectionViewDataSource, UICollectionViewDelegate {
    public func numberOfSections(in _: UICollectionView) -> Int {
        return 1
    }

    public func collectionView(_: UICollectionView, numberOfItemsInSection _: Int) -> Int {
        return emojis.count
    }

    public func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: EmojiCell.className, for: indexPath) as! EmojiCell
        let key = emojis[indexPath.row]
        if let emojiName = characterStringFrom(hexString: key) {
//            cell.imageView.image = UIImage(nameInEmoji: emojiName)
            cell.contentLabel.text = emojiName
        }
        return cell
    }

    public func collectionView(_: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let emojiName = emojis[indexPath.row]
        if let emojiName = characterStringFrom(hexString: emojiName) {
//            cell.imageView.image = UIImage(nameInEmoji: emojiName)
//            cell.contentLabel.text = emojiName
            delegate?.emojiViewDidSelect(emojiStr: emojiName)
        }
    }
}

extension ChatEmojiView {
    public func characterStringFrom(hexString: String) -> String? {
        // 将16进制字符串转换为整数
        if let unicodeValue = UInt32(hexString, radix: 16) {
            // 使用UnicodeScalar来创建字符
            let character = Character(UnicodeScalar(unicodeValue)!)
            let characterString = String(character)
            
            return characterString
        }
        return nil
    }
}
