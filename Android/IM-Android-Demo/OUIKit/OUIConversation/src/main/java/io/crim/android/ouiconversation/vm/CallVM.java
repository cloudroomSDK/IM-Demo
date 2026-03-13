package io.crim.android.ouiconversation.vm;

import android.app.Notification;
import android.graphics.Rect;
import android.os.SystemClock;
import android.text.TextUtils;
import android.util.Base64;

import com.rtc.sdk.RTCMeeting;
import com.rtc.sdk.RTCMeetingCallback;
import com.rtc.sdk.RTCMgr;
import com.rtc.sdk.RTCMgrCallback;
import com.rtc.sdk.RTCSDK;
import com.rtc.sdk.model.ASTATUS;
import com.rtc.sdk.model.BoardInfo;
import com.rtc.sdk.model.CRVIDEOSDK_ERR_DEF;
import com.rtc.sdk.model.CRVIDEOSDK_MEETING_DROPPED_REASON;
import com.rtc.sdk.model.INVITE_STATUS;
import com.rtc.sdk.model.MEDIA_STOP_REASON;
import com.rtc.sdk.model.MIXER_STATE;
import com.rtc.sdk.model.MeetInfo;
import com.rtc.sdk.model.MeetingAttr;
import com.rtc.sdk.model.MemberInfo;
import com.rtc.sdk.model.MixerOutputInfo;
import com.rtc.sdk.model.ScreenMarkData;
import com.rtc.sdk.model.Size;
import com.rtc.sdk.model.UserStatus;
import com.rtc.sdk.model.UsrVideoId;
import com.rtc.sdk.model.UsrVideoInfo;
import com.rtc.sdk.model.VSTATUS;
import com.rtc.sdk.model.VideoCfg;
import com.rtc.sdk.model.VideoStreamStatic;

import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import io.crim.android.ouicore.BuildConfig;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IMEvent;
import io.crim.android.ouicore.utils.BeanUtil;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.MD5Util;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.CustomMsgType;
import io.crim.android.sdk.enums.InvitationOpType;
import io.crim.android.sdk.listener.OnAdvanceMsgListener;
import io.crim.android.sdk.listener.OnMsgSendCallback;
import io.crim.android.sdk.models.CustomElemData;
import io.crim.android.sdk.models.KeyValue;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.OfflinePushInfo;
import io.crim.android.sdk.models.ReadReceiptInfo;
import io.crim.android.sdk.models.RevokedInfo;
import io.crim.android.sdk.models.SignalingInvitationInfo;
import io.crim.android.sdk.utils.JsonUtil;

/**
 * Created by zjw on 2024/9/18.
 */
public class CallVM extends BaseViewModel<CallVM.CallViewAction> implements RTCMgrCallback, OnAdvanceMsgListener, RTCMeetingCallback {

    private static final String TAG = "CallVM";
    public boolean callIncoming = false;
    public String userID = "";
    public String invitationMsgId = "";
    public long timeout = 15;
    public String groupID = "";
    public String nickname = "";
    public String faceUrl = "";
    public String mediaType = Constant.MediaType.AUDIO;
    public int roomID = 0;
    public State<Boolean> micOpen = new State<Boolean>(false);
    public State<Boolean> camOpen = new State<Boolean>(true);
    public State<Boolean> speakerMute = new State<Boolean>(true);
    public State<String> meetDurTime = new State<String>("00:00");
    private boolean startMeetTime = false;
    public int meetSecond = 0;
    public int meetMinute = 0;
    public Message newInvitationMsg;

    public interface CallViewAction extends io.crim.android.ouicore.base.IView {
        void err2Finish(String err, String cookie);

        void hangupCallSuccess();

        void cancelCallSuccess();

        void rejectCallSuccess();

        void enterMeetingSuccess();

        void acceptCall();

        void hangupCall();

        void cancelCall();

        void rejectCall();

        void userEnterMeeting(String userID);

        void userLeftMeeting(String userID);

        void videoStatusChanged(String userID, VSTATUS oldStatus, VSTATUS newStatus);

        void defVideoChanged(String userID, short videoID);

        void meetDurTimeChange(String time);
    }

    public void init() {
        RTCMgr.getInstance().registerCallback(this);
        RTCMeeting.getInstance().registerCallback(this);
        IMEvent.getInstance().addAdvanceMsgListener(this);
    }

    public void login() {
        LoginCertificate loginCertificate = BaseApp.inst().loginCertificate;
        // 设置服务器地址
        RTCSDK.getInstance().setServerAddr(loginCertificate.sdkSvr);
        String loginType = loginCertificate.sdkAuthType;
        if (loginType.equals("0")) {
            String appSecret = loginCertificate.sdkSecret;
            if (appSecret != null && !TextUtils.isEmpty(appSecret)) {
                byte[] decode = Base64.decode(appSecret, Base64.NO_WRAP);
                appSecret = MD5Util.MD5(new String(decode, StandardCharsets.UTF_8));
            }
            RTCMgr.getInstance().loginByAppID(loginCertificate.sdkAppId, appSecret, loginCertificate.userID, "", "");
        } else {
            RTCMgr.getInstance().loginByToken(loginCertificate.sdkToken, loginCertificate.userID, "", "");
        }
    }

    public void call() {
        CustomElemData data = new CustomElemData();
        SignalingInvitationInfo info = new SignalingInvitationInfo();
        info.setMediaType(mediaType);
        info.setRoomID(roomID+"");
        info.setInviteeUserIDList(new ArrayList<String>() {{
            add(userID);
        }});
        info.setInviterUserID(BaseApp.inst().loginCertificate.userID);
        info.setGroupID(groupID);
        data.setData(info);
        Message message = CRIMClient.getInstance().signalingManager.createSignalingInvite(data);
        invitationMsgId = message.getClientMsgID();
        writeLog(TAG + ":call mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        logcat(TAG + ": call mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        newInvitationMsg = message;
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("call===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": " + mediaType + " call error: " + error);
                getIView().toast(error);
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                writeLog(TAG + ": " + mediaType + " call success");
                logcat("call===success=" + JsonUtil.toString(message));
            }
        }, message, userID, groupID, new OfflinePushInfo());
    }

    public void accept() {
        if (newInvitationMsg==null){
            return;
        }
        Message message = CRIMClient.getInstance().signalingManager
            .createSignalingAccept(newInvitationMsg);
        CustomElemData customElemData = BeanUtil.getCustomElemData(message);
        writeLog(TAG + ": accept mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        logcat( TAG + ": accept mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("accept===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": " + mediaType + " accept error: " + error);
                getIView().toast(error);
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                writeLog(TAG + ": " + mediaType + " accept success");
                logcat("accept===success=" + JsonUtil.toString(message));
                enterMeeting();
            }
        }, message, userID, groupID, new OfflinePushInfo());
    }

    public void hangup() {
        if (newInvitationMsg==null){
            return;
        }
        int duration = 60 * meetMinute + meetSecond;
        int opType = InvitationOpType.CRIVT_OP_BUTT;
        if (duration==0){
            if (callIncoming){
                opType = InvitationOpType.CRIVT_OP_Reject;
            }else {
                opType = InvitationOpType.CRIVT_OP_Cancel;
            }
        }
        Message message = CRIMClient.getInstance().signalingManager.createSignalingHungUp(newInvitationMsg, opType,duration);
        writeLog(TAG + ": hangup mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        logcat(TAG + ": hangup mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("hangup===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": " + mediaType + " hangup error: " + error);
                getIView().toast(error);
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                writeLog(TAG + ": " + mediaType + " hangup success");
                logcat("hangup===success=" + JsonUtil.toString(message));
                notifyChatList(message);
                getIView().hangupCallSuccess();
            }
        }, message, userID, groupID, new OfflinePushInfo());
    }

    public void cancelCall() {
        if (newInvitationMsg==null){
            return;
        }
        Message message = CRIMClient.getInstance().signalingManager
            .createSignalingCancel(newInvitationMsg);
        writeLog(TAG + ": cancelCall mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        logcat(TAG + ": cancelCall mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("cancelCall===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": " + mediaType + " cancelCall error: " + error);
                if (getIView() != null) {
                    getIView().toast(error);
                }
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                logcat("cancelCall===success=" + JsonUtil.toString(message));
                writeLog(TAG + ": " + mediaType + " cancelCall success");
                notifyChatList(message);
                if (getIView() != null) {
                    getIView().cancelCallSuccess();
                }
            }
        }, message, userID, groupID, new OfflinePushInfo());
    }

    public void rejectCall() {
        if (newInvitationMsg==null){
            return;
        }
        Message message = CRIMClient.getInstance().signalingManager
            .createSignalingReject(newInvitationMsg, InvitationOpType.CRIVT_OP_Reject);
        writeLog(TAG + ": rejectCall mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        logcat(TAG + ": rejectCall mediaType: " + mediaType + " message=" + JsonUtil.toString(message));
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                logcat("rejectCall===onError=code: " + code + " error: " + error);
                writeLog(TAG + ": " + mediaType + " rejectCall error: " + error);
                if (getIView() != null) {
                    getIView().toast(error);
                }
            }

            @Override
            public void onProgress(long l) {

            }

            @Override
            public void onSuccess(Message message) {
                logcat("rejectCall===success=" + JsonUtil.toString(message));
                writeLog(TAG + ": " + mediaType + " rejectCall success");
                notifyChatList(message);
                if (getIView() != null) {
                    getIView().rejectCallSuccess();
                }
            }
        }, message, userID, groupID, new OfflinePushInfo());
    }

    private void notifyChatList(Message msg) {
        ChatVM chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
        if (chatVM != null) {
            chatVM.notifySendMsg(msg);
        }
    }

    public void switchMic() {
        micOpen.setValue(Boolean.FALSE.equals(micOpen.getValue()));
        String id = BaseApp.inst().loginCertificate.userID;
        if (Boolean.TRUE.equals(micOpen.getValue())) {
            RTCMeeting.getInstance().openMic(id);
        } else {
            RTCMeeting.getInstance().closeMic(id);
        }
    }

    public void switchCam() {
        camOpen.setValue(Boolean.FALSE.equals(camOpen.getValue()));
        String id = BaseApp.inst().loginCertificate.userID;
        if (Boolean.TRUE.equals(camOpen.getValue())) {
            RTCMeeting.getInstance().openVideo(id);
        } else {
            RTCMeeting.getInstance().closeVideo(id);
        }
    }

    public UsrVideoInfo reverseCam() {
        ArrayList<UsrVideoInfo> info = RTCMeeting.getInstance().getAllVideoInfo(BaseApp.inst().loginCertificate.userID);
        short defaultVideoID = RTCMeeting.getInstance().getDefaultVideo(BaseApp.inst().loginCertificate.userID);
        for (UsrVideoInfo videoInfo : info) {
            if (videoInfo.videoID != defaultVideoID) {
                RTCMeeting.getInstance().setDefaultVideo(videoInfo.userId, videoInfo.videoID);
                return videoInfo;
            }
        }
        return null;
    }

    public ArrayList<MemberInfo> getAllMembers() {
        return RTCMeeting.getInstance().getAllMembers();
    }

    public void setCallState(boolean calling) {
        ContactListVM contactListVM = BaseApp.inst().getVMByCache(ContactListVM.class);
        if (contactListVM != null) {
            contactListVM.isCalling = calling;
        }
    }

    public void setSpeakerMute() {
        speakerMute.setValue(Boolean.FALSE.equals(speakerMute.getValue()));
        RTCMeeting.getInstance().setSpeakerMute(Boolean.TRUE.equals(speakerMute.getValue()));
    }

    public void enterMeeting() {
        RTCMeeting.getInstance().enterMeeting(roomID, BaseApp.inst().loginCertificate.nickName);
    }

    @Override
    protected void releaseRes() {
        startMeetTime = false;
        IMEvent.getInstance().removeAdvanceMsgListener(this);
        RTCMeeting.getInstance().exitMeeting();
        RTCMgr.getInstance().logout();
        RTCMgr.getInstance().unregisterCallback(this);
        RTCMeeting.getInstance().unregisterCallback(this);
        super.releaseRes();
        Easy.delete(CallVM.class);
    }

    @Override
    public void onRecvNewMsg(Message message) {
        if (BaseApp.inst().loginCertificate.userID.equals(message.getRecvID())) {
            CustomElemData info = BeanUtil.getCustomElemData(message);
            if (info != null) {
                logcat(TAG + ": onRecvNewMsg===getCustomType=" + info.getCustomType());
                writeLog(TAG + ": onRecvNewMsg CustomType=" + info.getCustomType());
                switch (info.getCustomType()) {
                    case CustomMsgType.InviteeAccept:
                        getIView().acceptCall();
                        startKeepTime();
                        break;
                    case CustomMsgType.InviteeReject:
                        getIView().rejectCall();
                        break;
                    case CustomMsgType.InvitationCancel:
                        getIView().cancelCall();
                        break;
                    case CustomMsgType.InvitationHangUp:
                        getIView().hangupCall();
                        break;
                }
            }
        }
    }

    @Override
    public void onRecv1v1ReadReceipt(List<ReadReceiptInfo> list) {

    }

    @Override
    public void onRecvGrpReadReceipt(List<ReadReceiptInfo> list) {

    }

    @Override
    public void onRecvMsgRevokedV2(RevokedInfo info) {

    }

    @Override
    public void onRecvMsgExtensionsChanged(String s, List<KeyValue> list) {

    }

    @Override
    public void onRecvMsgExtensionsDeleted(String s, List<String> list) {

    }

    @Override
    public void onRecvMsgExtensionsAdded(String s, List<KeyValue> list) {

    }

    @Override
    public void onMsgDeleted(Message message) {

    }

    @Override
    public void onRecvOfflineNewMessage(List<Message> list) {

    }

    @Override
    public void onRecvOnlineOnlyMsg(List<Message> list) {

    }

    @Override
    public void loginSuccess(String usrID, String cookie) {
        writeLog(TAG + " loginSuccess");
        logcat(TAG + " loginSuccess");
        if (!callIncoming) {
            RTCMgr.getInstance().createMeeting("");
        }
    }

    @Override
    public void loginFail(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {
        writeLog(TAG + " loginFail " + sdkErr.toString());
        logcat(TAG + " loginFail " + sdkErr.toString());
        getIView().err2Finish(sdkErr.toString(), cookie);
    }

    @Override
    public void lineOff(CRVIDEOSDK_ERR_DEF sdkErr) {

    }

    @Override
    public void setDNDStatusSuccess(String cookie) {

    }

    @Override
    public void setDNDStatusFail(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void getUserStatusRslt(CRVIDEOSDK_ERR_DEF sdkErr, ArrayList<UserStatus> userStatus, String cookie) {

    }

    @Override
    public void notifyUserStatus(UserStatus uStatus) {

    }

    @Override
    public void startStatusPushRslt(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void stopStatusPushRslt(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void createMeetingSuccess(MeetInfo meetInfo, String cookie) {
        writeLog(TAG + " createMeetingSuccess");
        logcat(TAG + " createMeetingSuccess");
        roomID = meetInfo.ID;
        enterMeeting();
    }

    @Override
    public void createMeetingFail(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {
        writeLog(TAG + " createMeetingFail " + sdkErr.toString());
        logcat(TAG + " createMeetingFail " + sdkErr.toString());
        getIView().err2Finish(sdkErr.toString(), cookie);
    }

    @Override
    public void destroyMeetingRslt(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void callSuccess(String callID, String cookie) {

    }

    @Override
    public void callFail(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void acceptCallSuccess(String callID, String cookie) {

    }

    @Override
    public void acceptCallFail(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void rejectCallSuccess(String callID, String cookie) {

    }

    @Override
    public void rejectCallFail(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void hangupCallSuccess(String callID, String cookie) {
    }

    @Override
    public void hangupCallFail(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {
    }

    @Override
    public void notifyCallIn(String callID, MeetInfo meetInfo, String callerID, String usrExtDat) {

    }

    @Override
    public void notifyCallAccepted(String callID, MeetInfo meetInfo, String usrExtDat) {

    }

    @Override
    public void notifyCallRejected(String callID, CRVIDEOSDK_ERR_DEF reason, String usrExtDat) {

    }

    @Override
    public void notifyCallHungup(String callID, String usrExtDat) {

    }

    @Override
    public void sendCmdRlst(String sendId, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void sendBufferRlst(String sendId, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void sendFileRlst(String sendId, String fileName, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void sendProgress(String sendId, int sendedLen, int totalLen, String cookie) {

    }

    @Override
    public void cancelSendRlst(String sendId, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void notifyCmdData(String sourceUserId, String data) {

    }

    @Override
    public void notifyBufferData(String sourceUserId, String data) {

    }

    @Override
    public void notifyFileData(String sourceUserId, String tmpFile, String orgFileName) {

    }

    @Override
    public void notifyCancelSend(String sendId) {

    }

    @Override
    public void callMorePartyRslt(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void cancelCallMorePartyRslt(String callID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void notifyCallMorePartyStatus(String callID, INVITE_STATUS status) {

    }

    @Override
    public void notifyTokenWillExpire() {

    }

    @Override
    public void inviteSuccess(String inviteID, String cookie) {

    }

    @Override
    public void inviteFail(String inviteID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void cancelInviteSuccess(String inviteID, String cookie) {

    }

    @Override
    public void cancelInviteFail(String inviteID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void acceptInviteSuccess(String inviteID, String cookie) {

    }

    @Override
    public void acceptInviteFail(String inviteID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void rejectInviteSuccess(String inviteID, String cookie) {

    }

    @Override
    public void rejectInviteFail(String inviteID, CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void notifyInviteIn(String inviteID, String inviterUsrID, String usrExtDat) {

    }

    @Override
    public void notifyInviteAccepted(String inviteID, String usrExtDat) {

    }

    @Override
    public void notifyInviteRejected(String inviteID, CRVIDEOSDK_ERR_DEF reason, String usrExtDat) {

    }

    @Override
    public void notifyInviteCanceled(String inviteID, CRVIDEOSDK_ERR_DEF reason, String usrExtDat) {

    }

    //==================VideoCallback=====================

    @Override
    public void enterMeetingRslt(CRVIDEOSDK_ERR_DEF sdkErr) {
        logcat(TAG + " enterMeetingRslt sdkErr:" + sdkErr + "====" + roomID);
        writeLog(TAG + " enterMeetingRslt sdkErr:" + sdkErr + "====" + roomID);
        if (sdkErr == CRVIDEOSDK_ERR_DEF.CRVIDEOSDK_NOERR) {
            if (callIncoming) {
                startKeepTime();
            } else {
                call();
            }
            getIView().enterMeetingSuccess();
            if (mediaType.equals(Constant.MediaType.VIDEO)) {
                RTCMeeting.getInstance().openVideo(BaseApp.inst().loginCertificate.userID);
                // 获取摄像头的相关配置
                VideoCfg cfg = RTCMeeting.getInstance().getVideoCfg();
                //配置分辨率为640x360
                cfg.size = new Size(640, 360);
                //配置码率为默认
                cfg.maxbps = -1;
                // 保存摄像头配置到SDK
                RTCMeeting.getInstance().setVideoCfg(cfg);
            }
            if (BuildConfig.DEBUG) {
                micOpen.setValue(false);
                RTCMeeting.getInstance().closeMic(BaseApp.inst().loginCertificate.userID);
            } else {
                micOpen.setValue(true);
                RTCMeeting.getInstance().openMic(BaseApp.inst().loginCertificate.userID);
            }
            setSpeakerMute();
        } else {
            String sdkErrString = "enter meeting fail";
            if (sdkErr != null) {
                sdkErrString = sdkErr.toString();
            }
            getIView().err2Finish(sdkErrString, "");
        }
    }

    @Override
    public void userEnterMeeting(String userID) {
        logcat(TAG + " userEnterMeeting userID:" + userID);
    }

    public void startKeepTime() {
        startMeetTime = false;
        meetSecond = 0;
        meetMinute = 0;
        Thread threadMeetTime = new Thread(new Runnable() {
            @Override
            public void run() {
                while (startMeetTime) {
                    SystemClock.sleep(1000);
                    meetSecond++;
                    if (meetSecond > 59) {
                        meetSecond = 0;
                        meetMinute++;
                    }
                    String second = "00";
                    String minute = "00";
                    if (meetSecond < 10) {
                        second = "0" + meetSecond;
                    } else {
                        second = meetSecond + "";
                    }
                    if (meetMinute < 10) {
                        minute = "0" + meetMinute;
                    } else {
                        minute = meetMinute + "";
                    }
                    getIView().meetDurTimeChange(minute + ":" + second);
                }
            }
        });
        startMeetTime = true;
        threadMeetTime.start();
    }

    @Override
    public void userLeftMeeting(String userID) {
        getIView().userLeftMeeting(userID);
    }

    @Override
    public void setNickNameRsp(CRVIDEOSDK_ERR_DEF sdkErr, String userid, String newName) {

    }

    @Override
    public void notifyNickNameChanged(String userID, String oldName, String newName) {

    }

    @Override
    public void meetingStopped() {

    }


    @Override
    public void netStateChanged(int level) {

    }

    @Override
    public void audioDevChanged() {

    }

    @Override
    public void audioStatusChanged(String userID, ASTATUS oldStatus, ASTATUS newStatus) {

    }

    @Override
    public void notifySpeakerChanged(int speakType) {

    }

    @Override
    public void notifyAllAudioClose(String userID) {

    }

    @Override
    public void notifySetVoiceChange(short operId, short termId, int voiceType) {
    }

    @Override
    public void micEnergyUpdate(String userID, int oldLevel, int newLevel) {

    }

    @Override
    public void openVideoRslt(String devID, boolean bSuccess) {

    }

    @Override
    public void videoStatusChanged(String userID, VSTATUS oldStatus, VSTATUS newStatus) {
        getIView().videoStatusChanged(userID, oldStatus, newStatus);
    }

    @Override
    public void notifyVideoData(UsrVideoId usrVideoID, long frameTime) {

    }

    @Override
    public void notifyVideoStreamStatic(VideoStreamStatic videoStreamStatic) {

    }

    @Override
    public void videoDevChanged(String userID) {

    }

    @Override
    public void defVideoChanged(String userID, short videoID) {
        getIView().defVideoChanged(userID, videoID);
    }

    @Override
    public void notifyAudioPCMData(int aSide, byte[] audioDat) {

    }

    @Override
    public void notifyAudioErr(CRVIDEOSDK_ERR_DEF sdkErr) {
        RTCMeetingCallback.super.notifyAudioErr(sdkErr);
    }


    @Override
    public void startScreenShareRslt(CRVIDEOSDK_ERR_DEF sdkErr) {

    }

    @Override
    public void stopScreenShareRslt(CRVIDEOSDK_ERR_DEF sdkErr) {

    }

    @Override
    public void notifyScreenShareData(String userID, Rect changedRect, Size frameSize) {

    }

    @Override
    public void startScreenMarkRslt(CRVIDEOSDK_ERR_DEF sdkErr) {

    }

    @Override
    public void stopScreenMarkRslt(CRVIDEOSDK_ERR_DEF sdkErr) {

    }

    @Override
    public void notifyGiveCtrlRight(String operId, String targetId) {

    }

    @Override
    public void notifyReleaseCtrlRight(String operId, String targetId) {

    }

    @Override
    public void notifyScreenShareStarted(String sharerID) {
        RTCMeetingCallback.super.notifyScreenShareStarted(sharerID);
    }

    @Override
    public void notifyScreenShareStopped(String oprUserID) {
        RTCMeetingCallback.super.notifyScreenShareStopped(oprUserID);
    }

    @Override
    public void notifyScreenMarkStarted() {

    }

    @Override
    public void notifyScreenMarkStopped() {

    }

    @Override
    public void enableOtherMark(boolean bEnable) {

    }

    @Override
    public void sendMarkData(ScreenMarkData mkDat) {

    }

    @Override
    public void sendAllMarkData(ArrayList<ScreenMarkData> mkDatList) {

    }

    @Override
    public void clearAllMarks() {

    }

    @Override
    public void delMarkData(List<String> markIDList, String operatorID) {

    }

    @Override
    public void notifyInitBoardList(List<String> boardIDList) {

    }

    @Override
    public void notifyCreateBoard(BoardInfo boardInfo, String opId) {

    }

    @Override
    public void notifyCloseBoard(String boardId, String opId) {

    }

    @Override
    public void notifyCurrentBoard(String boardId, String opId) {

    }

    @Override
    public void notifyBoardExinfoUpdated(String boardId, String extInfo, String opId) {

    }

    @Override
    public void notifyMediaOpened(int totalTime, Size picSZ) {

    }

    @Override
    public void notifyMediaStart(String userid) {

    }

    @Override
    public void notifyMediaStop(String userid, MEDIA_STOP_REASON reason) {

    }

    @Override
    public void notifyMediaPause(String userid, boolean bPause) {

    }

    @Override
    public void notifyMediaData(String userid, int curPos) {

    }

    @Override
    public void locMixerStateChanged(String mixerID, MIXER_STATE state) {

    }

    @Override
    public void locMixerOutputInfo(String mixerID, String nameOrUrl, MixerOutputInfo info) {

    }

    @Override
    public void createCloudMixerFailed(String mixerID, CRVIDEOSDK_ERR_DEF err) {

    }

    @Override
    public void cloudMixerStateChanged(String operatorID, String mixerID, MIXER_STATE state, String exParam) {

    }

    @Override
    public void cloudMixerInfoChanged(String mixerID) {

    }

    @Override
    public void cloudMixerOutputInfoChanged(String mixerID, String jsonStr) {

    }

    @Override
    public void sendMeetingCustomMsgRslt(CRVIDEOSDK_ERR_DEF sdkErr, String cookie) {

    }

    @Override
    public void notifyMeetingCustomMsg(String fromUserID, String text) {

    }

    @Override
    public void notifyMeetingAttrsChanged(HashMap<String, MeetingAttr> attrSeq) {

    }

    @Override
    public void notifyMediaVolumeChanged(int vol) {

    }

    @Override
    public void kickoutRslt(CRVIDEOSDK_ERR_DEF sdkErr, String userID) {

    }

    @Override
    public void meetingDropped(CRVIDEOSDK_MEETING_DROPPED_REASON reason) {

    }

    @Override
    public void getMeetingAllAttrsSuccess(HashMap<String, MeetingAttr> attrSeq, String cookie) {

    }

    @Override
    public void getMeetingAllAttrsFail(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void getMeetingAttrsSuccess(HashMap<String, MeetingAttr> attrSeq, String cookie) {

    }

    @Override
    public void getMeetingAttrsFail(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void resetMeetingAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void addOrUpdateMeetingAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void delMeetingAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void clearMeetingAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void getUserAttrsSuccess(HashMap<String, HashMap<String, MeetingAttr>> attrMap, String cookie) {

    }

    @Override
    public void getUserAttrsFail(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void setUserAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void addOrUpdateUserAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void delUserAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void clearAllUserAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void clearUserAttrsRslt(CRVIDEOSDK_ERR_DEF err, String cookie) {

    }

    @Override
    public void notifyMeetingAttrsChanged(HashMap<String, MeetingAttr> add, HashMap<String, MeetingAttr> updates, List<String> delKeys) {

    }

    @Override
    public void notifyUserAttrsChanged(String uid, HashMap<String, MeetingAttr> add, HashMap<String, MeetingAttr> updates, List<String> delKeys) {

    }

    @Override
    public void configNotification(Notification.Builder notice) {

    }

    //==================VideoCallback=====================
}
