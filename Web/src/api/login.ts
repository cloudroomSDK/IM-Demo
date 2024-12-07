import { v4 as uuidv4 } from "uuid";
import { IMTYPE } from "~/utils/imsdk";
import request from "~/utils/request";
import { useUserStore } from "~/stores";
import { API } from "~/api/typings";

// @ts-ignore
const platform = window.electronAPI?.getPlatform() ?? 5;

const getAreaCode = (code: string) => (code.includes("+") ? code : `+${code}`);

export interface BusinessUserInfo {
  userID: string;
  password: string;
  account: string;
  phoneNumber: string;
  areaCode: string;
  email: string;
  nickname: string;
  faceURL: string;
  gender: number;
  level: number;
  birth: number;
  allowAddFriend: BusinessAllowType;
  allowBeep: BusinessAllowType;
  allowVibration: BusinessAllowType;
  globalRecvMsgOpt: IMTYPE.MessageReceiveOptType;
}

export enum BusinessAllowType {
  Allow = 1,
  NotAllow = 2,
}

export default {
  getCode(params: API.Login.SendSmsParams) {
    return request.post(
      "/account/code/send",
      {
        ...params,
        platform,
        areaCode: getAreaCode(params.areaCode),
      },
      {
        headers: {
          operationID: uuidv4(),
        },
      }
    );
  },
  login(params: API.Login.LoginParams) {
    return request.post<unknown, API.Login.LoginData>(
      "/account/login",
      {
        ...params,
        platform,
        areaCode: params.areaCode ? getAreaCode(params.areaCode) : undefined,
        usedFor: API.Login.UsedFor.Login,
      },
      {
        headers: {
          operationID: uuidv4(),
        },
      }
    );
  },
  loginByToken() {
    return request.post<unknown, API.Login.LoginData>("/account/login2", null, {
      headers: {
        operationID: uuidv4(),
      },
    });
  },
};

export const searchBusinessUserInfo = async (keyword: string) => {
  return request.post<unknown, { total: number; users: BusinessUserInfo[] }>(
    "/user/search/full",
    {
      keyword,
      pagination: {
        pageNumber: 1,
        showNumber: 10,
      },
    },
    {
      headers: {
        operationID: uuidv4(),
      },
    }
  );
};

export const getBusinessUserInfo = async (userIDs: string[]) => {
  return request.post<unknown, { users: BusinessUserInfo[] }>(
    "/user/find/full",
    {
      userIDs,
    },
    {
      headers: {
        operationID: uuidv4(),
      },
    }
  );
};

interface UpdateBusinessUserInfoParams {
  email: string;
  nickname: string;
  faceURL: string;
  gender: number;
  birth: number;
  allowAddFriend: number;
  allowBeep: number;
  allowVibration: number;
  globalRecvMsgOpt: number;
}

export const updateBusinessUserInfo = async (
  params: Partial<UpdateBusinessUserInfoParams>
) => {
  if (Object.keys(params).length === 0) {
    return;
  }
  return request.post<unknown>(
    "/user/update",
    {
      ...params,
      userID: useUserStore().userInfo?.userID,
    },
    {
      headers: {
        operationID: uuidv4(),
      },
    }
  );
};

// 获取服务器时间
export const getSvrTime: () => Promise<number> = (() => {
  let diffTime = 0; // 服务器时间与本地时间差
  let timePromise: Promise<void> | undefined;

  return async () => {
    if (!timePromise) {
      timePromise = new Promise<void>(async (resolve) => {
        try {
          const { svrTime } = await request.get<unknown, { svrTime: number }>(
            "/sys/time"
          );
          diffTime = Date.now() - svrTime;
        } catch (error) {
          timePromise = undefined;
        }
        resolve();
      });
    }
    await timePromise;
    return Date.now() - diffTime;
  };
})();
