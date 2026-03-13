package io.crim.android.ouicore.entity;

import android.content.Context;

import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.LogUtil;
import io.crim.android.ouicore.utils.SPUtil;

public class LoginCertificate {
    public String nickName;
    public String faceURL;
    public String userID;
    public String imToken;
    public String chatToken;
    public String sdkToken;
    public String sdkSvr;
    public String sdkAppId;
    public String sdkAuthType;
    public String sdkSecret;

    public boolean allowSendMsgNotFriend;
    public boolean allowAddFriend;

    //允许响铃
    public boolean allowBeep;
    //允许振动
    public boolean allowVibration;
    // 全局免打扰 0：正常；1：不接受消息；2：接受在线消息不接受离线消息；
    public int globalRecvMsgOpt;

    public void cache(Context context) {
        SPUtil.get(context).setCache("user.LoginCertificate",
            GsonHel.toJson(this));
    }

    public static LoginCertificate getCache(Context context) {
        String u = SPUtil.get(context).getString("user.LoginCertificate");
        LogUtil.INSTANCE.logcat("LoginCertificate getCache:"+u);
        if (u.isEmpty()) return null;
        return GsonHel.fromJson(u, LoginCertificate.class);
    }

    public static void clear() {
        SPUtil.remove(BaseApp.inst(),
            "user.LoginCertificate");
    }

}
