
import Foundation

public class EmojiHelper {
    static let ReplaceableStringKey: NSAttributedString.Key = .init(rawValue: "ReplaceableStringKey")

    public static let shared: EmojiHelper = {
        var emojis: [EmojiHelper.Emoji] = []
        for key in EmojiHelper.emojiMap.keys {
            if let imageName = EmojiHelper.emojiMap[key] {
                let emoji = EmojiHelper.Emoji(imageName: imageName, imageDesc: key)
                emojis.append(emoji)
            }
        }
        let v = EmojiHelper(emojis: emojis)
        return v
    }()

    static let emojiMap: [String: String] = [
        "[天使]": "ic_face_01",
        "[生气]": "ic_face_02",
        "[中毒-1]": "ic_face_03",
        "[中毒]": "ic_face_04",
        "[迷茫]": "ic_face_05",
        "[酷-1]": "ic_face_06",
        "[酷]": "ic_face_07",
        "[哭-1]": "ic_face_08",
        "[哭]": "ic_face_09",
        "[魔鬼]": "ic_face_10",
        "[头晕]": "ic_face_11",
        "[面无表情]": "ic_face_12",
        "[懵逼]": "ic_face_13",
        "[开心-2]": "ic_face_14",
        "[开心-1]": "ic_face_15",
        "[开心]": "ic_face_16",
        "[热恋]": "ic_face_17",
        "[受伤]": "ic_face_18",
        "[哭哭]": "ic_face_19",
        "[亲吻-1]": "ic_face_20",
        "[亲吻-2]": "ic_face_21",
        "[亲吻]": "ic_face_22",
        "[口罩]": "ic_face_23",
        "[静音]": "ic_face_24",
        "[面无表情-1]": "ic_face_25",
        "[难过-1]": "ic_face_26",
        "[难过]": "ic_face_27",
        "[害怕-1]": "ic_face_28",
        "[害怕]": "ic_face_29",
        "[闭嘴]": "ic_face_30",
        "[震惊-1]": "ic_face_31",
        "[生病]": "ic_face_32",
        "[睡觉]": "ic_face_33",
        "[笑-1]": "ic_face_34",
        "[笑]": "ic_face_35",
        "[微笑-1]": "ic_face_36",
        "[眼红]": "ic_face_37",
        "[奸笑]": "ic_face_38",
        "[震惊]": "ic_face_39",
        "[流汗]": "ic_face_40",
        "[思考]": "ic_face_41",
        "[疲惫]": "ic_face_42",
        "[吐舌-2]": "ic_face_43",
        "[吐舌-1]": "ic_face_44",
        "[吐舌]": "ic_face_45",
        "[斜眼]": "ic_face_46",
        "[呕吐-1]": "ic_face_47",
        "[暴怒]": "ic_face_48",
        "[眨眼]": "ic_face_49",
        "[僵尸]": "ic_face_50",
    ]

    let emojis: [Emoji]

    init(emojis: [Emoji]) {
        self.emojis = emojis
    }

    /// 在属性文本中做可替换的文本标记
    public func markReplaceableRange(inAttributedString: NSAttributedString, withString: String) -> NSAttributedString {
        let mutable = NSMutableAttributedString(attributedString: inAttributedString)
        mutable.addAttribute(EmojiHelper.ReplaceableStringKey, value: withString, range: NSRange(location: 0, length: inAttributedString.length))
        return mutable
    }

    public func replaceTextWithEmojiIn(attributedString: NSAttributedString, font: UIFont = UIFont.systemFont(ofSize: 14)) -> NSMutableAttributedString {
        let attributedString = NSMutableAttributedString(attributedString: attributedString)
        if attributedString.length <= 0 {
            return attributedString
        }

        let matchedResults = getMatchedEmojiResultIn(string: attributedString.string)
        var offset = 0
        for result in matchedResults {
            let emojiHeight = font.lineHeight
            let attachment = NSTextAttachment()
            attachment.image = result.emojiImage
            attachment.bounds = CGRect(x: 0, y: font.descender, width: emojiHeight, height: emojiHeight)
            let emojiAttString = NSMutableAttributedString(attributedString: NSAttributedString(attachment: attachment))
            let marked = markReplaceableRange(inAttributedString: emojiAttString, withString: result.emojiDesc)

            let actualRange = NSRange(location: result.range.location - offset, length: result.emojiDesc.length)
            attributedString.replaceCharacters(in: actualRange, with: marked)
            offset += result.emojiDesc.length - marked.length
        }
        return attributedString
    }

    public func getPlainTextIn(attributedString: NSAttributedString, atRange: NSRange) -> NSString {
        let string: NSString = attributedString.string as NSString
        let result = NSMutableString()
        attributedString.enumerateAttribute(EmojiHelper.ReplaceableStringKey, in: atRange, options: .longestEffectiveRangeNotRequired) { (mark: Any?, range: NSRange, _: UnsafeMutablePointer<ObjCBool>) in
            if let mark = mark as? String {
                result.append(mark)
            } else {
                result.append(string.substring(with: range))
            }
        }

        return result
    }

    private func getMatchedEmojiResultIn(string: String) -> [EmojiMatchedResult] {
        if string.isEmpty {
            return []
        }

        let string = NSString(string: string)

        do {
            let regex: NSRegularExpression = try NSRegularExpression(pattern: "\\[.+?\\]", options: .caseInsensitive)
            let results: [NSTextCheckingResult] = regex.matches(in: string as String, options: .reportCompletion, range: NSRange(location: 0, length: string.length))

            var emojiResults: [EmojiMatchedResult] = []
            for result in results {
                let emojiName = string.substring(with: result.range)
                if let emoji = getEmojiWith(name: emojiName) {
                    let img = getImageWith(name: emoji.imageName)
                    let res = EmojiMatchedResult(range: result.range, emojiImage: img, emojiDesc: emoji.imageDesc)
                    emojiResults.append(res)
                }
            }
            return emojiResults
        } catch {
            print("获取匹配Emoji结果失败:", error.localizedDescription)
        }
        return []
    }

    private func getEmojiWith(name: String) -> Emoji? {
        for emoji in emojis {
            if emoji.imageDesc == name {
                return emoji
            }
        }
        return nil
    }

    private func getImageWith(name: String) -> UIImage? {
        let image = UIImage(nameInEmoji: name)
        return image
    }

    struct Emoji {
        let imageName: String
        let imageDesc: String
    }

    struct EmojiMatchedResult {
        let range: NSRange
        let emojiImage: UIImage?
        let emojiDesc: String
    }
}

extension String {
    public var length: Int {
        let string = NSString(string: self)
        return string.length
    }
}
