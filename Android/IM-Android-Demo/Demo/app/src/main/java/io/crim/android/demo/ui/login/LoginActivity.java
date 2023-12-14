package io.crim.android.demo.ui.login;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import java.util.Timer;
import java.util.TimerTask;

import androidx.annotation.Nullable;
import io.crim.android.demo.R;
import io.crim.android.demo.databinding.ActivityLogin2Binding;
import io.crim.android.demo.ui.LoginSettingActivity;
import io.crim.android.demo.ui.main.MainActivity;
import io.crim.android.demo.vm.LoginVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.widget.WaitDialog;

/**
 * Created by zjw on 2023/9/21.
 */
public class LoginActivity extends BaseActivity<LoginVM, ActivityLogin2Binding>implements LoginVM.ViewAction {

    private WaitDialog waitDialog;
    public static final String FORM_LOGIN = "form_login";
    //验证码倒计时
    private int countdown = 60;
    private Timer timer;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        bindVM(LoginVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityLogin2Binding.inflate(getLayoutInflater()));
        if (Constant.getSdkServer().equals("sdk.cloudroom.com")){
            view.tvVerCodeTip.setVisibility(View.GONE);
        }else {
            view.tvVerCodeTip.setVisibility(View.VISIBLE);
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
                waitDialog.show();
                vm.login(vm.pwd.getValue(),3);
            }
        });
        view.tvSend.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //正在倒计时中...不触发操作
                if (countdown != 60) return;
                vm.getVerificationCode(3);
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
