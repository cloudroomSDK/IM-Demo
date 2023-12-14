package io.crim.android.ouigroup.ui;

import android.os.Bundle;

import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.vm.GroupVM;
import io.crim.android.ouigroup.databinding.ActivityMemberPermissionBinding;

public class MemberPermissionActivity extends BaseActivity<GroupVM, ActivityMemberPermissionBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(GroupVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityMemberPermissionBinding.inflate(getLayoutInflater()));
        sink();
        initView();
        click();
    }

    private void click() {
        view.notViewMemberProfiles.setOnSlideButtonClickListener(isChecked -> {
            int status = isChecked ? 1 : 0;
            vm.setGroupLookMemberInfo(status, data -> {
                vm.groupsInfo.getValue().setLookMemberInfo(status);
                vm.groupsInfo.setValue(vm.groupsInfo.getValue());
            });
        });
        view.notAddMemberToFriend.setOnSlideButtonClickListener(isChecked -> {
            int status = isChecked ? 1 : 0;
            vm.setGroupApplyMemberFriend(status, data -> {
                vm.groupsInfo.getValue().setApplyMemberFriend(status);
                vm.groupsInfo.setValue(vm.groupsInfo.getValue());
            });
        });
    }

    private void initView() {
        vm.groupsInfo.observe(this, groupInfo -> {
            if (null == groupInfo) return;
            view.notViewMemberProfiles.post(() -> view.notViewMemberProfiles.setCheckedWithAnimation(groupInfo.getLookMemberInfo() != 0));
            view.notAddMemberToFriend.post(() -> view.notAddMemberToFriend.setCheckedWithAnimation(groupInfo.getApplyMemberFriend() != 0));
        });
    }
}
