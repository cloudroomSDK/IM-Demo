package io.crim.android.ouicontact.ui;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Toast;

import io.crim.android.ouicontact.R;
import io.crim.android.ouicontact.databinding.ActivityFriendRequestDetailBinding;
import io.crim.android.ouicontact.vm.ContactVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.SinkHelper;

public class FriendRequestDetailActivity extends BaseActivity<ContactVM, ActivityFriendRequestDetailBinding> {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(ContactVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityFriendRequestDetailBinding.inflate(getLayoutInflater()));
        setLightStatus();
        SinkHelper.get(this).setTranslucentStatus(view.getRoot());
        view.setContactVM(vm);
        view.avatar.load(vm.friendDetail.getValue().getFromFaceURL(),vm.friendDetail.getValue().getFromNickname());
    }

    @Override
    public void onSuccess(Object body) {
        super.onSuccess(body);
        finish();
        Toast.makeText(this, getString(io.crim.android.ouicore.R.string.send_succ), Toast.LENGTH_SHORT).show();
    }
}
