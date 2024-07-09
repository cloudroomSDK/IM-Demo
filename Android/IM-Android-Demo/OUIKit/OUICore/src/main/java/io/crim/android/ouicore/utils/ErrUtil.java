package io.crim.android.ouicore.utils;

/**
 * Created by zjw on 2023/12/21.
 */
public class ErrUtil {

    public static String getErrTip(int code, String errMsg) {
        switch (code) {
            case 500:
                return "服务器内部错误";
            case 1002:
                return "没有权限";
            case 1004:
                return "没有记录";
            case 1005:
                return "用户已注册";
            case 1204:
                return "群已不存在";
            case 1302:
                return "你已被对方拉黑";
            case 1402:
                return "被禁言";
            case 10001:
                return "请求参数错误";
            case 10002:
                return "数据库错误";
            case 10003:
                return "服务器错误";
            case 10005:
                return "没有记录";
            case 10006:
                return "记录不存在";
            case 20001:
                return "密码错误";
            case 20002:
                return "账号不存在";
            case 20003:
                return "手机号已经注册";
            case 20004:
                return "账号已经注册";
            case 20005:
                return "频繁获取验证码";
            case 20006:
                return "验证码错误";
            case 20007:
                return "验证码过期";
            case 20008:
                return "验证码失败次数过多";
            case 20009:
                return "已经使用";
            case 20010:
                return "邀请码已经使用";
            case 20011:
                return "邀请码不存在";
            case 20012:
                return "该账号已注销";
            case 20013:
                return "拒绝添加好友";
            case 30001:
                return "验证码错误";
            case 30002:
                return "验证码已过期";
            case 30003:
                return "邀请码被使用";
            case 30004:
                return "邀请码不存在";
            case 40001:
                return "账号未注册";
            case 40002:
                return "密码错误";
            case 40003:
                return "登录受ip限制";
            case 40004:
                return "ip禁止注册登录";
            case 50001:
                return "token过期";
            case 50002:
                return "token格式错误";
            case 50003:
                return "token未生效";
            case 50004:
                return "未知错误";
            case 50005:
                return "创建错误";
        }
        return errMsg;
    }
}
