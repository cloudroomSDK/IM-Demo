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
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.SPUtil;
import io.crim.android.ouicore.utils.SimpleTextWatcher;
import io.crim.android.ouicore.widget.SlideButton;

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
        refreshHttpsIgnore(loginSettingVM.HEAD.getValue());
        boolean httpsIgnore = SPUtil.get(BaseApp.inst()).getBoolean(Constant.HTTPS_IGNORE);
        view.btnIgnore.setCheckedWithAnimation(httpsIgnore);
        view.btnIgnore.setOnSlideButtonClickListener(new SlideButton.OnSlideButtonClickListener() {
            @Override
            public void onClicked(boolean isChecked) {
                SPUtil.get(BaseApp.inst()).setCache(Constant.HTTPS_IGNORE, isChecked);
            }
        });
        view.etBusinessServer.addTextChangedListener(new SimpleTextWatcher() {
            @Override
            public void afterTextChanged(Editable editable) {
                String value = view.etBusinessServer.getText().toString().trim();
                refreshHttpsIgnore(value);
                if (!TextUtils.isEmpty(value)) {
                    if (value.equals(getString(R.string.default_txt))) {
                        return;
                    }
                }
                SPUtil.get(BaseApp.inst()).setCache("APP_AUTH_URL", value);
                updateBusinessServerTxt(value);
            }
        });
        view.llDefault.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SPUtil.get(BaseApp.inst()).setCache("DEFAULT_IP", "");
                SPUtil.get(BaseApp.inst()).setCache("APP_AUTH_URL", "");
                view.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        restart();
                    }
                }, 500);
            }
        });
        updateBusinessServerTxt(Constant.getAppAuthUrl());
        view.llSave.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                restart();
            }
        });
    }

    private void refreshHttpsIgnore(String address) {
        if (address == null) {
            return;
        }
        if (address.startsWith("https")) {
            view.groupIgnore.setVisibility(View.VISIBLE);
        } else {
            view.groupIgnore.setVisibility(View.GONE);
            SPUtil.get(BaseApp.inst()).setCache(Constant.HTTPS_IGNORE, false);
        }
    }

    private void updateBusinessServerTxt(String server) {
        if (server != null && server.equals(Constant.getDefaultAppAuthUrl())) {
            loginSettingVM.HEAD.setValue(getString(R.string.default_txt));
        }
    }

    private void restart() {
        Intent intent = new Intent(BaseApp.inst(), SplashActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        BaseApp.inst().startActivity(intent);
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    public static class LoginSettingVM {
        public MutableLiveData<String> HEAD = new MutableLiveData<>(Constant.getAppAuthUrl());
    }
}
