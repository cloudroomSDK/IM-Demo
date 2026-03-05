import { API } from "~/api/typings";
import {
  BlackUserItem,
  ConversationItem,
  FriendUserItem,
  GroupItem,
  GroupMemberItem,
  MessageItem,
  SelfUserInfo,
} from "~/utils/imsdk";
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
  userInfo?: SelfUserInfo;
  isSyncing: boolean; //正在同步数据库
  syncProgress: number; //同步进度
}

export declare enum MessageReceiveOptType {
  Nomal = 0,
  NotReceive = 1,
  NotNotify = 2,
}
export type MsgItem = MessageItem & {
  isShowTime?: boolean;
};

export interface ConversationStore {
  currentConversation?: ConversationItem;
  conversationList?: ConversationItem[];
  conversationListPromise?: Promise<ConversationItem[]>;
  currentQuoteMessage?: MsgItem;
  msgList: MsgItem[];
  multipleStatus: boolean;
  multipleSelect: string[];
  unreadMsgCount: number;
}

export interface FriendStore {
  list?: FriendUserItem[];
  newFriendCount: number; //有新的好友申请数量
  listPromise?: Promise<FriendUserItem[]>;
  blackList?: BlackUserItem[];
  blackListPromise?: Promise<BlackUserItem[]>;
}

export interface GroupStore {
  messageCount: number;
  list?: GroupItem[];
  listPromise?: Promise<GroupItem[]>;
  currentGroupInfo?: GroupItem;
  currentMemberInGroup?: GroupMemberItem;
  currentGroupMemberList?: GroupMemberItem[];
}
