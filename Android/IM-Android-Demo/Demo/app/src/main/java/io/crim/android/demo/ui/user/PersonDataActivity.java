package io.crim.android.demo.ui.user;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;

import com.alibaba.android.arouter.launcher.ARouter;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.demo.databinding.ActivityPersonInfoBinding;
import io.crim.android.demo.ui.main.EditTextActivity;
import io.crim.android.demo.vm.FriendVM;
import io.crim.android.demo.vm.PersonalVM;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.databinding.LayoutCommonDialogBinding;
import io.crim.android.ouicore.ex.User;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.ouicore.widget.CommonDialog;
import io.crim.android.ouicore.widget.WaitDialog;

public class PersonDataActivity extends BaseActivity<PersonalVM, ActivityPersonInfoBinding> {

    private ChatVM chatVM;
    private FriendVM friendVM = new FriendVM();
    private WaitDialog waitDialog;
    private String uid;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(PersonalVM.class, true);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityPersonInfoBinding.inflate(getLayoutInflater()));
        sink();
        init();
        listener();
        uid = getIntent().getStringExtra(Constant.K_ID);
        if (TextUtils.isEmpty(uid)) {
            chatVM = BaseApp.inst().getVMByCache(ChatVM.class);
            uid = chatVM.userID;
        }
        vm.getUserInfo(uid);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (isFinishing()) removeCacheVM();
    }

    private void init() {
        waitDialog = new WaitDialog(this);
        friendVM.waitDialog = waitDialog;
        friendVM.setContext(this);
        friendVM.setIView(this);
        friendVM.getBlacklist();
    }

    @Override
    public void onSuccess(Object body) {
        super.onSuccess(body);
        final String cid = "single_" + uid;
        BaseApp.inst().getVMByCache(ContactListVM.class).deleteConversationAndDeleteAllMsg(cid);
        setResult(RESULT_OK);
        finish();
    }

    private void listener() {
        view.part.setOnClickListener(v -> {
            CommonDialog commonDialog = new CommonDialog(this);
            commonDialog.show();
            LayoutCommonDialogBinding mainView = commonDialog.getMainView();
            mainView.tips.setText(io.crim.android.ouicore.R.string.delete_friend_tips);
            mainView.cancel.setOnClickListener(v1 -> commonDialog.dismiss());
            mainView.confirm.setOnClickListener(v1 -> {
                commonDialog.dismiss();
                friendVM.deleteFriend(uid);
            });
        });
        view.recommend.setOnClickListener(v -> {
            UserInfo userInfo = vm.userInfo.val();
            if (userInfo != null) {
                User user = new User(userInfo.getUserID());
                user.setName(userInfo.getNickname());
                user.setFaceUrl(userInfo.getFaceURL());
                ARouter.getInstance().build(Routes.Contact.ALL_FRIEND)
                    .withSerializable("recommend",
                        user).navigation();
            }
        });
        view.moreData.setOnClickListener(v -> {
            startActivity(new Intent(this, MoreDataActivity.class));
        });
        view.remark.setOnClickListener(view -> {
            if (null == vm.userInfo.getValue()) return;
            String remark = "";
            try {
                remark = vm.userInfo.val().getFriendInfo().getRemark();
            } catch (Exception e) {
            }
            resultLauncher.launch(new Intent(this, EditTextActivity.class).putExtra(EditTextActivity.TITLE, getString(io.crim.android.ouicore.R.string.remark)).putExtra(EditTextActivity.INIT_TXT, remark));
        });
        friendVM.blackListUser.observe(this, userInfos -> {
            boolean isCon = false;
            for (UserInfo userInfo : userInfos) {
                if (userInfo.getUserID().equals(uid)) {
                    isCon = true;
                    break;
                }
            }
            boolean finalIsCon = isCon;
            view.slideButton.post(() -> view.slideButton.setCheckedWithAnimation(finalIsCon));
        });
        view.joinBlackList.setOnClickListener(v -> {
            if (view.slideButton.isChecked()) friendVM.removeBlacklist(uid);
            else {
                addBlackList();
            }
        });
        view.slideButton.setOnSlideButtonClickListener(isChecked -> {
            if (isChecked) addBlackList();
            else friendVM.removeBlacklist(uid);
        });
    }

    private void addBlackList() {
        CommonDialog commonDialog = new CommonDialog(this);
        commonDialog.setCanceledOnTouchOutside(false);
        commonDialog.setCancelable(false);
        commonDialog.getMainView().tips.setText("确认对" + vm.userInfo.val().getNickname() + "拉黑吗？");
        commonDialog.getMainView().cancel.setOnClickListener(v -> {
            commonDialog.dismiss();
            friendVM.blackListUser.setValue(friendVM.blackListUser.getValue());
        });
        commonDialog.getMainView().confirm.setOnClickListener(v -> {
            commonDialog.dismiss();
            friendVM.addBlacklist(uid);
        });
        commonDialog.show();
    }

    private ActivityResultLauncher<Intent> resultLauncher =
        registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
            if (result.getResultCode() != Activity.RESULT_OK) return;
            String resultStr = result.getData().getStringExtra(Constant.K_RESULT);

            waitDialog.show();
            CRIMClient.getInstance().friendshipManager.setFriendRemark(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    waitDialog.dismiss();
                    toast(error + code);
                }

                @Override
                public void onSuccess(String data) {
                    waitDialog.dismiss();
                    vm.userInfo.val().setRemark(resultStr);
                    Obs.newMessage(Constant.Event.USER_INFO_UPDATE);
                }
            }, uid, resultStr);
        });
}
