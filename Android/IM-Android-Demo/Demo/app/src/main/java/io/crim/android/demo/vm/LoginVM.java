package io.crim.android.demo.vm;

import android.util.Log;

import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import androidx.annotation.NonNull;
import androidx.lifecycle.MutableLiveData;
import io.crim.android.demo.repository.CRIMService;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.net.RXRetrofit.NetObserver;
import io.crim.android.ouicore.net.RXRetrofit.Parameter;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;
import io.crim.android.ouicore.widget.WaitDialog;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.Platform;
import io.crim.android.sdk.listener.OnBase;

import static io.crim.android.ouicore.utils.Common.md5;


/**
 * Created by zjw on 2023/9/21.
 */
public class LoginVM extends BaseViewModel<LoginVM.ViewAction> {
    public static final int MAX_COUNTDOWN = 60;

    public MutableLiveData<String> account = new MutableLiveData<>("");
    //密码或验证码
    public MutableLiveData<String> pwd = new MutableLiveData<>("");
    public MutableLiveData<Boolean> isPhone = new MutableLiveData<>(true);
    public MutableLiveData<Integer> countdown = new MutableLiveData<>(MAX_COUNTDOWN);
    public MutableLiveData<String> nickName = new MutableLiveData<>("");
    public String verificationCode;
    //是否是找回密码
    public boolean isFindPassword = false;

    public void login(String verificationCode, int usedFor) {
        Parameter parameter = getParameter(verificationCode, usedFor);
        N.API(CRIMService.class)
            .login(parameter.buildJsonBody())
            .compose(N.IOMain())
            .map(CRIMService.turn(LoginCertificate.class))
            .subscribe(new NetObserver<LoginCertificate>(getContext()) {

                @Override
                public void onSuccess(LoginCertificate loginCertificate) {
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
                    } else {
                        token = SharedPreferencesUtil.get(BaseApp.inst()).getString("LOGIN_TOKEN");
                    }
                    try {
                        CRIMClient.getInstance().login(new OnBase<String>() {
                            @Override
                            public void onError(int code, String error) {
                                Log.d("eeeeeee", "onError===" + error);
                                getIView().err(error);
                            }

                            @Override
                            public void onSuccess(String data) {
                                //缓存登录信息
                                loginCertificate.cache(getContext());
                                BaseApp.inst().loginCertificate = loginCertificate;
                                getIView().jump();
                            }
                        }, loginCertificate.userID, token, appID, appSecret);

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                @Override
                protected void onFailure(Throwable e) {
                    getIView().err(e.getMessage());
                }
            });
    }

    /**
     * @param verificationCode
     * @param usedFor          1注册 2重置 3登录
     * @return
     */
    @NonNull
    private Parameter getParameter(String verificationCode, int usedFor) {
        Parameter parameter = new Parameter().add("password", "")
//                TextUtils.isEmpty(verificationCode) ? md5(pwd.getValue()) : null)
            .add("platform", 2)
            .add("usedFor", usedFor)
            .add("operationID", System.currentTimeMillis() + "")
            .add("verifyCode", verificationCode);
//        if (isPhone.getValue()) {
        parameter.add("phoneNumber", account.getValue());
        parameter.add("areaCode", "+86");
        parameter.add("appID", Constant.getAppID());
//        } else
//            parameter.add("email", account.getValue());
        return parameter;
    }


    /**
     * @param usedFor 1注册 2重置 3登录
     */
    public void getVerificationCode(int usedFor) {
        Parameter parameter = getParameter(null, usedFor);
        WaitDialog waitDialog = showWait();
        N.API(CRIMService.class).getVerificationCode(parameter.buildJsonBody()).map(CRIMService.turn(Object.class)).compose(N.IOMain()).subscribe(new NetObserver<Object>(getContext()) {
            @Override
            public void onSuccess(Object o) {
                getIView().succ(o);
            }

            @Override
            public void onComplete() {
                super.onComplete();
                waitDialog.dismiss();
            }

            @Override
            protected void onFailure(Throwable e) {
                getIView().err(e.getMessage());
            }
        });

    }

    @NonNull
    public WaitDialog showWait() {
        WaitDialog waitDialog = new WaitDialog(getContext());
        waitDialog.setNotDismiss();
        waitDialog.show();
        return waitDialog;
    }
    private Timer timer;

    public void countdown() {
        if (null == timer) timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (countdown.getValue() == 0) {
                    timer.cancel();
                    timer = null;
                    return;
                }
                countdown.postValue(countdown.getValue() - 1);
            }
        }, 1000, 1000);

    }

    @Override
    protected void releaseRes() {
        super.releaseRes();
        if (null != timer) {
            timer.cancel();
            timer = null;
        }
    }

    /**
     * 检查验证码并注册
     */
    public void checkVerificationCode(String verificationCode, int usedFor) {
        Parameter parameter = getParameter(verificationCode, usedFor);
        WaitDialog waitDialog = showWait();
        N.API(CRIMService.class).checkVerificationCode(parameter.buildJsonBody()).map(CRIMService.turn(HashMap.class)).compose(N.IOMain()).subscribe(new NetObserver<HashMap>(getContext()) {
            @Override
            public void onComplete() {
                super.onComplete();
                waitDialog.dismiss();
            }

            @Override
            public void onSuccess(HashMap o) {
                LoginVM.this.verificationCode = verificationCode;
                getIView().succ("checkVerificationCode");
            }

            @Override
            protected void onFailure(Throwable e) {
                getIView().err(e.getMessage());
            }
        });
    }

    public void resetPassword(String password) {
        Parameter parameter = getParameter(verificationCode, 2);
        //这里要把密码传入
        parameter.add("password", md5(password));
        WaitDialog waitDialog = showWait();
        N.API(CRIMService.class).resetPassword(parameter.buildJsonBody()).map(CRIMService.turn(HashMap.class)).compose(N.IOMain()).subscribe(new NetObserver<HashMap>(getContext()) {
            @Override
            public void onComplete() {
                super.onComplete();
                waitDialog.dismiss();
            }

            @Override
            public void onSuccess(HashMap o) {
                getIView().succ(null);
            }

            @Override
            protected void onFailure(Throwable e) {
                getIView().err(e.getMessage());
            }
        });
    }

    public void register() {
        Parameter parameter = new Parameter();
        parameter.add("verifyCode",verificationCode);
        parameter.add("platform", Platform.ANDROID);
        parameter.add("autoLogin", true);

        Map<String,String> user=new HashMap<>();
        user.put("password",md5(pwd.getValue()));
        user.put("nickname",nickName.getValue());
        user.put("areaCode", "+86");
        user.put("phoneNumber", account.getValue());
        parameter.add("user",user);

        WaitDialog waitDialog = showWait();
        N.API(CRIMService.class).register(parameter.buildJsonBody()).map(CRIMService.turn(LoginCertificate.class)).compose(N.IOMain()).subscribe(new NetObserver<LoginCertificate>(context.get()) {
            @Override
            public void onComplete() {
                super.onComplete();
                waitDialog.dismiss();
            }

            @Override
            public void onSuccess(LoginCertificate o) {
//                setSelfInfo();
                o.cache(getContext());
                getIView().jump();
            }

            @Override
            protected void onFailure(Throwable e) {
                getIView().toast(e.getMessage());
            }
        });
    }

    ///设置个人信息
    public void setSelfInfo() {
        CRIMClient.getInstance().userInfoManager.setSelfInfo(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
            }

            @Override
            public void onSuccess(String data) {
            }
        }, nickName.getValue(), null, 0, 0, null, 0, null, null);
    }

    public interface ViewAction extends io.crim.android.ouicore.base.IView {
        ///跳转
        void jump();

        void err(String msg);

        void succ(Object o);

        void initDate();

    }
}
