package io.crim.android.demo.vm;

import android.view.View;

import com.alibaba.android.arouter.launcher.ARouter;
import com.igexin.sdk.PushManager;

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
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;
import io.crim.android.ouicore.vm.NotificationVM;
import io.crim.android.ouicore.vm.UserLogic;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.listener.OnConversationListener;
import io.crim.android.sdk.models.ConversationInfo;
import io.crim.android.sdk.models.UserInfo;

import static io.crim.android.ouicore.utils.Common.md5;

public class MainVM extends BaseViewModel<LoginVM.ViewAction> implements OnConnListener,
    OnConversationListener {

    private static final String TAG = "App";
    public MutableLiveData<String> nickname = new MutableLiveData<>("");
    public MutableLiveData<Integer> visibility = new MutableLiveData<>(View.INVISIBLE);
    public boolean fromLogin, isInitDate;
    private CallingService callingService;
    public State<Integer> totalUnreadMsgCount = new State<>();
    private final UserLogic userLogic = Easy.find(UserLogic.class);

    @Override
    protected void viewCreate() {
        IMEvent.getInstance().addConnListener(this);
        IMEvent.getInstance().addConversationListener(this);

        callingService =
            (CallingService) ARouter.getInstance().build(Routes.Service.CALLING).navigation();
        if (null != callingService) callingService.setOnServicePriorLoginCallBack(this::initDate);

        BaseApp.inst().loginCertificate = LoginCertificate.getCache(getContext());
        boolean logged = IMUtil.isLogged();
        if (fromLogin || logged) {
            initDate();
        } else {
            int loginType = SharedPreferencesUtil.get(BaseApp.inst()).getInteger("LOGIN_TYPE");
            if (loginType <= 0) {
                loginType = 1;
            }
            String appID = "";
            String token = "";
            String appSecret = "";
            if (loginType == 1) {
                appID = Constant.getAppID();
                appSecret = md5(Constant.getAppSecret());
                sdkLoginByAppId(BaseApp.inst().loginCertificate, appID, appSecret);
            } else {
                token = SharedPreferencesUtil.get(BaseApp.inst()).getString("LOGIN_TOKEN");
                sdkLoginByToken(BaseApp.inst().loginCertificate, token);
            }
        }
        if (null != BaseApp.inst().loginCertificate.nickName)
            nickname.setValue(BaseApp.inst().loginCertificate.nickName);
    }

    private void sdkLoginByAppId(LoginCertificate loginCertificate, String appId, String appSecret) {
        try {
            CRIMClient.getInstance().login(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    sdkLoginOnError(code, error);
                }

                @Override
                public void onSuccess(String data) {
                    initDate();
                }
            }, loginCertificate.userID, appId, appSecret);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void sdkLoginByToken(LoginCertificate loginCertificate, String token) {
        try {
            CRIMClient.getInstance().login(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    sdkLoginOnError(code, error);
                }

                @Override
                public void onSuccess(String data) {
                    initDate();
                }
            }, loginCertificate.userID, token);

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
        N.API(NiService.class).CommNI(Constant.getAppAuthUrl() + "client_config/get",
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
                }
            }

            @Override
            protected void onFailure(Throwable e) {
                toast(e.getMessage());
            }
        });
    }

    void getSelfUserInfo() {
        CRIMClient.getInstance().userInfoManager.getSelfInfo(new OnBase<UserInfo>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
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
    public void onConversationChanged(List<ConversationInfo> list) {

    }

    @Override
    public void onNewConversation(List<ConversationInfo> list) {

    }

    @Override
    public void onSyncServerFailed() {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.SYNC_ERR);
    }

    @Override
    public void onSyncServerFinish() {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.DEFAULT);
    }

    @Override
    public void onSyncServerStart() {
        userLogic.connectStatus.setValue(UserLogic.ConnectStatus.SYNCING);
    }

    @Override
    public void onTotalUnreadMessageCountChanged(int i) {
        totalUnreadMsgCount.setValue(i);
    }
}
