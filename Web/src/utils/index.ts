// @ts-ignore
import CryptoJS from "crypto-js";
import { ElMessage, ElMessageBox } from "element-plus";
import { errorDesc, IMTYPE } from "./imsdk";
import { UserInfo } from "~/components";
import { h } from "vue";
import { useUserStore } from "~/stores";
import useClipboard from "vue-clipboard3";
import { getFaceList, unicodeToChar } from "./face";
export function base64Parms(str: string) {
  return CryptoJS.enc.Base64.parse(str).toString(CryptoJS.enc.Utf8);
}

export function md5(str: string) {
  return CryptoJS.MD5(str).toString();
}

/**
 * @function 弹出文件选择框，选择文件
 * @param {Boolean} multiple 是否多选
 * @param {String} accept 文件类型
 * @return {File[]}
 */
export function importFile({
  accept = "*",
  multiple = true,
}: {
  accept?: string;
  multiple?: boolean; //文件多选
}) {
  return new Promise<File[]>((resolve, reject) => {
    let input: HTMLInputElement | null = document.createElement("input");
    input.type = "file";
    input.accept = accept;
    input.multiple = multiple;
    input.click();
    let files: File[] = [];
    const focus = function () {
      setTimeout(() => {
        window.removeEventListener("focus", focus);
        input = null;
        reject();
      }, 1000);
    };
    window.addEventListener("focus", focus);

    input.onchange = function () {
      files = Array.from(input!.files ?? []);
      resolve(files);
    };
  });
}

export function clone(obj: any) {
  return JSON.parse(JSON.stringify(obj));
}

export function getPicInfo(file: File): Promise<HTMLImageElement> {
  return new Promise((resolve, reject) => {
    const _URL = window.URL || window.webkitURL;
    const img = new Image();
    img.onload = function () {
      resolve(img);
    };
    img.src = _URL.createObjectURL(file);
  });
}

export const base64toFile = (base64Str: string) => {
  var arr = base64Str.split(","),
    fileType = arr[0].match(/:(.*?);/)![1],
    bstr = atob(arr[1]),
    n = bstr.length,
    u8arr = new Uint8Array(n);

  while (n--) {
    u8arr[n] = bstr.charCodeAt(n);
  }

  return new File([u8arr], `screenshot${Date.now()}.png`, {
    type: fileType,
  });
};

export const getVideoSnshotFile = (file: File): Promise<File> => {
  const url = URL.createObjectURL(file);
  return new Promise((reslove, reject) => {
    const video = document.createElement("VIDEO") as HTMLVideoElement;
    video.autoplay = true;
    video.muted = true;
    video.innerHTML = `<source src="${url}" type="audio/mp4">`;
    const canvas = document.createElement("canvas");
    const ctx = canvas.getContext("2d");
    video.addEventListener("canplay", () => {
      canvas.width = video.videoWidth;
      canvas.height = video.videoHeight;

      //@ts-ignore
      ctx.drawImage(video, 0, 0, video.videoWidth, video.videoHeight);
      video.pause();
      const base64 = canvas.toDataURL("image/png");
      const file = base64toFile(base64);
      reslove(file);
    });
  });
};

export const toLastMessage = (messageItem: IMTYPE.MessageItem) => {
  if (messageItem.contentType === 101) {
    return messageItem.textElem.content;
  }
  if (messageItem.contentType === 102) {
    return "[图片]";
  }
  if (messageItem.contentType === 103) {
    return "[语音]";
  }
  if (messageItem.contentType === 104) {
    return "[视频]";
  }
  if (messageItem.contentType === 105) {
    return "[文件]";
  }
  if (messageItem.contentType === 106) {
    let text = messageItem.atTextElem.text;
    messageItem.atTextElem.atUsersInfo?.forEach((item) => {
      const reg = new RegExp(`@${item.atUserID}`, "g");
      text = text.replace(reg, `@${item.groupNickname}`);
    });
    return text;
  }
  if (messageItem.contentType === 107) {
    return "[聊天记录]";
  }
  if (messageItem.contentType === 108) {
    return "[名片]";
  }
  if (messageItem.contentType === 109) {
    return "[位置]";
  }
  if (messageItem.contentType === 114) {
    return messageItem.quoteElem.text;
  }
  if (messageItem.contentType === 1201) {
    return "你们已经成为好友，可以开始聊天了";
  }

  if (messageItem.contentType === 1501) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 创建了群聊`;
  }
  if (messageItem.contentType === 1504) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.quitUser.nickname} 退出了群聊`;
  }
  if (messageItem.contentType === 1507) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 将群主转让给了 ${data.newGroupOwner.nickname}`;
  }
  if (messageItem.contentType === 1508) {
    const data = JSON.parse(messageItem.notificationElem.detail);

    const nameListStr = data.kickedUserList
      // @ts-ignore
      .map((item) => item.nickname)
      .join("、");

    return `${nameListStr} 被 ${data.opUser.nickname} 踢出群聊`;
  }
  if (messageItem.contentType === 1509) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    const nameListStr = data.invitedUserList
      // @ts-ignore
      .map((item) => item.nickname)
      .join("、");
    return `${data.opUser.nickname} 邀请 ${nameListStr} 加入群聊`;
  }
  if (messageItem.contentType === 1510) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.entrantUser.nickname} 加入了群聊`;
  }
  if (messageItem.contentType === 1511) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 解散了群聊`;
  }
  if (messageItem.contentType === 1512) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.mutedUser.nickname} 被 ${data.opUser.nickname} 禁言${~~(
      data.mutedSeconds / 60
    )}分${data.mutedSeconds % 60}秒`;
  }
  if (messageItem.contentType === 1513) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 取消了 ${data.mutedUser.nickname} 的禁言`;
  }
  if (messageItem.contentType === 1514) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 开启了群禁言`;
  }
  if (messageItem.contentType === 1515) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 关闭了群禁言`;
  }
  if (messageItem.contentType === 1520) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `${data.opUser.nickname} 修改了群名称`;
  }
  if (messageItem.contentType === 1701) {
    const data = JSON.parse(messageItem.notificationElem.detail);
    return `已${data.isPrivate ? "开启" : "关闭"}阅后即焚`;
  }
  if (messageItem.contentType === 2101) {
    const userStore = useUserStore();
    const data = JSON.parse(messageItem.notificationElem.detail);
    let nickname = "";
    if (data.revokerID === userStore.userInfo?.userID) {
      if (data.revokerID === data.sourceMessageSendID) {
        nickname = "你";
      } else {
        nickname = "admin";
      }
    } else {
      nickname = data.sourceMessageSenderNickname;
    }
    return `${nickname}撤回了一条消息`;
  }

  return "[未知消息]" + messageItem.contentType;
};

export const openUserInfo = (
  userID: string,
  groupInfo?: IMTYPE.GroupItem,
  isAdmin: boolean = false
) => {
  ElMessageBox({
    title: "",
    message: h(UserInfo, {
      userID,
      groupID: groupInfo?.groupID,
      allowLookInfo: groupInfo?.groupID
        ? isAdmin || groupInfo?.lookMemberInfo === 0
        : true,
      allowAddFriend: groupInfo?.groupID
        ? isAdmin || groupInfo?.applyMemberFriend === 0
        : true,
      onClose: () => {
        ElMessageBox.close();
      },
    }),
    showClose: true,
    customStyle: { width: "300px" },
    showConfirmButton: false,
  }).then((action) => {
    console.log(action);
  });
};
export async function copyToClipboard(text: string) {
  try {
    const { toClipboard } = useClipboard();
    await toClipboard(text);
    console.log("Copied to clipboard");
  } catch (e) {
    console.error(e);
  }
}

export const downloadUrl = (url: string) => {
  const a = document.createElement("a");
  a.href = url;
  a.click();
};
export const formatFileSize = (bytes: number): string => {
  if (bytes === 0) return "0 bytes";

  const units = ["bytes", "KB", "MB", "GB", "TB"];

  let num = bytes;
  let unitIndex = 0;

  while (num >= 1024 && unitIndex < units.length - 1) {
    num = num / 1024;
    unitIndex++;
  }

  return `${num.toFixed(2)} ${units[unitIndex]}`;
};

export const errorHandle = (error: IMTYPE.WsResponse) => {
  console.error(error);
  if (error.errCode) {
    const desc = errorDesc[error.errCode] || error.errMsg;
    ElMessage.error({
      message: desc,
      offset: 50,
    });
  }
};

export const chatTextSplit = (function () {
  const emoticons = getFaceList();
  return (message: IMTYPE.MessageItem | string) => {
    let text = "";
    let atUsersInfo: any[] = [];
    if (typeof message === "string") {
      text = message;
    } else if (message.contentType === 101) {
      text = message.textElem.content;
    } else if (message.contentType === 106) {
      text = message.atTextElem.text;
      atUsersInfo = message.atTextElem.atUsersInfo || [];
    } else if (message.contentType === 114) {
      text = message.quoteElem.text;
    }
    const atIds = atUsersInfo.map((item) => `@${item.atUserID}`);
    const idsReg = atIds.length ? `(${atIds.join("|")})|` : "";
    const easyAtReg = `(@\{\\w+\\|.+?\})|`;
    const regex = new RegExp(
      `${idsReg}${easyAtReg}(?:${emoticons.join("|")})`,
      "g"
    );

    // 执行匹配并生成结果
    let result = [];
    let lastIndex = 0;
    let match;
    while ((match = regex.exec(text)) !== null) {
      // 将当前表情之前的文本作为普通文本部分加入结果
      if (match.index > lastIndex) {
        result.push({
          type: "text",
          text: text.substring(lastIndex, match.index),
        });
      }
      // 将匹配到的表情加入结果
      if (match[0].startsWith("@{")) {
        const idx = match[0].indexOf("|");
        const userID = match[0].slice(2, idx);
        const nickname = match[0].slice(idx + 1, -1);

        // 将匹配到的@加入结果
        result.push({
          type: "at",
          userID,
          nickname, // 去除@
        });
      } else if (match[0].startsWith("@")) {
        const userID = match[0].slice(1);
        const nickname = atUsersInfo.find(
          (item) => item.atUserID === userID
        )?.groupNickname;
        // 将匹配到的@加入结果
        result.push({
          type: "at",
          userID,
          nickname, // 去除@
        });
      } else {
        const unicode = match[0].codePointAt(0)?.toString(16);
        if (unicode) {
          const char = unicodeToChar(unicode);
          const emojiName = emoticons.find((item) => item === char);
          if (emojiName) {
            result.push({ type: "face", value: unicode.toLocaleUpperCase() });
          }
        }
      }

      lastIndex = regex.lastIndex;
    }

    // 将最后一个表情之后的文本加入结果
    if (lastIndex < text.length) {
      result.push({
        type: "text",
        text: text.substring(lastIndex),
      });
    }
    return result;
  };
})();
