import { API } from "~/api/typings";
import { IMTYPE } from "~/utils/imsdk";
import { ComponentOptions } from "vue";

export interface ConfigStore {
  businessServer: string;
  sdkServer: string;
  useToken: boolean;
  appId: string;
  appSecret: string;
  token: string;
}

export interface Dialog {
  component?: ComponentOptions;
  show: boolean;
  title?: string | null;
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
  // userID: string;
  appId?: string;
  appSecret?: string;
  token?: string;
}

export interface UserStore {
  businessData?: API.Login.LoginData;
  userInfo?: IMTYPE.SelfUserInfo;
  lastLoginInfo?: SDKLoginInfo;
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
  currentConversationIndex?: number;
  currentGroupInfo?: IMTYPE.GroupItem;
  currentMemberInGroup?: IMTYPE.GroupMemberItem;
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
  list?: IMTYPE.GroupItem[];
  listPromise?: Promise<IMTYPE.GroupItem[]>;
}
