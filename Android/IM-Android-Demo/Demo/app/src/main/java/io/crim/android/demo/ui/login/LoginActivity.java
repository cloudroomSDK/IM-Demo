package io.crim.android.demo.ui.login;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import java.util.Timer;
import java.util.TimerTask;

import androidx.annotation.Nullable;
import io.crim.android.demo.R;
import io.crim.android.demo.databinding.ActivityLoginBinding;
import io.crim.android.demo.ui.LoginSettingActivity;
import io.crim.android.demo.ui.main.MainActivity;
import io.crim.android.demo.vm.LoginVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.widget.WaitDialog;

/**
 * Created by zjw on 2023/9/21.
 */
public class LoginActivity extends BaseActivity<LoginVM, ActivityLoginBinding> implements LoginVM.ViewAction {

    private WaitDialog waitDialog;
    public static final String FORM_LOGIN = "form_login";
    //验证码倒计时
    private int countdown = 60;
    private Timer timer;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(LoginVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityLoginBinding.inflate(getLayoutInflater()));
        if (Constant.getSdkServer().equals("sdk.cloudroom.com")) {
            view.tvVerCodeTip.setText("");
        } else {
            view.tvVerCodeTip.setText("私有云验证码用8888");
        }

        waitDialog = new WaitDialog(this);
        view.ivSetting.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                startActivity(new Intent(LoginActivity.this, LoginSettingActivity.class));
            }
        });
        view.tvLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String phone = LoginActivity.this.view.etPhone.getText().toString().trim();
                if (phone.length() == 11) {
                    waitDialog.show();
                    vm.login(vm.veriCode.getValue(), 3);
                } else {
                    toast("请输入11位手机号");
                }
            }
        });
        view.tvSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //正在倒计时中...不触发操作
                if (countdown != 60) return;
                String phone = LoginActivity.this.view.etPhone.getText().toString().trim();
                if (phone.length() == 11) {
                    vm.getVerificationCode(3);
                } else {
                    toast("请输入11位手机号");
                }
            }
        });
        view.tvPwdLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                waitDialog.show();
                vm.login(vm.veriCode.getValue(), 3);
            }
        });
        view.tvSwitchPwdLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                vm.phoneNum.setValue("");
                vm.veriCode.setValue("");
                view.codeLoginGroup.setVisibility(View.GONE);
                view.pwdLoginGroup.setVisibility(View.VISIBLE);
            }
        });
        view.tvSwitchCodeLogin.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                vm.account.setValue("");
                vm.pwd.setValue("");
                view.codeLoginGroup.setVisibility(View.VISIBLE);
                view.pwdLoginGroup.setVisibility(View.GONE);
            }
        });
        view.setLoginVM(vm);
    }

    @Override
    public void jump() {
        startActivity(new Intent(this, MainActivity.class).putExtra(FORM_LOGIN, true));
        waitDialog.dismiss();
        finish();
    }

    @Override
    public void err(String msg) {
        waitDialog.dismiss();
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void succ(Object o) {
        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                countdown--;
                runOnUiThread(new TimerTask() {
                    @Override
                    public void run() {
                        view.tvSend.setText(countdown + "s");
                        if (countdown <= 0) {
                            view.tvSend.setText(R.string.get_vc);
                            countdown = 60;
                            timer.cancel();
                            timer = null;
                        }
                    }
                });
            }
        }, 0, 1000);
    }

    @Override
    public void initDate() {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (null != timer) {
            timer.cancel();
            timer = null;
        }
    }
}
