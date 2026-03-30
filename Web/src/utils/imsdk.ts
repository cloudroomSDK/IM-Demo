import { CbEvents, getSDK, MessageItem } from "crim-wasm-sdk";
export * from "crim-wasm-sdk";

// vite里获取使用显示url引入wasm，webpack里需要使用url-loader解析
import SDK_WASM_URL from "../../node_modules/crim-wasm-sdk/assets/CRIM.wasm?url";
import SQL_WASM_URL from "../../node_modules/crim-wasm-sdk/assets/sql-wasm.wasm?url";

import { getPicInfo } from ".";
import { v4 as uuidV4 } from "uuid";
import {
  useFriendStore,
  useAppStore,
  useConversationStore,
  useUserStore,
} from "~/stores";
import { MemberSelect } from "~/components";

import { getSvrTime } from "~/api/login";
import { ElMessage } from "element-plus";

export const IMSDK = getSDK({
  assetsUrl: {
    sdkWasm: SDK_WASM_URL,
    sqlWasm: SQL_WASM_URL,
  },
  debug: false
});

IMSDK.on(CbEvents.OnSyncServerStart, (e) => {
  const userStore = useUserStore();
  userStore.syncProgress = 0;
  userStore.isSyncing = true;
});
IMSDK.on(CbEvents.OnSyncServerFailed, (e) => {
  const userStore = useUserStore();
  ElMessage.error("消息同步失败");
  userStore.isSyncing = false;
});
IMSDK.on(CbEvents.OnSyncServerProgress, ({ data }) => {
  const userStore = useUserStore();
  userStore.syncProgress = data;
});
IMSDK.on(CbEvents.OnSyncServerFinish, (e) => {
  const userStore = useUserStore();
  userStore.syncProgress = 0;
  userStore.isSyncing = false;
});

export const errorDesc: Record<number, string> = {
  601: "你已被对方删除",
  1302: "你已被对方拉黑",
  1305: "不同个项目下不能添加",
  1402: "被禁言",
};

const getMediaDuration = (path: string): Promise<number> =>
  new Promise((resolve) => {
    const vel = new Audio(path);
    vel.onloadedmetadata = function () {
      resolve(Number(vel.duration.toFixed()));
    };
  });

export const createImageMessage = async (file: File) => {
  const { width, height } = await getPicInfo(file);

  const baseInfo = {
    uuid: uuidV4(),
    type: file.type,
    size: file.size,
    width,
    height,
    url: "",
  };
  const options = {
    sourcePicture: baseInfo,
    bigPicture: baseInfo,
    snapshotPicture: baseInfo,
    sourcePath: "",
    file,
  };
  const { data } = await IMSDK.createImageMsgByFile(options);
  return data;
};
export const createVideoMessage = async (file: File, snapShotFile: File) => {
  const { width, height } = await getPicInfo(snapShotFile);
  const options = {
    videoFile: file,
    snapshotFile: snapShotFile,
    videoPath: "",
    duration: await getMediaDuration(URL.createObjectURL(file)),
    videoType: file.type,
    snapshotPath: "",
    videoUUID: uuidV4(),
    videoUrl: "",
    videoSize: file.size,
    snapshotUUID: uuidV4(),
    snapshotSize: snapShotFile.size,
    snapshotUrl: URL.createObjectURL(snapShotFile),
    snapshotWidth: width,
    snapshotHeight: height,
    snapShotType: snapShotFile.type,
  };
  const { data } = await IMSDK.createVideoMsgByFile(options);
  return data;
};

export const setDraft = (conversationID: string, draftText: string) => {
  IMSDK.setConversationDraft({
    conversationID,
    draftText,
  });
};

export const inviteFriendInGroup = async (groupID: string) => {
  const friendStore = useFriendStore();
  const appStore = useAppStore();
  const friendList = await friendStore.getList();

  const { data } = await IMSDK.getSpecifiedGrpMembersInfo({
    groupID: groupID,
    userIDList: friendList.map((item) => item.userID),
  });

  const inGroupUserIDList = data.map((item) => item.userID);

  // 在群内且是自己的好友时，添加disabled属性
  const selectList = friendList.map((item) => ({
    ...item,
    disabled: inGroupUserIDList.includes(item.userID!),
  }));

  const selectUserList = await appStore.showDialog<
    {
      userID: string;
      imgSrc: string;
      showName: string;
    }[]
  >({
    component: MemberSelect,
    title: "选择联系人",
    width: 700,
    data: {
      type: "friend",
      selectList: selectList,
    },
  });

  IMSDK.inviteUserToGrp({
    groupID: groupID,
    userIDList: selectUserList.map((item) => item.userID),
    reason: "",
  });
};

export const clearConversationMsg = async (conversationID: string) => {
  await IMSDK.clearConversationAndDeleteAllMsg(conversationID);
  const conversationStore = useConversationStore();

  if (
    conversationID === conversationStore.currentConversation?.conversationID
  ) {
    conversationStore.msgList = [];
  }
};

/**
 * 获取阅后即焚消息剩余时间
 * @param meesageItem
 * @returns { number } 剩余毫秒时间
 */
export const getPrivateExpirationMsgTime = (() => {
  let obj: Record<string, number> = {};
  return async (conversationID: string, msg: MessageItem): Promise<number> => {
    const svrTime = await getSvrTime();
    if (obj[msg.clientMsgID]) {
      return obj[msg.clientMsgID] - svrTime;
    }
    const hasReadTime = msg.attachedInfoElem!.hasReadTime || svrTime; //已读时间
    const endTime = hasReadTime + msg.attachedInfoElem!.burnDuration * 1e3; //到期时间
    obj[msg.clientMsgID] = endTime; //存储到期时间

    const time = endTime - svrTime; //剩余时间
    setTimeout(() => {
      const conversationStore = useConversationStore();
      IMSDK.deleteMsg({
        conversationID: conversationID,
        clientMsgID: msg.clientMsgID,
      });
      conversationStore.removeList([msg.clientMsgID]);
      delete obj[msg.clientMsgID];
    }, time);
    return time;
  };
})();
