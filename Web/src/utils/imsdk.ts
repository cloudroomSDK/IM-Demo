import { getSDK } from "@cloudroom/im-wasm-sdk";
export type { IMTYPE } from "@cloudroom/im-wasm-sdk";
import { getPicInfo } from ".";
import { v4 as uuidV4 } from "uuid";
import { useFriendStore, useAppStore, useConversationStore } from "~/stores";
import { MemberSelect } from "~/components";

// vite里获取使用显示url引入wasm，webpack里需要使用url-loader解析
import SDK_WASM_URL from "@cloudroom/im-wasm-sdk/dist/CRIM.wasm?url";
import SQL_WASM_URL from "@jlongster/sql.js/dist/sql-wasm.wasm?url";

export const IMSDK = getSDK({
  assetsUrl: {
    sdkWasm: SDK_WASM_URL,
    sqlWasm: SQL_WASM_URL,
  },
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
  };
  const options = {
    sourcePicture: baseInfo,
    bigPicture: baseInfo,
    snapshotPicture: baseInfo,
    sourcePath: "",
    file,
  };
  return (await IMSDK.createImageMsgByFile(options)).data;
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

  return (await IMSDK.createVideoMsgByFile(options)).data;
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