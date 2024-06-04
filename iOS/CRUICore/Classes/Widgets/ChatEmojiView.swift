

    
    

import UIKit

public protocol ChatEmojiViewDelegate: AnyObject {
    func emojiViewDidSelect(emojiStr: String)
}

public class ChatEmojiView: UIView {
    public weak var delegate: ChatEmojiViewDelegate?

    private let emojis: [String] = [
        "[天使]",
        "[生气]",
        "[中毒-1]",
        "[中毒]",
        "[迷茫]",
        "[酷-1]",
        "[酷]",
        "[哭-1]",
        "[哭]",
        "[魔鬼]",
        "[头晕]",
        "[面无表情]",
        "[懵逼]",
        "[开心-2]",
        "[开心-1]",
        "[开心]",
        "[热恋]",
        "[受伤]",
        "[哭哭]",
        "[亲吻-1]",
        "[亲吻-2]",
        "[亲吻]",
        "[口罩]",
        "[静音]",
        "[面无表情-1]",
        "[难过-1]",
        "[难过]",
        "[害怕-1]",
        "[害怕]",
        "[闭嘴]",
        "[震惊-1]",
        "[生病]",
        "[睡觉]",
        "[笑-1]",
        "[笑]",
        "[微笑-1]",
        "[眼红]",
        "[奸笑]",
        "[震惊]",
        "[流汗]",
        "[思考]",
        "[疲惫]",
        "[吐舌-2]",
        "[吐舌-1]",
        "[吐舌]",
        "[斜眼]",
        "[呕吐-1]",
        "[暴怒]",
        "[眨眼]",
        "[僵尸]",
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

        override init(frame: CGRect) {
            super.init(frame: frame)
            contentView.addSubview(imageView)
            imageView.snp.makeConstraints { make in
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
        if let emojiName = EmojiHelper.emojiMap[key] {
            cell.imageView.image = UIImage(nameInEmoji: emojiName)
        }
        return cell
    }

    public func collectionView(_: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let emojiName = emojis[indexPath.row]
        delegate?.emojiViewDidSelect(emojiStr: emojiName)
    }
}
