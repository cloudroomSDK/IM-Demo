package io.crim.android.ouicore.utils;

import android.text.TextUtils;

import io.crim.android.ouicore.AccountConfig;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.im.IM;
import io.crim.android.sdk.enums.CRIMLogLevel;

public class Constant {

    public static final String DEFAULT_IP = AccountConfig.DEFAULT_IP;

    public static final String HTTPS_IGNORE = "HTTPS_IGNORE";

    //--------IP----------
    private static final String PROTOCOL = "http";
    private static final String APP_AUTH_PORT = "8018";
    //--------------------

    public static int LOG_LEVEL = CRIMLogLevel.Debug.value();

    public static String getBusinessServer() {
        String ip = SPUtil.get(BaseApp.inst()).getString("DEFAULT_IP");
        if (!TextUtils.isEmpty(ip)) {
            return ip;
        }
        return DEFAULT_IP;
    }

    public static String getAppAuthUrl() {
        String url = SPUtil.get(BaseApp.inst()).getString("APP_AUTH_URL");
        if (TextUtils.isEmpty(url)) return PROTOCOL + "://" + DEFAULT_IP + ":" + APP_AUTH_PORT + "/";
        return url;
    }

    public static String getDefaultAppAuthUrl() {
        return PROTOCOL + "://" + DEFAULT_IP + ":" + APP_AUTH_PORT + "/";
    }

    //存储音频的文件夹
    public static final String AUDIO_DIR = IM.getStorageDir() + "/audio/";
    //视频存储文件夹
    public static final String VIDEO_DIR = IM.getStorageDir() + "/video/";
    //图片存储文件夹
    public static final String PICTURE_DIR = IM.getStorageDir() + "/picture/";
    //文件夹
    public static final String File_DIR = IM.getStorageDir() + "/file/";

    //二维码
    public static class QR {
        public static final String QR_ADD_FRIEND = "io.crim.app/addFriend";
        public static final String QR_JOIN_GROUP = "io.crim.app/joinGroup";
    }


    public static class Event {
        //转发选人
        public static final int FORWARD = 10002;
        //音视频通话
        public static final int CALLING_REQUEST_CODE = 10003;
        //用户信息更新
        public static final int USER_INFO_UPDATE = 10004;
        //设置背景
        public static final int SET_BACKGROUND = 10005;
        //群信息更新
        public static final int UPDATE_GROUP_INFO = 10006;
        //设置群通知
        public static final int SET_GROUP_NOTIFICATION = 10007;
        //插入了消息到本地
        public static final int INSERT_MSG = 10008;
        //推荐名片到当前聊天
        public static final int RECOMMEND_FROM_CHAT = 10009;
        //调转到地图
        public static final int MAP_POSITION = 10010;
    }

    public static final String K_ID = "Id";
    public static final String K_ROOM_ID = "K_ROOM_ID";
    public static final String K_INVITATION_MSG_ID = "K_INVITATION_MSG_ID";
    public static final String K_GROUP_ID = "group_id";
    public static final String K_IS_PERSON = "is_person";
    public static final String K_NOTICE = "notice";
    public static final String K_NAME = "name";
    public static final String K_RESULT = "result";
    public static final String K_RESULT2 = "result2";
    public static final String K_FROM = "from";
    public static final String K_SIZE = "size";
    public static final String NOTICE_TAG = "msg_notification";
    public static final String CALL_INCOMING = "CALL_INCOMING";
    public static final String K_MEDIA_TYPE = "MEDIA_TYPE";
    public static final String K_FACE_URL = "K_FACE_URL";


    //最大通话人数
    public static final int MAX_CALL_NUM = 9;
    //好友红点
    public static final String K_FRIEND_NUM = "k_friend_num";
    //群红点
    public static final String K_GROUP_NUM = "k_group_num";
    public static final String K_SET_BACKGROUND = "set_background";

    //邀请入群
    public static final String IS_INVITE_TO_GROUP = "isInviteToGroup";
    //移除群聊
    public static final String IS_REMOVE_GROUP = "isRemoveGroup";
    public static final String IS_CREATE_GROUP = "isCreateGroup";
    //选择群成员
    public static final String IS_SELECT_MEMBER = "isSelectMember";
    //选择好友
    public static final String IS_SELECT_FRIEND = "isSelectFriend";
    //自定义消息类型
    public static final String K_CUSTOM_TYPE = "customType";


    //加载中
    public static final int LOADING = 201;

    public static class MsgType {
        //本地呼叫记录
        public static final int LOCAL_CALL_HISTORY = -110;

        //会议邀请
        public static final int CUSTOMIZE_MEETING = 905;

    }


    /// 进群验证设置选项
    public static class GroupVerification {
        /// 申请需要同意 邀请直接进
        public static final int applyNeedVerificationInviteDirectly = 0;

        /// 所有人进群需要验证，除了群主管理员邀
        public static final int allNeedVerification = 1;

        /// 直接进群
        public static final int directly = 2;
    }

    //超级群
    public static final int SUPER_GROUP_LIMIT = 250;

    public static class MediaType {
        public static final String VIDEO = "video";
        public static final String AUDIO = "audio";
    }
}
