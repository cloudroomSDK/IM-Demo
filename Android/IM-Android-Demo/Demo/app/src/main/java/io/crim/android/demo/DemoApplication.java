package io.crim.android.demo;

import android.content.Context;
import android.content.Intent;
import android.text.TextUtils;
import android.util.Log;

import com.alibaba.android.arouter.launcher.ARouter;
import com.igexin.sdk.PushManager;
import com.tencent.bugly.crashreport.CrashReport;
import com.vanniktech.emoji.EmojiManager;
import com.vanniktech.emoji.google.GoogleEmojiProvider;

import java.util.HashMap;
import java.util.Map;

import androidx.multidex.MultiDex;
import crim_sdk_callback.OnConnListener;
import io.crim.android.demo.ui.login.LoginActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IM;
import io.crim.android.ouicore.im.IMEvent;
import io.crim.android.ouicore.net.RXRetrofit.HttpConfig;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.services.CallingService;
import io.crim.android.ouicore.utils.AndroidTool;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.L;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.UserLogic;
import io.crim.android.ouicore.voice.SPlayer;
import io.realm.Realm;


public class DemoApplication extends BaseApp {
    private static final String TAG = BaseApp.class.getSimpleName();
    public Realm realm;


    @Override
    public void onCreate() {
        L.e("App", "-----onCreate");
        super.onCreate();
        initController();

        MultiDex.install(this);
        initBugly();
        //ARouter init
        ARouter.init(this);
//        ARouter.crimLog();
//        ARouter.openDebug();

        initPush();
        //net init
        initNet();

        //im 初始化
        initIM();

        //音频播放
        SPlayer.init(this);
        SPlayer.instance().setCacheDirPath(Constant.AUDIO_DIR);

        EmojiManager.install(new GoogleEmojiProvider());
    }

    private void initBugly() {
        //Bugly崩溃捕获
        CrashReport.UserStrategy strategy = new CrashReport.UserStrategy(this);
        strategy.setAppChannel("IM demo android"); //设置渠道
        strategy.setCrashHandleCallback(new CrashReport.CrashHandleCallback() {
            @Override
            public synchronized Map<String, String> onCrashHandleStart(int i, String s, String s1, String s2) {
                return new HashMap();
            }

            @Override
            public synchronized byte[] onCrashHandleStart2GetExtraDatas(int i, String s, String s1, String s2) {
                byte[] extraDatas = null;
                try {
                    Context context = getApplicationContext();
                    StringBuilder builder = new StringBuilder();
                    builder.append("MeetVer:").append(AndroidTool.getVersion(context)).append("\n");
                    builder.append("DumpTime:")
                        .append(AndroidTool.getCurTimeStr("yyyy-MM-dd HH:mm:ss")).append("\n");
                    builder.append("IP:").append(AndroidTool.getIPAddress(context)).append("\n");
                    String nickName = BaseApp.inst().loginCertificate.nickName;
                    if (nickName == null || TextUtils.isEmpty(nickName)) {
                        nickName = BaseApp.inst().loginCertificate.userID;
                    }
                    builder.append("Client:").append(nickName).append("\n");
                } catch (Exception e) {
                    Log.w(TAG, "onCrashHandleStart2GetExtraDatas ex:" + e.getMessage());
                }
                return extraDatas;
            }
        });
        CrashReport.initCrashReport(
            this, "e3a70537c3",
            false, strategy
        );
        CrashReport.startCrashReport();
    }

    private void initController() {
        Easy.installVM(UserLogic.class);
    }


    private void initPush() {
        PushManager.getInstance().initialize(this);
        PushManager.getInstance().setDebugLogger(this, s -> L.i("getui", s));
    }

    private void initNet() {
        Constant.getSdkServer();
        Constant.getBusinessServer();
        N.init(new HttpConfig().setBaseUrl(Constant.getAppAuthUrl())
            .addInterceptor(chain -> {
                String token = "";
                try {
                    token = BaseApp.inst().loginCertificate.chatToken;
                } catch (Exception ignored) {
                }
                return chain.proceed(chain.request().newBuilder().addHeader("token",
                    token).addHeader("operationID",
                    System.currentTimeMillis() + "").build());
            }));
    }

    private void initIM() {
        IM.initSdk(this);
        listenerIMOffline();
        CallingService callingService =
            (CallingService) ARouter.getInstance().build(Routes.Service.CALLING).navigation();
        if (null != callingService) {
            callingService.initKeepAlive(getPackageName());
            IMEvent.getInstance().addSignalingListener(callingService);
        }

    }

    private void listenerIMOffline() {
        IMEvent.getInstance().addConnListener(new OnConnListener() {
            @Override
            public void onConnectFailed(int code, String error) {
            }

            @Override
            public void onConnectSuccess() {
            }

            @Override
            public void onConnecting() {
            }

            @Override
            public void onKickedOffline() {
                offline();
            }

            @Override
            public void onUserTokenExpired() {
                offline();
            }

            private void offline() {
                LoginCertificate.clear();
                CallingService callingService = (CallingService) ARouter.getInstance()
                    .build(Routes.Service.CALLING).navigation();
                if (null != callingService)
                    callingService.stopAudioVideoService(BaseApp.inst());

                BaseApp.inst().startActivity(new Intent(BaseApp.inst(), LoginActivity.class)
                    .addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK
                        | Intent.FLAG_ACTIVITY_NEW_TASK));
            }
        });
    }
}
