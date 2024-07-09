package io.crim.android.demo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextUtils;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.lifecycle.MutableLiveData;
import io.crim.android.demo.R;
import io.crim.android.demo.SplashActivity;
import io.crim.android.demo.databinding.ActivityLoginSettingBinding;
import io.crim.android.ouicore.AccountConfig;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;
import io.crim.android.ouicore.utils.SimpleTextWatcher;

/**
 * Created by zjw on 2023/9/25.
 */
public class LoginSettingActivity extends BaseActivity<BaseViewModel, ActivityLoginSettingBinding> {

    private final LoginSettingVM loginSettingVM = new LoginSettingVM();

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityLoginSettingBinding.inflate(getLayoutInflater()));
        view.setLoginSettingVM(loginSettingVM);
        int loginType = SharedPreferencesUtil.get(BaseApp.inst()).getInteger("LOGIN_TYPE");
        refreshLoginTypeLayout(loginType);
        int httpType = SharedPreferencesUtil.get(BaseApp.inst()).getInteger("HTTP_TYPE");
        if (httpType < 1) {
            httpType = 1;
        }
        refreshHttpTypeLayout(httpType);
        view.llHttp.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                new HttpConfigDialog(LoginSettingActivity.this)
                    .setType(loginType)
                    .setSelectListener(new HttpConfigDialog.HttpTypeSelectListener() {
                        @Override
                        public void selectType(int type) {
                            SharedPreferencesUtil.get(BaseApp.inst()).setCache("HTTP_TYPE", type);
                            refreshHttpTypeLayout(type);
                            Constant.getProtocol();
                            Constant.saveUrl();
                        }
                    })
                    .show();
            }
        });
        view.llType.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new LoginTypeSelectDialog(LoginSettingActivity.this)
                    .setType(loginType)
                    .setSelectListener(new LoginTypeSelectDialog.LoginTypeSelectListener() {
                        @Override
                        public void selectType(int type) {
                            refreshLoginTypeLayout(type);
                            SharedPreferencesUtil.get(BaseApp.inst()).setCache("LOGIN_TYPE", type);
                        }
                    })
                    .show();
            }
        });
        view.etBusinessServer.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                String value = loginSettingVM.HEAD.getValue();
                if (value != null && value.contains(":")) {
                    toast("业务服务器无需填写端口号");
                } else {
                    SharedPreferencesUtil.get(BaseApp.inst()).setCache("DEFAULT_IP", loginSettingVM.HEAD.getValue());
                    Constant.saveUrl();
                }
            }
        });
        view.etServer.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("SERVER_IP", loginSettingVM.SERVER_IP.getValue());
                Constant.saveUrl();
            }
        });
        view.etAppID.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                String appID = loginSettingVM.APP_ID.getValue();
                if (appID.equals(getString(R.string.default_app_id)) || (!TextUtils.isEmpty(AccountConfig.APP_ID) && appID.equals(AccountConfig.APP_ID))) {
                    appID = "";
                }
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("APP_ID", appID);
                Constant.saveUrl();
            }
        });
        view.etSecret.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("APP_SECRET", loginSettingVM.APP_SECRET.getValue());
                Constant.saveUrl();
            }
        });
        view.etToken.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("LOGIN_TOKEN", loginSettingVM.LOGIN_TOKEN.getValue());
                Constant.saveUrl();
            }
        });
        view.llDefault.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("LOGIN_TYPE", 1);
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("DEFAULT_IP", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("SERVER_IP", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("APP_ID", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("APP_SECRET", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("LOGIN_TOKEN", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("APP_AUTH_URL", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("IM_API_URL", "");
                SharedPreferencesUtil.get(BaseApp.inst()).setCache("HTTP_TYPE", 1);
                view.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        restart();
                    }
                }, 500);
            }
        });
        if (Constant.getAppID().equals(AccountConfig.APP_ID)) {
            loginSettingVM.APP_ID.setValue(getString(R.string.default_app_id));
        }
        view.llSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                restart();
            }
        });
    }

    private void refreshHttpTypeLayout(int type) {
        view.tvHttpType.setText(getResources().getStringArray(R.array.datenc_types)[type - 1]);
    }

    private void refreshLoginTypeLayout(int type) {
        String typeStr = "";
        if (type == 2) {
            LoginSettingActivity.this.view.llID.setVisibility(View.GONE);
            LoginSettingActivity.this.view.llSecret.setVisibility(View.GONE);
            LoginSettingActivity.this.view.llToken.setVisibility(View.VISIBLE);
            typeStr = "动态Token鉴权";
//            SharedPreferencesUtil.get(BaseApp.inst()).setCache("LOGIN_TOKEN", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhcHBJRCI6Imp2eHh3bXpjaHQiLCJleHAiOjE3MTA0ODI4MTEsInNhbHQiOiJOZTlraWpQTUg4In0.zFCRoonPgOVBiujVklSzSwHM_J2iSj3j9_NZWJCLj-w");
        } else {
            typeStr = "账号密码鉴权";
            LoginSettingActivity.this.view.llID.setVisibility(View.VISIBLE);
            LoginSettingActivity.this.view.llSecret.setVisibility(View.VISIBLE);
            LoginSettingActivity.this.view.llToken.setVisibility(View.GONE);
        }
        LoginSettingActivity.this.view.tvType.setText(typeStr);
    }

    private void restart() {
        Intent intent = new Intent(BaseApp.inst(), SplashActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        BaseApp.inst().startActivity(intent);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    public static class LoginSettingVM {
        public MutableLiveData<String> HEAD = new MutableLiveData<>(Constant.getBusinessServer());
        public MutableLiveData<String> SERVER_IP = new MutableLiveData<>(Constant.getSdkServer());
        public MutableLiveData<String> APP_ID = new MutableLiveData<>(Constant.getAppID());
        public MutableLiveData<String> APP_SECRET = new MutableLiveData<>(Constant.getAppSecret());
        public MutableLiveData<String> LOGIN_TOKEN = new MutableLiveData<>(SharedPreferencesUtil.get(BaseApp.inst()).getString("LOGIN_TOKEN"));
    }
}
