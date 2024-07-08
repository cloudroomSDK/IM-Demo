import {
	MessageType
} from "@cloudroom/crimsdk";
import dayjs from "dayjs";
import {
	isThisYear
} from "date-fns";
import calendar from "dayjs/plugin/calendar";
import relativeTime from "dayjs/plugin/relativeTime";
import updateLocale from "dayjs/plugin/updateLocale";
import "dayjs/locale/zh-cn";
import useUserStore from "@/store/user";
import crimsdk, {
	SessionType
} from "@cloudroom/crimsdk";

dayjs.extend(calendar);
dayjs.extend(relativeTime);
dayjs.extend(updateLocale);
dayjs.locale("zh-cn");

dayjs.updateLocale("en", {
	calendar: {
		sameElse: "YYYY-MM-DD",
	},
});

const zhCalenDarOpt = {
	sameDay: "[今天] HH:mm:ss",
	nextDay: "[明天]",
	nextWeek: "dddd",
	lastDay: "[昨天] HH:mm",
	lastWeek: "dddd HH:mm",
	sameElse: "YYYY年M月D日 HH:mm",
};

dayjs.updateLocale("zh-cn", {
	calendar: zhCalenDarOpt,
});

export const calendarTime = (time, calendar = Object.assign(zhCalenDarOpt, {
	sameDay: "[今天] HH:mm",
})) => dayjs(time).calendar(null, calendar);

export const formatTime = (time, fmt = "YYYY-MM-DD HH:mm") => {
	return dayjs(time).format(fmt);
}

export const isMySelf = (userID) => userID === useUserStore().userID;

// 会话列表时间
export const formatConversionTime = (timestamp) => {
	if (!timestamp) return "";
	const now = dayjs(Date.now());
	const tsp = dayjs(timestamp);
	if (now.format("YYYY-MM-DD") === tsp.format("YYYY-MM-DD")) {
		// 上下午要显示AM、PM所以固定用了en
		return tsp.locale("en").format("hh:mm A");
	}
	return tsp.format("ddd");
};


export const parseMessageByType = (pmsg, isNotify = false) => {
	const getName = (user) => isMySelf(user.userID) ? "你" : user.nickname;;
	const msgPerfix = pmsg.sessionType === SessionType.Single ? "" : `${pmsg.senderNickname}：`;

	switch (pmsg.contentType) {
		case MessageType.TextMessage:
			return `${msgPerfix}${pmsg.textElem.content}`;
		case MessageType.AtTextMessage:
			return `${msgPerfix}${parseAt(pmsg.atTextElem, true)}`;
		case MessageType.PictureMessage:
			return `${msgPerfix}[图片]`;
		case MessageType.VoiceMessage:
			return `${msgPerfix}[语音]`;
		case MessageType.VideoMessage:
			return `${msgPerfix}[视频]`;
		case MessageType.FileMessage:
			return `${msgPerfix}[文件]`;
		case MessageType.LocationMessage:
			return `${msgPerfix}[位置]`;
		case MessageType.FaceMessage:
			return `${msgPerfix}[表情]`;
		case MessageType.RevokeMessage:
			return `${msgPerfix}撤回了一条消息`;
		case MessageType.FriendAdded:
			return "你们已经是好友了，开始聊天吧~";
		case MessageType.MemberEnter:
			const enterDetails = JSON.parse(pmsg.notificationElem.detail);
			const enterUser = enterDetails.entrantUser;
			return `${getName(enterUser)}进入了群聊`;
		case MessageType.GroupCreated:
			const groupCreatedDetail = JSON.parse(pmsg.notificationElem.detail);
			const groupCreatedUser = groupCreatedDetail.opUser;
			return `${getName(groupCreatedUser)}创建了群聊`;
		case MessageType.MemberInvited:
			const inviteDetails = JSON.parse(pmsg.notificationElem.detail);
			const inviteOpUser = inviteDetails.opUser;
			const invitedUserList = inviteDetails.invitedUserList ?? [];
			let inviteStr = "";
			invitedUserList.find(
				(user, idx) => (inviteStr += getName(user) + "、") && idx > 3,
			);
			inviteStr = inviteStr.slice(0, -1);
			return `${getName(inviteOpUser)}邀请了${inviteStr}${
        invitedUserList.length > 3 ? "..." : ""
      }进入群聊`;
		case MessageType.MemberKicked:
			const kickDetails = JSON.parse(pmsg.notificationElem.detail);
			const kickOpUser = kickDetails.opUser;
			const kickdUserList = kickDetails.kickedUserList ?? [];
			let kickStr = "";
			kickdUserList.find(
				(user, idx) => (kickStr += getName(user) + "、") && idx > 3,
			);
			kickStr = kickStr.slice(0, -1);
			return `${getName(kickOpUser)}踢出了${kickStr}${
        kickdUserList.length > 3 ? "..." : ""
      }`;
		case MessageType.MemberQuit:
			const quitDetails = JSON.parse(pmsg.notificationElem.detail);
			const quitUser = quitDetails.quitUser;
			return `${getName(quitUser)}退出了群聊`;
		case MessageType.GroupInfoUpdated:
			const groupUpdateDetail = JSON.parse(pmsg.notificationElem.detail);
			const groupUpdateUser = groupUpdateDetail.opUser;
			let updateFiled = "群设置";
			if (groupUpdateDetail.group.groupName) {
				updateFiled = `群名称为 ${groupUpdateDetail.group.groupName}`;
			}
			if (groupUpdateDetail.group.faceURL) {
				updateFiled = "群头像";
			}
			if (groupUpdateDetail.group.introduction) {
				updateFiled = "群介绍";
			}
			return `${getName(groupUpdateUser)}修改了${updateFiled}`;
		case MessageType.GroupOwnerTransferred:
			const transferDetails = JSON.parse(pmsg.notificationElem.detail);
			const transferOpUser = transferDetails.opUser;
			const newOwner = transferDetails.newGroupOwner;
			return `${getName(transferOpUser)}将群主转让给${getName(newOwner)}`;

		case MessageType.GroupMuted:
			const mutedDetails = JSON.parse(pmsg.notificationElem.detail);
			return `${getName(mutedDetails.opUser)}开启群禁言`;
		case MessageType.GroupCancelMuted:
			const cancelMutedDetails = JSON.parse(pmsg.notificationElem.detail);
			return `${getName(cancelMutedDetails.opUser)}取消群禁言`;
		case MessageType.GroupMemberMuted:
			const memMutedDetails = JSON.parse(pmsg.notificationElem.detail);
			return `${getName(memMutedDetails.opUser)}将${getName(memMutedDetails.mutedUser)}禁言`; // 至${formatTime(memMutedDetails.mutedUser.muteEndTime)}
		case MessageType.GroupMemberCancelMuted:
			const cancelMemMutedDetails = JSON.parse(pmsg.notificationElem.detail);
			return `${getName(cancelMemMutedDetails.opUser)}取消${getName(cancelMemMutedDetails.mutedUser)}的禁言`;
		case MessageType.GroupDismissed:
			const dismissDetails = JSON.parse(pmsg.notificationElem.detail);
			const dismissUser = dismissDetails.opUser;
			return `${getName(dismissUser)}解散了群聊`;
		case MessageType.GroupNameUpdated:
			const groupNameUpdateDetail = JSON.parse(pmsg.notificationElem.detail);
			const groupNameUpdateUser = groupNameUpdateDetail.opUser;
			return `${getName(groupNameUpdateUser)}修改了群名称为${
        groupNameUpdateDetail.group.groupName
      }`;
		case MessageType.OANotification:
			const customNoti = JSON.parse(pmsg.notificationElem.detail);
			return customNoti.text;
		default:
			return "";
	}
};

// 消息的时间分割线
export const getMsgTimeline = (time, time1) => {
	if ((time - time1) > 60 * 60 * 1000) {
		return dayjs(time).calendar();
	}
	return null;
}

const noticeMsgTypes = [
	MessageType.RevokeMessage,
	MessageType.FriendAdded,
	MessageType.GroupCreated,
	MessageType.GroupInfoUpdated,
	MessageType.MemberQuit,
	MessageType.GroupOwnerTransferred,
	MessageType.MemberKicked,
	MessageType.MemberInvited,
	MessageType.MemberEnter,
	MessageType.GroupDismissed,
	MessageType.GroupMemberMuted,
	MessageType.GroupMuted,
	MessageType.GroupCancelMuted,
	MessageType.GroupMemberCancelMuted,
	MessageType.GroupNameUpdated,
	MessageType.BurnMessageChange,
];
const textTypes = [
	MessageType.TextMessage,
	MessageType.AtTextMessage,
	MessageType.QuoteMessage,
];

const mediaTypes = [
	MessageType.VoiceMessage,
	MessageType.VideoMessage,
	MessageType.PictureMessage,
];
export const getMsgType = (type) => ({
	notice: noticeMsgTypes.includes(type),
	text: textTypes.includes(type),
	media: mediaTypes.includes(type),
	file: MessageType.FileMessage === type,
	card: MessageType.CardMessage === type,
	merge: MessageType.MergeMessage === type,
	location: MessageType.LocationMessage === type,
});

export const tipMsgFormat = (msg) => {
	const getName = (user) =>
		isMySelf(user.userID) ? "你" : user.nickname;

	const getUserID = (user) => user.userID;

	const parseInfo = (user) => formatHyperlink(getName(user), getUserID(user));

	const elemDetail = {};
	if (msg.notificationElem && msg.notificationElem.detail) {
		Object.assign(elemDetail, JSON.parse(msg.notificationElem.detail));
	}

	switch (msg.contentType) {
		case MessageType.FriendAdded:
			return `你们已经是好友了~`;
		case MessageType.GroupCreated:
			const groupCreatedUser = elemDetail.opUser;
			return `${parseInfo(groupCreatedUser)}创建了群聊`;
		case MessageType.GroupInfoUpdated:
			const groupUpdateUser = elemDetail.opUser;
			let updateFiled = "群设置";
			if (elemDetail.group.groupName) {
				updateFiled = `群名称为 ${elemDetail.group.groupName}`;
			}
			if (elemDetail.group.faceURL) {
				updateFiled = "群头像";
			}
			if (elemDetail.group.introduction) {
				updateFiled = "群介绍";
			}
			return `${parseInfo(groupUpdateUser)}修改了${updateFiled}`;
		case MessageType.RevokeMessage:
			return `${parseInfo({
				userID: elemDetail.revokerID,
				nickname: elemDetail.revokerNickname
			})} 撤回了一条消息`;
		case MessageType.GroupMuted:
			return `${parseInfo(elemDetail.opUser)} 开启群禁言`;
		case MessageType.GroupCancelMuted:
			return `${parseInfo(elemDetail.opUser)} 取消群禁言`;
		case MessageType.GroupMemberMuted:
			return `${parseInfo(elemDetail.opUser)} 将${parseInfo(elemDetail.mutedUser)}禁言至${formatTime(elemDetail.mutedUser.muteEndTime)}`;
		case MessageType.GroupMemberCancelMuted:
			return `${parseInfo(elemDetail.opUser)} 取消${parseInfo(elemDetail.mutedUser)}的禁言`;
		case MessageType.GroupOwnerTransferred:
			const transferOpUser = elemDetail.opUser;
			const newOwner = elemDetail.newGroupOwner;
			return `${parseInfo(transferOpUser)} 转让群主给${parseInfo(newOwner)}`;
		case MessageType.MemberQuit:
			const quitUser = elemDetail.quitUser;
			return `${parseInfo(quitUser)} 退出了群组`;
		case MessageType.MemberInvited:
			const inviteOpUser = elemDetail.opUser;
			const invitedUserList = elemDetail.invitedUserList ?? [];
			let inviteStr = "";
			invitedUserList.find(
				(user, idx) => (inviteStr += parseInfo(user) + "、") && idx > 3,
			);
			inviteStr = inviteStr.slice(0, -1);
			return `${parseInfo(inviteOpUser)} 邀请了${inviteStr}${
        invitedUserList.length > 3 ? "..." : ""
      }加入群聊`;
		case MessageType.MemberKicked:
			const kickOpUser = elemDetail.opUser;
			const kickdUserList = elemDetail.kickedUserList ?? [];
			let kickStr = "";
			kickdUserList.find(
				(user, idx) => (kickStr += parseInfo(user) + "、") && idx > 3,
			);
			kickStr = kickStr.slice(0, -1);
			return `${parseInfo(kickOpUser)} 踢出了${kickStr}${
        kickdUserList.length > 3 ? "..." : ""
      }`;
		case MessageType.MemberEnter:
			const enterUser = elemDetail.entrantUser;
			return `${parseInfo(enterUser)} 加入了群聊`;
		case MessageType.GroupDismissed:
			const dismissUser = elemDetail.opUser;
			return `${parseInfo(dismissUser)} 解散了群聊`;
		case MessageType.GroupNameUpdated:
			const groupNameUpdateUser = elemDetail.opUser;
			return `${parseInfo(groupNameUpdateUser)} 修改了群名称为${
        elemDetail.group.groupName
      }`;
		case MessageType.OANotification:
			return elemDetail.text;
		default:
			return "";
	}
};

export const formatMsgTime = (timestemp, keepSameYear = false) => {
	return dayjs(timestemp).format("M月D日 HH:mm:ss");
};

export const parseAt = (atel, isParse = false, opt = {
	linkStyle: "color:#0089FF;"
}) => {
	let mstr = atel.text;
	const pattern = /@\S+\s/g;
	const arr = mstr.match(pattern);
	const atUserList = atel.atUsersInfo ?? [];
	arr?.map((match) => {
		const member = atUserList.find(
			(user) => user.atUserID === match.slice(1, -1),
		);
		if (member && !isParse) {
			mstr = mstr.replace(
				match,
				formatHyperlink(`@${member.groupNickname} `, member.atUserID, opt.linkStyle),
			);
		} else {
			mstr = mstr.replace(match, `@${member.groupNickname} `);
		}
	});
	return mstr;
};

export const parseBr = (content) => {
	if (!content) {
		return "";
	}
	return content.replace(/\n/g, "\\n").trim();
};

export const formatHyperlink = (nickname, currentUserID, style = "color:#0089FF;") => {
	return `<a href="${currentUserID}" style="text-decoration:none;${style}">${nickname}</a>`;
};

const regex = /\b(http[s]?:\/\/[^\s]+)\b/g;
export const parseLink = (content) =>
	content.replace(regex, (match) => formatHyperlink(match, match));

export const secFormat = (sec) => {
	let h;
	let s;
	h = Math.floor(sec / 60);
	s = sec % 60;
	h += "";
	s += "";
	h = h.length === 1 ? "0" + h : h;
	s = s.length === 1 ? "0" + s : s;
	return h + ":" + s;
};

export const html2Text = (html) => {
	if (!html) {
		return "";
	}
	return html
		.replace(/<\/p><p>/g, "\n")
		.replace(/\&nbsp;/g, " ")
		.replace(/<p>/g, "")
		.replace(/<\/p>/g, "")
		.replace(/<br>/g, "");
	// .trim();
};

export const formatInputHtml = (html) => {
	let atUserList = [];
	let text = html2Text(html);
	const imgReg = new RegExp("(i?)(<img)([^>]+>)", "gmi");
	const customDataReg = /data-custom=".+"/;
	text = text.replace(imgReg, (img) => {
		if (img.includes('class="at_el"')) {
			const atInfoArr = img
				.match(customDataReg)[0]
				.slice(13, -1)
				.split("&amp;");
			atUserList.push({
				atUserID: atInfoArr[0].slice(7),
				groupNickname: atInfoArr[1].slice(15),
			});
			return `@${atInfoArr[0].slice(7)} `;
		}
		if (img.includes('class="emoji_el"')) {
			return img.match(customDataReg)[0].slice(23, -1);
		}
		return "";
	});
	return {
		text,
		atUserList,
	};
};

export const offlinePushInfo = {
	title: "you have a new msg",
	desc: "you have a new message",
	ex: "",
	iOSPushSound: "",
	iOSBadgeCount: true,
};

export const filterPreviewImage = (msgs) => {
	return msgs
		.filter((msg) => {
			if (msg.contentType === MessageType.PictureMessage) {
				return true;
			}
			if (msg.contentType === MessageType.OANotification) {
				let notificationData = {};
				try {
					notificationData = JSON.parse(msg.notificationElem.detail);
				} catch (error) {}
				if (notificationData.mixType === 1) {
					msg.pictureElem.snapshotPicture.url =
						notificationData.pictureElem.sourcePicture.url;
					return true;
				}
				return false;
			}
			return false;
		})
		.map((item) => item.pictureElem.sourcePicture.url);
}

export const faces = {
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
};