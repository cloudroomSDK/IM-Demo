
import Foundation

public struct MessageHelper {
    /// 获取消息摘要
    public static func getAbstructOf(conversation: ConversationInfo) -> NSAttributedString {
        var abstruct = ""
        let message = conversation.latestMsg
        let isSingleChat = conversation.conversationType == .c1v1
        let unreadCount = conversation.unreadCount
        let status = conversation.recvMsgOpt
        
        if status != .receive, unreadCount > 0 {
            let unreadDesc: String
            if unreadCount > 99 {
                unreadDesc = "99+"
            } else {
                unreadDesc = "\(unreadCount)"
            }
            abstruct = "[\(unreadDesc)]条"
        }
        let defaultAttr: [NSAttributedString.Key: Any] = [
            NSAttributedString.Key.font: UIFont.f14,
            NSAttributedString.Key.foregroundColor: UIColor.c8E9AB0,
        ]
        guard let message = message else {
            return NSAttributedString(string: abstruct, attributes: defaultAttr)
        }
        var tmpAttr: NSAttributedString?
        switch message.contentType {
        case .text:
            abstruct += message.textElem?.content ?? ""
        case .image:
            abstruct += "[图片]"
        case .audio:
            abstruct += "[语音]"
        case .video:
            abstruct += "[视频]"
        case .file:
            abstruct += "[文件]"
        case .at:
            // TODO:
            if let atElem = message.atTextElem {
                if atElem.isAtSelf {
                    let textColor = message.isRead ? UIColor.c8E9AB0 : UIColor.c0089FF
                    tmpAttr = NSAttributedString(string: "[有人@我]", attributes: [
                        NSAttributedString.Key.font: UIFont.f14,
                        NSAttributedString.Key.foregroundColor: textColor,
                    ])
                } else {
                    let names = atElem.atUsersInfo?.compactMap { $0.groupNickname } ?? []
                    var desc: String = ""
                    for name in names {
                        desc.append(name)
                        desc.append(" ")
                    }
                    tmpAttr = NSAttributedString(string: "@\(desc)", attributes: [
                        NSAttributedString.Key.font: UIFont.f14,
                        NSAttributedString.Key.foregroundColor: UIColor.c8E9AB0,
                    ])
                }
            }
        case .card:
            abstruct += "[名片]"
        case .location:
            abstruct += "[位置]"
        case .face:
            abstruct += "[自定义表情]"
        case .quote:
            abstruct += message.quoteElem?.text ?? ""
        case .privateMessage:
            abstruct += conversation.isPrivateChat ? "开启了阅后即焚" : "关闭了阅后即焚"
        case .merge:
            abstruct = "[转发]"
        case .custom:
            abstruct += customMessageAbstruct(message: message)
        default:
            tmpAttr = getSystemNotificationOf(message: message, isSingleChat: isSingleChat)
        }
        var ret = NSMutableAttributedString(string: abstruct, attributes: defaultAttr)
        if let tmpAttr = tmpAttr {
            ret.append(tmpAttr)
        }
        return ret
    }
    
    public static func getSummary(by message: MessageInfo) -> String {
   
        var abstruct = ""
        
        switch message.contentType {
        case .text:
            abstruct += message.textElem?.content ?? ""
        case .image:
            abstruct += "[图片]"
        case .audio:
            abstruct += "[语音]"
        case .video:
            abstruct += "[视频]"
        case .file:
            abstruct += "[文件]"
        case .at:
            // TODO:
            if let atElem = message.atTextElem {
                
                if atElem.isAtSelf {
                    abstruct += "[有人@我]"
                } else {
                    let names = atElem.atUsersInfo?.compactMap { $0.groupNickname } ?? []
                    var desc: String = ""
                    for name in names {
                        desc.append(name)
                        desc.append(" ")
                    }
                    abstruct += desc
                }
            }
        case .card:
            abstruct += "[名片]"
        case .location:
            abstruct += "[位置]"
        case .face:
            abstruct += "[自定义表情]"
        case .quote:
            abstruct += message.quoteElem?.text ?? ""
        case .merge:
            abstruct = "[转发]"
        case .custom:
            abstruct += customMessageAbstruct(message: message)
        default:
            break
        }
        
        return abstruct
    }

    private static func getMidnightOf(date: Date) -> TimeInterval {
        let formatter = DateFormatter()
        formatter.dateFormat = "yyyy/MM/dd"
        let dateStr = formatter.string(from: date)
        let midnight = formatter.date(from: dateStr)
        return midnight?.timeIntervalSince1970 ?? 0
    }

    public static func convertList(timestamp_ms: Int) -> String {
        let current = TimeInterval(timestamp_ms / 1000)

        let date = Date(timeIntervalSince1970: current)
        let formatter = DateFormatter()
        formatter.dateFormat = "hh:mm a"
        formatter.amSymbol = "AM"
        formatter.pmSymbol = "PM"
        let zero = getMidnightOf(date: Date())
        let offset = zero - current
        let secondsPerDay: Double = 24 * 60 * 60

        var desc = ""
        if offset <= 0 {
            let now = Date().timeIntervalSince1970
            if fabs(now - current) < 3 * 60 {
                desc = "刚刚".innerLocalized()
            } else {
                desc = formatter.string(from: date)
            }
            return desc
        }

        if offset <= 1 * secondsPerDay {
            let m = formatter.string(from: date)
            desc = "昨天".innerLocalized() + " " + "\(m)"
            return desc
        }

        let calendar = Calendar.current
        if offset <= 7 * secondsPerDay {
            let flag = Calendar.Component.weekday
            let currentCompo = calendar.dateComponents([flag], from: date)
            guard let week = currentCompo.weekday else {
                return desc
            }
            switch week {
            case 1: desc = "周日".innerLocalized()
            case 2: desc = "周一".innerLocalized()
            case 3: desc = "周二".innerLocalized()
            case 4: desc = "周三".innerLocalized()
            case 5: desc = "周四".innerLocalized()
            case 6: desc = "周五".innerLocalized()
            case 7: desc = "周日".innerLocalized()
            default:
                break
            }
            return desc
        }

        formatter.dateFormat = "yyyy/MM/dd"
        desc = formatter.string(from: date)
        return desc
    }

    public static func getSystemNotificationOf(message: MessageInfo, isSingleChat: Bool) -> NSAttributedString? {
        let nameAttributes: [NSAttributedString.Key: Any] = [
            NSAttributedString.Key.font: UIFont.f14,
            NSAttributedString.Key.foregroundColor: UIColor.systemBlue,
        ]

        let contentAttributes: [NSAttributedString.Key: Any] = [
            NSAttributedString.Key.font: UIFont.f14,
            NSAttributedString.Key.foregroundColor: UIColor.c8E9AB0,
        ]
        func getSpaceString() -> NSAttributedString {
            return NSAttributedString(string: " ", attributes: contentAttributes)
        }
        func getOpUserName(message: MessageInfo) -> NSAttributedString {
            var ret = NSMutableAttributedString()
            if let opUser = message.notificationElem?.opUser {
                if opUser.userID == IMController.shared.userID {
                    ret.append(NSAttributedString(string: "你".innerLocalized(), attributes: contentAttributes))
                } else {
                    let name = NSAttributedString(string: opUser.nickname ?? "", attributes: nameAttributes)
                    ret.append(name)
                    ret.append(getSpaceString())
                }
            }
            return ret
        }
        switch message.contentType {
        case .friendAppApproved:
            let content = NSAttributedString(string: "你们已成功加为好友".innerLocalized(), attributes: contentAttributes)
            return content
        case .memberQuit:
            if let elem = message.notificationElem?.quitUser {
                var ret = NSMutableAttributedString()
                if elem.userID == IMController.shared.userID {
                    ret.append(NSAttributedString(string: "你".innerLocalized(), attributes: contentAttributes))
                } else {
                    let name = NSAttributedString(string: elem.nickname ?? "", attributes: nameAttributes)
                    ret.append(name)
                    ret.append(getSpaceString())
                }
                let content = NSAttributedString(string: "已退出群聊".innerLocalized(), attributes: contentAttributes)
                ret.append(content)
                return ret
            }
        case .memberEnter:
            if let elem = message.notificationElem?.entrantUser {
                var ret = NSMutableAttributedString()
                if elem.userID == IMController.shared.userID {
                    let name = NSAttributedString(string: "你".innerLocalized(), attributes: contentAttributes)
                    ret.append(name)
                } else {
                    ret.append(getSpaceString())
                    ret.append(NSAttributedString(string: elem.nickname ?? "", attributes: nameAttributes))
                    ret.append(getSpaceString())
                }
                ret.append(NSAttributedString(string: "已加入群聊".innerLocalized(), attributes: contentAttributes))
                return ret
            }
        case .memberKicked:
            let ret = NSMutableAttributedString()
            if let kickedUsers = message.notificationElem?.kickedUserList {
                let name = NSAttributedString(string: (kickedUsers.map {
                    $0.userID == IMController.shared.userID ?
                    "你" :
                    $0.nickname ?? ""
                }).joined(separator: ","),
                                              attributes: nameAttributes)
                ret.append(name)
            }
            ret.append(getSpaceString())
            ret.append(NSAttributedString(string: "被", attributes: contentAttributes))
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "踢出群聊", attributes: contentAttributes))
            return ret
        case .memberInvited:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "邀请".innerLocalized(), attributes: contentAttributes))
            if let invitedUser = message.notificationElem?.invitedUserList {
                let name = NSAttributedString(string: (invitedUser.map {
                    ($0.userID == IMController.shared.userID ?
                     "你" :
                        $0.nickname) ?? ""
                }).joined(separator: ","),
                                              attributes: nameAttributes)
                ret.append(name)
            }
            ret.append(NSAttributedString(string: "加入群聊".innerLocalized(), attributes: contentAttributes))
            return ret
        case .groupCreated:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "创建了群聊".innerLocalized(), attributes: contentAttributes))
            return ret
        case .groupInfoSet:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "更新了群信息".innerLocalized(), attributes: contentAttributes))
            return ret
        case .revoke:
            let ret = NSMutableAttributedString()
            var revoker = "" // 撤回人
            var beRevoker = "" // 被撤回的人
            
            let info = message.revokedInfo
            
            if info.sessionType == .c1v1 {
                revoker = info.revokerIsSelf ? "我".innerLocalized() : info.revokerNickname ?? ""
            } else {
                revoker = info.revokerIsSelf ? "我".innerLocalized() : info.revokerNickname!
                beRevoker = info.revokerIsSelf || info.revokerID == info.sourceMessageSendID
                ? "" : (info.sourceMessageSendIDIsSelf ? "我" : info.sourceMessageSenderNickname!)
            }
            
            ret.append(NSAttributedString(string: revoker, attributes: nameAttributes))
            ret.append(NSAttributedString(string: " 撤回了".innerLocalized(), attributes: contentAttributes))
            if !beRevoker.isEmpty {
                ret.append(NSAttributedString(string: " \(beRevoker) ".innerLocalized(), attributes: nameAttributes))
            }
            ret.append(NSAttributedString(string: "一条消息".innerLocalized(), attributes: contentAttributes))
            if info.revokerIsSelf, let textElem = message.textElem {
                let tapText = " 重新编辑".innerLocalized()
                ret.append(NSAttributedString(string: tapText, attributes: nameAttributes))
                let tapAttributes: [NSAttributedString.Key: Any] = [
                    .link: "reedit"
                ]
                let tapRange = (ret.string as NSString).range(of: tapText)
                ret.addAttributes(tapAttributes, range: tapRange)
            }
            return ret
        case .conversationNotification:
            let content = NSAttributedString(string: isSingleChat ? "你已开启此会话消息免打扰".innerLocalized() : "你已解除屏蔽该群聊".innerLocalized(), attributes: contentAttributes)
            return content
        case .conversationNotNotification:
            let content = NSAttributedString(string: isSingleChat ? "你已关闭此会话消息免打扰".innerLocalized() : "你已屏蔽该群聊".innerLocalized(), attributes: contentAttributes)
            return content
        case .dismissGrp:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "解散了群聊".innerLocalized(), attributes: contentAttributes))
            return ret
        case .typing:
            return nil
        case .privateMessage:
            if let value = message.notificationElem?.detailObject {
                let enable = value["isPrivate"] as? Bool
                let content = NSAttributedString(string: enable == true ?
                                                 "开启了阅后即焚".innerLocalized() :
                                                 "关闭了阅后即焚".innerLocalized(),
                                                 attributes: contentAttributes)
                return content
            }
            return nil
        case .groupMuted:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "开启了全体禁言".innerLocalized(), attributes: contentAttributes))
            return ret
        case .groupCancelMuted:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "取消了全体禁言".innerLocalized(), attributes: contentAttributes))
            return ret
        case .groupOwnerTransferred:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "将群主转让给了", attributes: contentAttributes))
            ret.append(NSAttributedString(string: "\(message.notificationElem?.groupNewOwner?.nickname ?? "")", attributes: nameAttributes))
            return ret
        case .groupSetName:
            let ret = NSMutableAttributedString()
            ret.append(getOpUserName(message: message))
            ret.append(NSAttributedString(string: "修改了群名称".innerLocalized(), attributes: contentAttributes))
            return ret
        case .oaNotification:
            let ret = NSMutableAttributedString()
            if let detail = message.notificationElem?.detailObject {
                 ret.append(NSAttributedString(string: "\(detail["text"] ?? "")", attributes: contentAttributes))
            }
            return ret
        case .groupMemberMuted:
            let ret = NSMutableAttributedString()
            let detail = message.notificationElem?.detailObject
            guard let detail = detail,
                    let mutedUser = detail["mutedUser"] as? [String: Any],
                    let mutedSeconds = detail["mutedSeconds"] as? Int else {return ret}
            
            var dispalySeconds = FormatUtil.getMutedFormat(of: mutedSeconds)
            ret.append(NSAttributedString(string: "\(mutedUser["nickname"] ?? "")", attributes: nameAttributes))
            ret.append(NSAttributedString(string: "被", attributes: contentAttributes))
            ret.append(NSAttributedString(string: "\(getOpUserName(message: message).string) ", attributes: nameAttributes))
            ret.append(NSAttributedString(string: "禁言 \(dispalySeconds)", attributes: contentAttributes))
            //ret.append(NSAttributedString(string: "\(mutedUser["nickname"] ?? "")被\(getOpUserName(message: message).string)禁言 \(dispalySeconds)", attributes: contentAttributes))
            return ret
        case .groupMemberCancelMuted:
            let ret = NSMutableAttributedString()
            let detail = message.notificationElem?.detailObject
            guard let detail = detail,
                    let mutedUser = detail["mutedUser"] as? [String: Any],
                    let opUser = detail["opUser"] as? [String: Any] else {return ret}
            
            ret.append(NSAttributedString(string: "\(getOpUserName(message: message).string)取消了\(mutedUser["nickname"] ?? "")的禁言", attributes: contentAttributes))
            return ret
        case .groupMemberInfoSet:
            let ret = NSMutableAttributedString()
            ret.append(NSAttributedString(string: "\(getOpUserName(message: message).string)编辑了群成员信息", attributes: contentAttributes))
            return ret            
        default:
            let content = NSAttributedString(string: "不支持的消息类型\(message.contentType)".innerLocalized(), attributes: contentAttributes)
            return content
        }
        return nil
    }

    public static func getAudioMessageDisplayWidth(duration: Int) -> CGFloat {
        let duration = CGFloat(duration)
        let Lmin: CGFloat = 50

        let Lmax: CGFloat = kScreenWidth * 0.5
        var barLen: CGFloat = 0
        var barCanChangeLen: CGFloat = Lmax - Lmin
        var unitWidth: CGFloat = barCanChangeLen / 58

        switch duration {
        case 0 ..< 2:
            barLen = Lmin
        case 2 ..< 10:
            barLen = Lmin + (duration - 2) * unitWidth
        case 10 ..< 60:
            barLen = (Lmin + 10 * unitWidth) + (70 - duration) / 10 * unitWidth
        default:
            barLen = Lmax
        }
        return barLen
    }
    
    public static func getCustomMessageValueOf(message: MessageInfo) -> NSAttributedString {
        guard message.contentType == .custom,
                let type = message.customElem?.type,
                let value = message.customElem?.value() else { return NSAttributedString() }
        
        let contentAttributes: [NSAttributedString.Key: Any] = [
            NSAttributedString.Key.font: UIFont.f14,
            NSAttributedString.Key.foregroundColor: UIColor.c8E9AB0,
        ]
        
        let isVideo = value["isVideo"] as? Bool ?? false
        let duration = value["duration"] as? Int ?? 0
        let linkDuration = FormatUtil.getCallingFormat(of: duration)
        let opUser = message.sendID == IMController.shared.getLoginUserID() ? "" : "对方"
        
        var str = NSMutableAttributedString()
        
        switch type {
        case .invite, .accept:
            break
        case .reject:
            str.append(NSAttributedString(string: opUser + "已拒绝".innerLocalized(), attributes: contentAttributes))
            break
        case .cancel:
            str.append(NSAttributedString(string: opUser + "已取消".innerLocalized(), attributes: contentAttributes))
            break
        case .hungUp:
            str.append(NSAttributedString(string: "通话时长".innerLocalized() + linkDuration, attributes: contentAttributes))
            break
        case .call:
            break
        case .blockedByFriend, .deletedByFriend, .mutedInGroup:
            str.append(NSAttributedString(string: customMessageAbstruct(message: message), attributes: contentAttributes))
            break
        default:
            break
        }
        return str
    }
    
    private static func customMessageAbstruct(message: MessageInfo) -> String {
        guard let type = message.customElem?.type else {
            return "不支持的消息类型".innerLocalized()
        }
        
        switch type {
        case .invite, .accept, .reject, .cancel, .hungUp:
            return "[" + "音视频".innerLocalized() + "]"
        case .call:
            return "[" + "音视频".innerLocalized() + "]"
        case .customEmoji:
            return "[表情]".innerLocalized()
        case .tagMessage:
            return "[标签]".innerLocalized()
        case .blockedByFriend:
            return "你已被对方拉黑".innerLocalized()
        case .deletedByFriend:
            return "对方开启了朋友验证，你还不是他（她）朋友，请先发送朋友验证请求，对方验证通过后才能聊天。".innerLocalized()
        case .mutedInGroup:
            return "您已被禁言".innerLocalized()
        }
    }
}

extension MessageInfo {
    public func getSummary() -> String {
        return MessageHelper.getSummary(by: self)
    }
}

extension ConversationInfo {
    public var summary: String? {
        return conversationType == .c1v1 ? latestMsg?.getSummary() : showName! + ":" + (latestMsg?.getSummary())!
    }
}
