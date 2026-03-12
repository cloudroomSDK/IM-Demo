package io.crim.android.demo.vm;

import android.text.TextUtils;
import android.util.Base64;
import android.view.View;

import com.alibaba.android.arouter.launcher.ARouter;
import com.igexin.sdk.PushManager;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import androidx.lifecycle.MutableLiveData;
import crim_sdk_callback.OnConnListener;
import io.crim.android.ouicore.api.NiService;
import io.crim.android.ouicore.api.OneselfService;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IMEvent;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.net.RXRetrofit.NetObserver;
import io.crim.android.ouicore.services.CallingService;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.MD5Util;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.NotificationVM;
import io.crim.android.ouicore.vm.UserLogic;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.listener.OnConversationListener;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.sdk.models.UserInfo;

public class MainVM extends BaseViewModel<LoginVM.ViewAction> implements OnConnListener, OnConversationListener {

    private static final String TAG = "MainVM";
    public MutableLiveData<String> nickname = new MutableLiveData<>("");
    public MutableLiveData<Integer> visibility = new MutableLiveData<>(View.INVISIBLE);
    public boolean fromLogin, isInitDate;
    private CallingService callingService;
    public State<Integer> totalUnreadMsgCount = new State<>();
    private final UserLogic userLogic = Easy.find(UserLogic.class);

    @Override
    protected void viewCreate() {
        BaseApp.inst().loginCertificate = LoginCertificate.getCache(getContext());
        boolean logged = IMUtil.isLogged();
        if (fromLogin || logged) {
            initDate();
        } else {
            String loginType = BaseApp.inst().loginCertificate.sdkAuthType;
            getIView().initSdk(BaseApp.inst().loginCertificate.sdkSvr);
            if (loginType.equals("0")) {
                sdkLoginByAppId(BaseApp.inst().loginCertificate);
            } else {
                sdkLoginByToken(BaseApp.inst().loginCertificate);
            }
        }
        IMEvent.getInstance().addConnListener(this);
        IMEvent.getInstance().addConversationListener(this);

        callingService =
            (CallingService) ARouter.getInstance().build(Routes.Service.CALLING).navigation();
        if (null != callingService) callingService.setOnServicePriorLoginCallBack(this::initDate);
        if (null != BaseApp.inst().loginCertificate.nickName)
            nickname.setValue(BaseApp.inst().loginCertificate.nickName);
    }

    private void sdkLoginByAppId(LoginCertificate loginCertificate) {
        writeLog(TAG + ":sdkLoginByAppId");
        logcat("sdkLoginByAppId==sdkSecret=" + loginCertificate.sdkSecret);
        String appSecret = loginCertificate.sdkSecret;
        if (appSecret != null && !TextUtils.isEmpty(appSecret)) {
            byte[] decode = Base64.decode(appSecret, Base64.NO_WRAP);
            appSecret = MD5Util.MD5(new String(decode, StandardCharsets.UTF_8));
        }
        try {
            CRIMClient.getInstance().login(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    writeLog("sdkLoginByAppId onError:code:" + code + " error:" + error);
                    logcat("sdkLoginByAppId onError:code:" + code + " error:" + error);
                    sdkLoginOnError(code, error);
                }

                @Override
                public void onSuccess(String data) {
                    writeLog("sdkLoginByAppId onSuccess");
                    logcat("sdkLoginByAppId onSuccess");
                    initDate();
                }
            }, loginCertificate.userID, loginCertificate.sdkAppId, appSecret);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void sdkLoginByToken(LoginCertificate loginCertificate) {
        writeLog(TAG + ":sdkLoginByToken");
        logcat("sdkLoginByToken==token===" + loginCertificate.sdkToken);
        try {
            CRIMClient.getInstance().login(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    writeLog("sdkLoginByToken onError:code:" + code + "-error:" + error);
                    logcat("sdkLoginByToken onError:code:" + code + "-error:" + error);
                    sdkLoginOnError(code, error);
                }

                @Override
                public void onSuccess(String data) {
                    writeLog("sdkLoginByToken onSuccess");
                    logcat("sdkLoginByToken onSuccess");
                    initDate();
                }
            }, loginCertificate.userID, loginCertificate.sdkToken);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void sdkLoginOnError(int code, String error) {
        getIView().toast(error + code);
        getIView().jump();
    }

    private void initDate() {
        if (isInitDate) return;
        isInitDate = true;
        if (null != callingService)
            callingService.startAudioVideoService(getContext());

        initGlobalVM();
        getIView().initDate();
        getSelfUserInfo();
        onConnectSuccess();

        getClientConfig();
    }

    private void initGlobalVM() {
        Easy.installVM(NotificationVM.class);
    }

    private void getClientConfig() {
        String appAuthUrl = Constant.getAppAuthUrl();
        if (!appAuthUrl.endsWith("/")) {
            appAuthUrl += "/";
        }
        N.API(NiService.class).CommNI(appAuthUrl + "client_config/get",
            BaseApp.inst().loginCertificate.chatToken,
            NiService.buildParameter().buildJsonBody()).compose(N.IOMain()).map(OneselfService.turn(Map.class)).subscribe(new NetObserver<Map>(getContext()) {
            @Override
            public void onSuccess(Map m) {
                try {
                    HashMap<String, Object> map = (HashMap) m.get("config");
                    int allowSendMsgNotFriend = Integer.valueOf((String) map.get("allowSendMsgNotFriend"));
                    BaseApp.inst().loginCertificate.allowSendMsgNotFriend = allowSendMsgNotFriend == 1;
                    BaseApp.inst().loginCertificate.cache(BaseApp.inst());
                } catch (Exception ignored) {
                    ignored.printStackTrace();
                }
            }

            @Override
            protected void onFailure(Throwable e) {
                logcat("getClientConfig onFailure: :" + e.getMessage());
                toast(e.getMessage());
            }
        });
    }

    void getSelfUserInfo() {
        CRIMClient.getInstance().userInfoManager.getSelfInfo(new OnBase<UserInfo>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
                logcat("getSelfUserInfo onError: :" + error);
            }

            @Override
            public void onSuccess(UserInfo data) {
                // 返回当前登录用户的资料
                BaseApp.inst().loginCertificate.nickName = (null == data.getNickname()) ? "" : data.getNickname();
                BaseApp.inst().loginCertificate.faceURL = data.getFaceURL();
                BaseApp.inst().loginCertificate.cache(getContext());
                PushManager.getInstance().bindAlias(getContext(), data.getUserID(), "sn" + data.getUserID());
                nickname.setValue(BaseApp.inst().loginCertificate.nickName);
                Obs.newMessage(Constant.Event.USER_INFO_UPDATE);
            }
        });
    }

    @Override
    protected void releaseRes() {
        IMEvent.getInstance().removeConnListener(this);
    }

    @Override
    public void onConnectFailed(int code, String error) {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.CONNECT_ERR);
    }

    @Override
    public void onConnectSuccess() {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.DEFAULT);
        visibility.setValue(View.VISIBLE);
    }

    @Override
    public void onConnecting() {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.CONNECTING);
    }

    @Override
    public void onKickedOffline() {
    }

    @Override
    public void onUserTokenExpired() {
    }

    @Override
    public void onUserTokenInvalid(String s) {

    }

    @Override
    public void onConversationChanged(List<ConversationInfo> list) {

    }

    @Override
    public void onNewConversation(List<ConversationInfo> list) {

    }

    @Override
    public void onSyncServerFailed(boolean b) {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.SYNC_ERR);
    }

    @Override
    public void onSyncServerFinish(boolean b) {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.DEFAULT);
    }

    @Override
    public void onSyncServerStart(boolean b) {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.SYNCING);
    }

    @Override
    public void onSyncServerProgress(long l) {

    }

    @Override
    public void onTotalUnreadMessageCountChanged(int i) {
        totalUnreadMsgCount.setValue(i);
    }
}
