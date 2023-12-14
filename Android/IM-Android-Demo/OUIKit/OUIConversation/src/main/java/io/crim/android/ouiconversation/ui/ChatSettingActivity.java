package io.crim.android.ouiconversation.ui;


import android.content.Intent;
import android.os.Bundle;

import com.alibaba.android.arouter.core.LogisticsCenter;
import com.alibaba.android.arouter.facade.Postcard;
import com.alibaba.android.arouter.launcher.ARouter;

import java.util.ArrayList;
import java.util.List;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.Opt;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.ouiconversation.databinding.ActivityChatSettingBinding;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.ouicore.widget.BottomPopDialog;
import io.crim.android.ouicore.widget.CommonDialog;

public class ChatSettingActivity extends BaseActivity<ChatVM, ActivityChatSettingBinding> implements ChatVM.ViewAction {

    ContactListVM contactListVM = new ContactListVM();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(ChatVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityChatSettingBinding.inflate(getLayoutInflater()));
        sink();

        initView();
        click();
    }

    private ActivityResultLauncher<Intent> personDetailLauncher =
        registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> {
        if (result.getResultCode() == RESULT_OK) {
            setResult(RESULT_OK);
            finish();
        }
    });

    private void click() {
        view.addChat.setOnClickListener(v -> {
            BottomPopDialog dialog = new BottomPopDialog(this);
            dialog.show();
            dialog.getMainView().menu3.setOnClickListener(v1 -> dialog.dismiss());
            dialog.getMainView().menu1.setText(io.crim.android.ouicore.R.string.general_group);
            dialog.getMainView().menu2.setText(io.crim.android.ouicore.R.string.work_group);

            dialog.getMainView().menu1.setOnClickListener(v1 -> {
                dialog.dismiss();
                ARouter.getInstance().build(Routes.Group.CREATE_GROUP).withString(Constant.K_ID,
                    vm.userID).navigation();
            });
            dialog.getMainView().menu2.setOnClickListener(v1 -> {
                dialog.dismiss();
                ARouter.getInstance().build(Routes.Group.CREATE_GROUP).withString(Constant.K_ID,
                    vm.userID).withBoolean(Constant.K_RESULT, true).navigation();
            });
        });
        view.picture.setOnClickListener(v -> {

        });
        view.video.setOnClickListener(v -> {
        });
        view.file.setOnClickListener(v ->{});

        view.readVanish.setOnSlideButtonClickListener(isChecked -> {
            CRIMClient.getInstance().conversationManager.setConversationPrivateChat(new OnBase<String>() {
                @Override
                public void onError(int code, String error) {
                    toast(error + code);
                    view.readVanish.setCheckedWithAnimation(!isChecked);
                }

                @Override
                public void onSuccess(String data) {
                    view.readVanish.setCheckedWithAnimation(isChecked);
                }
            }, vm.conversationInfo.getValue().getConversationID(), isChecked);
        });
        view.topSlideButton.setOnSlideButtonClickListener(is -> {
            contactListVM.pinConversation(vm.conversationInfo.getValue(), is);
        });
        view.searchChat.setOnClickListener(v -> {

        });
        view.chatbg.setOnClickListener(view1 -> {
            startActivity(new Intent(this, SetChatBgActivity.class));
        });

        view.noDisturb.setOnSlideButtonClickListener(is -> {
            vm.setConversationRecvMessageOpt(is ? Opt.ReceiveNotNotifyMessage : Opt.NORMAL,
                vm.conversationInfo.getValue().getConversationID());
        });
        view.user.setOnClickListener(v -> {
            Postcard postcard = ARouter.getInstance().build(Routes.Main.PERSON_DETAIL);
            LogisticsCenter.completion(postcard);
            personDetailLauncher.launch(new Intent(this, postcard.getDestination()).putExtra(Constant.K_ID, vm.userID).putExtra(Constant.K_RESULT, true));
        });
        view.clearRecord.setOnClickListener(v -> {
            CommonDialog commonDialog = new CommonDialog(this);
            commonDialog.show();
            commonDialog.getMainView().tips.setText(io.crim.android.ouicore.R.string.clear_chat_tips);
            commonDialog.getMainView().cancel.setOnClickListener(view1 -> commonDialog.dismiss());
            commonDialog.getMainView().confirm.setOnClickListener(view1 -> {
                commonDialog.dismiss();
                vm.clearCHistory(vm.conversationID);
            });
        });
    }

    private void initView() {
        view.readVanish.setCheckedWithAnimation(vm.conversationInfo.getValue().isPrivateChat());

        vm.notDisturbStatus.observe(this, integer -> {
            view.noDisturb.post(() -> view.noDisturb.setCheckedWithAnimation(integer == 2));
        });
        vm.conversationInfo.observe(this, conversationInfo -> {
            view.topSlideButton.post(() -> view.topSlideButton.setCheckedWithAnimation(conversationInfo.isPinned()));
        });

        List<String> uid = new ArrayList<>();
        uid.add(vm.userID);
        CRIMClient.getInstance().userInfoManager.getUsersInfo(new OnBase<List<UserInfo>>() {
            @Override
            public void onError(int code, String error) {
                toast(error + code);
            }

            @Override
            public void onSuccess(List<UserInfo> data) {
                if (data.isEmpty()) return;
                view.avatar.load(data.get(0).getFaceURL());
                view.userName.setText(data.get(0).getNickname());
            }
        }, uid);
    }

    @Override
    public void scrollToPosition(int position) {

    }

    @Override
    public void closePage() {

    }
}
