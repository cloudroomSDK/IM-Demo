package io.crim.android.ouicontact.ui;


import android.os.Bundle;

import io.crim.android.ouicontact.databinding.ActivityGroupNoticeDetailBinding;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouicore.base.BaseActivity;


public class GroupNoticeDetailActivity extends BaseActivity<ContactVM, ActivityGroupNoticeDetailBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(ContactVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityGroupNoticeDetailBinding.inflate(getLayoutInflater()));
        sink();
        view.setContactVM(vm);

        initView();
    }

    private void initView() {
        view.avatar.load(vm.groupDetail.getValue().getUserFaceURL());
    }

    @Override
    public void onSuccess(Object body) {
        finish();
    }
}
