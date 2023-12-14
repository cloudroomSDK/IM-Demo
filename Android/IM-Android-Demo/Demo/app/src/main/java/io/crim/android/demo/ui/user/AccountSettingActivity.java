package io.crim.android.demo.ui.user;

import android.content.Intent;
import android.os.Bundle;

import io.crim.android.demo.databinding.ActivityAccountSettingBinding;
import io.crim.android.demo.vm.PersonalVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.widget.CommonDialog;
import io.crim.android.ouicore.widget.WaitDialog;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;

public class AccountSettingActivity extends BaseActivity<PersonalVM, ActivityAccountSettingBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(PersonalVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityAccountSettingBinding.inflate(getLayoutInflater()));
        sink();
        vm.getSelfUserInfo();
        listener();
    }

    private void listener() {
        vm.userInfo.observe(this, extendUserInfo -> {
            if (null == extendUserInfo) return;
            view.slideButton.setCheckedWithAnimation(extendUserInfo.getGlobalRecvMsgOpt() == 2);
        });
        WaitDialog waitDialog = new WaitDialog(this);
        view.slideButton.setOnSlideButtonClickListener(isChecked -> {
            waitDialog.show();
            CRIMClient.getInstance().conversationManager.setGlobalRecvMsgOpt(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    waitDialog.dismiss();
                    toast(error + code);
                }

                @Override
                public void onSuccess(String data) {
                    waitDialog.dismiss();
                    view.slideButton.setCheckedWithAnimation(isChecked);
                }
            }, isChecked ? 2 : 0);
        });
        view.clearRecord.setOnClickListener(v -> {
            CommonDialog commonDialog = new CommonDialog(this);
            commonDialog.getMainView().tips.setText(io.crim.android.ouicore.R.string.clear_chat_all_record);
            commonDialog.getMainView().cancel.setOnClickListener(view1 -> commonDialog.dismiss());
            commonDialog.getMainView().confirm.setOnClickListener(view1 -> {
                commonDialog.dismiss();
                CRIMClient.getInstance().messageManager.deleteAllMsgFromLocalAndSvr(null);
                toast(getString(io.crim.android.ouicore.R.string.cleared));
            });
            commonDialog.show();
        });
        view.blackList
            .setOnClickListener(view1 -> {
                startActivity(new Intent(this, BlackListActivity.class));
            });
    }
}
