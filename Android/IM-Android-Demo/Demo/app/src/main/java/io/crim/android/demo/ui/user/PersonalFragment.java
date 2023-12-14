package io.crim.android.demo.ui.user;

import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.alibaba.android.arouter.launcher.ARouter;

import java.util.Observable;
import java.util.Observer;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.demo.databinding.FragmentPersonalBinding;
import io.crim.android.demo.ui.login.LoginActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.OnDedrepClickListener;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.widget.CommonDialog;
import io.crim.android.ouicore.widget.WaitDialog;

public class PersonalFragment extends BaseFragment implements Observer {
    public FragmentPersonalBinding view;

    public static PersonalFragment newInstance() {

        Bundle args = new Bundle();

        PersonalFragment fragment = new PersonalFragment();
        fragment.setArguments(args);
        return fragment;
    }


    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        view = FragmentPersonalBinding.inflate(inflater);

        initView();
        listener();
        Obs.inst().addObserver(this);
        return view.getRoot();
    }

    private void initView() {
        view.avatar.load(BaseApp.inst().loginCertificate.faceURL);
        view.name.setText(BaseApp.inst().loginCertificate.nickName);
        view.userId.setText("ID：" + BaseApp.inst().loginCertificate.userID);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Obs.inst().deleteObserver(this);
    }

    private void listener() {
        view.accountSetting.setOnClickListener(v->{
            startActivity(new Intent(getActivity(),AccountSettingActivity.class));
        });
        view.aboutLy.setOnClickListener(v -> {
            startActivity(new Intent(getActivity(),AboutUsActivity.class));
        });
        view.personalInfo.setOnClickListener(new OnDedrepClickListener() {
            @Override
            public void click(View v) {
                startActivity(new Intent(getActivity(), PersonalInfoActivity.class));
            }
        });
        view.userId.setOnClickListener(v -> {
            Common.copy(BaseApp.inst().loginCertificate.userID);
            toast(getString(io.crim.android.ouicore.R.string.copy_succ));
        });
        view.qrCode.setOnClickListener(v -> {
            ARouter.getInstance().build(Routes.Group.SHARE_QRCODE).navigation();
        });
        view.quit.setOnClickListener(v -> {
            CommonDialog commonDialog = new CommonDialog(getActivity()).atShow();
            commonDialog.getMainView().tips.
                setText(io.crim.android.ouicore.R.string.quit_tips);
            commonDialog.getMainView().cancel.setOnClickListener(v2 -> commonDialog.dismiss());
            commonDialog.getMainView().confirm.setOnClickListener(v2 -> {
                commonDialog.dismiss();
                WaitDialog waitDialog = new WaitDialog(getActivity());
                waitDialog.show();
                CRIMClient.getInstance().logout(new OnBase<String>() {
                    @Override
                    public void onError(int code, String error) {
                        waitDialog.dismiss();
                        toast(error + code);
                    }

                    @Override
                    public void onSuccess(String data) {
                        waitDialog.dismiss();
                        IMUtil.logout((AppCompatActivity) getActivity(), LoginActivity.class);
                    }
                });

            });
        });
    }

    @Override
    public void update(Observable observable, Object o) {
        Obs.Msg message = (Obs.Msg) o;
        if (message.tag == Constant.Event.USER_INFO_UPDATE) {
            view.avatar.load(BaseApp.inst().loginCertificate.faceURL);
            view.name.setText(BaseApp.inst().loginCertificate.nickName);
        }
    }
}
