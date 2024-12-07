import { API } from "~/api/typings";
import { IMTYPE } from "~/utils/imsdk";
import { Component } from "vue";

export interface ConfigStore {
  businessServer: string;
}

export interface Dialog {
  component?: Component;
  show: boolean;
  title?: string;
  data?: any;
  width?: number;
  confirm?: Function;
  cancel?: Function;
}
export interface AppStore {
  globalDialog: Dialog;
  sideWidth: number;
}

export interface SDKLoginInfo {
  sdkServer: string;
  appId?: string;
  appSecret?: string;
  token?: string;
}

export interface UserStore {
  businessData?: API.Login.LoginData;
  userInfo?: IMTYPE.SelfUserInfo;
}

export declare enum MessageReceiveOptType {
  Nomal = 0,
  NotReceive = 1,
  NotNotify = 2,
}
export type MsgItem = IMTYPE.MessageItem & {
  isShowTime?: boolean;
};

export interface ConversationStore {
  currentConversation?: IMTYPE.ConversationItem;
  conversationList?: IMTYPE.ConversationItem[];
  conversationListPromise?: Promise<IMTYPE.ConversationItem[]>;
  currentQuoteMessage?: MsgItem;
  msgList: MsgItem[];
  lastMinSeq: number;
  multipleStatus: boolean;
  multipleSelect: string[];
  unreadMsgCount: number;
}

export interface FriendStore {
  list?: IMTYPE.FriendUserItem[];
  newFriendCount: number; //有新的好友申请数量
  listPromise?: Promise<IMTYPE.FriendUserItem[]>;
  blackList?: IMTYPE.BlackUserItem[];
  blackListPromise?: Promise<IMTYPE.BlackUserItem[]>;
}

export interface GroupStore {
  messageCount: number;
  list?: IMTYPE.GroupItem[];
  listPromise?: Promise<IMTYPE.GroupItem[]>;
  currentGroupInfo?: IMTYPE.GroupItem;
  currentMemberInGroup?: IMTYPE.GroupMemberItem;
  currentGroupMemberList?: IMTYPE.GroupMemberItem[];
}
