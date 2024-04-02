import { ElMessage } from "element-plus";

interface ErrorData {
  errCode: number;
  errMsg?: string;
}

export const errorHandle = (errData: ErrorData) => {
  if (errData.errMsg) {
    const desc = errCodeMap[errData.errCode] || errData.errMsg;
    desc && ElMessage.error(desc);
  }
};

const errCodeMap: Record<number, string> = {
  1005: "用户已注册",
  20001: "密码错误",
  20002: "账号不存在",
  20003: "手机号已注册",
  20004: "账号已注册",
  20005: "操作过于频繁，请稍后再试",
  20006: "验证码错误",
  20007: "验证码已过期",
  20008: "验证码错误次数超过限制，请稍后再试",
  20009: "验证码已被使用",
  20010: "邀请码已被使用",
  20011: "邀请码不存在",
  20012: "该账号已注销",
  20013: "拒绝添加好友",
};
