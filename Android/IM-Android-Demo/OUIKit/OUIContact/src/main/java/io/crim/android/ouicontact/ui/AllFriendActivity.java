package io.crim.android.ouicontact.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;

import com.alibaba.android.arouter.core.LogisticsCenter;
import com.alibaba.android.arouter.facade.Postcard;
import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;

import java.util.ArrayList;
import java.util.List;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.ouicontact.databinding.ActivityAllFriendBinding;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.databinding.LayoutCommonDialogBinding;
import io.crim.android.ouicore.entity.ExUserInfo;
import io.crim.android.ouicore.ex.User;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.SocialityVM;
import io.crim.android.ouicore.widget.CommonDialog;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnMsgSendCallback;
import io.crim.android.sdk.models.CardElem;
import io.crim.android.sdk.models.FriendInfo;
import io.crim.android.sdk.models.Message;
import io.crim.android.sdk.models.OfflinePushInfo;

@Route(path = Routes.Contact.ALL_FRIEND)
public class AllFriendActivity extends BaseActivity<SocialityVM, ActivityAllFriendBinding> {

    private RecyclerViewAdapter<ExUserInfo, RecyclerView.ViewHolder> adapter;
    //从聊天跳转过来
    private boolean formChat;
    private String groupID = "";
    private String userID = "";
    //从推荐好友跳转过来 带的userinfo
    private User recommend;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVM(SocialityVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityAllFriendBinding.inflate(getLayoutInflater()));
        sink();
        formChat = getIntent().getBooleanExtra("formChat", false);
        userID = getIntent().getStringExtra("userID");
        groupID = getIntent().getStringExtra("groupID");
        recommend = (User) getIntent().getSerializableExtra("recommend");
        vm.getAllFriend();

        listener();
        initView();
    }

    private void initView() {
        if (formChat){
            view.tvTitle.setText("选择好友");
        }
        view.scrollView.fullScroll(View.FOCUS_DOWN);
        view.recyclerView.setLayoutManager(new LinearLayoutManager(this));
        adapter = new RecyclerViewAdapter<ExUserInfo, RecyclerView.ViewHolder>() {
            private int STICKY = 1;
            private int ITEM = 2;

            private String lastSticky = "";

            @Override
            public void setItems(List<ExUserInfo> items) {
                lastSticky = items.get(0).sortLetter;
                items.add(0, getExUserInfo());
                for (int i = 0; i < items.size(); i++) {
                    ExUserInfo userInfo = items.get(i);
                    if (!lastSticky.equals(userInfo.sortLetter)) {
                        lastSticky = userInfo.sortLetter;
                        items.add(i, getExUserInfo());
                    }
                }
                super.setItems(items);
            }

            @NonNull
            private ExUserInfo getExUserInfo() {
                ExUserInfo exUserInfo = new ExUserInfo();
                exUserInfo.sortLetter = lastSticky;
                exUserInfo.isSticky = true;
                return exUserInfo;
            }

            @Override
            public int getItemViewType(int position) {
                return getItems().get(position).isSticky ? STICKY : ITEM;
            }

            @NonNull
            @Override
            public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
                if (viewType == ITEM)
                    return new ViewHol.ItemViewHo(parent);

                return new ViewHol.StickyViewHo(parent);
            }

            @Override
            public void onBindView(@NonNull RecyclerView.ViewHolder holder, ExUserInfo data, int position) {
                if (getItemViewType(position) == ITEM) {
                    ViewHol.ItemViewHo itemViewHo = (ViewHol.ItemViewHo) holder;
                    FriendInfo friendInfo = data.userInfo.getFriendInfo();
                    itemViewHo.view.avatar.load(friendInfo.getFaceURL());
                    itemViewHo.view.nickName.setText(friendInfo.getNickname());
                    itemViewHo.view.select.setVisibility(View.GONE);
                    itemViewHo.view.getRoot().setOnClickListener(v -> {
                        if (null != recommend) {
                            CommonDialog commonDialog = new CommonDialog(AllFriendActivity.this);
                            commonDialog.show();
                            LayoutCommonDialogBinding mainView = commonDialog.getMainView();
                            mainView.tips.setText(String.format(getString(io.crim.android.ouicore.R.string.recommend_who)
                                , friendInfo.getNickname()));
                            mainView.cancel.setOnClickListener(v1 -> commonDialog.dismiss());
                            mainView.confirm.setOnClickListener(v1 -> {
                                commonDialog.dismiss();
                                sendCardMessage(friendInfo.getUserID(), null, recommend.key, recommend.getName(), recommend.getFaceUrl());
                            });
                            return;
                        }
                        if (formChat) {
                            sendChatWindow(friendInfo);
                            return;
                        }
                        ARouter.getInstance().build(Routes.Main.PERSON_DETAIL)
                            .withString(Constant.K_ID, friendInfo.getUserID()).navigation(AllFriendActivity.this, 1001);
                    });
                } else {
                    ViewHol.StickyViewHo stickyViewHo = (ViewHol.StickyViewHo) holder;
                    stickyViewHo.view.title.setText(data.sortLetter);
                }
            }
        };
        view.recyclerView.setAdapter(adapter);
    }

    private void sendChatWindow(FriendInfo friendInfo) {
        setResult(RESULT_OK, new Intent().putExtra(Constant.K_RESULT,
            GsonHel.toJson(friendInfo)));
        finish();
        /*CommonDialog commonDialog = new CommonDialog(AllFriendActivity.this);
        commonDialog.show();
        LayoutCommonDialogBinding mainView = commonDialog.getMainView();
        mainView.tips.setText(BaseApp.inst().getString(io.crim.android.ouicore.R.string.send_card_confirm));
        mainView.cancel.setOnClickListener(v1 -> commonDialog.dismiss());
        mainView.confirm.setOnClickListener(v1 -> {
            commonDialog.dismiss();
            setResult(RESULT_OK, new Intent().putExtra(Constant.K_RESULT,
                GsonHel.toJson(friendInfo)));
            finish();
        });*/
    }

    private void sendCardMessage(String recvUid, String recvGid, String recomUid, String recomName, String recomFaceUrl) {
        CardElem cardElem = new CardElem();
        cardElem.setUserID(recomUid);
        cardElem.setNickname(recomName);
        cardElem.setFaceURL(recomFaceUrl);
        Message message = CRIMClient.getInstance().messageManager.createCardMsg(cardElem);
        OfflinePushInfo offlinePushInfo = new OfflinePushInfo(); // 离线推送的消息备注；不为null
        CRIMClient.getInstance().messageManager.sendMsg(new OnMsgSendCallback() {
            @Override
            public void onError(int code, String error) {
                toast(error + code);
            }

            @Override
            public void onProgress(long progress) {
            }

            @Override
            public void onSuccess(Message message) {
                toast(AllFriendActivity.this.
                    getString(io.crim.android.ouicore.R.string.send_succ));
                finish();
            }
        }, message, recvUid, recvGid, offlinePushInfo);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode == RESULT_OK && requestCode == 1001) {
            vm.getAllFriend();
        }
    }

    private ActivityResultLauncher<Intent> searchFriendLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(),
        result -> {
            try {
                if (result.getResultCode() != RESULT_OK) return;

                String uid = result.getData().getStringExtra(Constant.K_ID);
                if (formChat) {
                    for (ExUserInfo item : adapter.getItems()) {
                        if (null != item.userInfo && item.userInfo.getUserID().equals(uid)) {
                            sendChatWindow(item.userInfo.getFriendInfo());
                            return;
                        }
                    }
                }
                ARouter.getInstance().build(Routes.Main.PERSON_DETAIL)
                    .withString(Constant.K_ID, uid)
                    .navigation();
            } catch (Exception ignored) {

            }
        });

    private void listener() {
        view.searchView.setOnClickListener(v ->
            {
                Postcard postcard = ARouter.getInstance().build(Routes.Contact.SEARCH_FRIENDS);
                LogisticsCenter.completion(postcard);
                searchFriendLauncher.launch(new Intent(this, postcard.getDestination()));
            }
        );
        vm.letters.observe(this, v -> {
            if (null == v || v.isEmpty()) return;
            StringBuilder letters = new StringBuilder();
            for (String s : v) {
                letters.append(s);
            }
            view.sortView.setLetters(letters.toString());
        });


        vm.exUserInfo.observe(this, v -> {
            if (null == v || v.isEmpty()) return;
            List<ExUserInfo> exUserInfos = new ArrayList<>(v);
            adapter.setItems(exUserInfos);
        });

        view.sortView.setOnLetterChangedListener((letter, position) -> {
            for (int i = 0; i < adapter.getItems().size(); i++) {
                ExUserInfo exUserInfo = adapter.getItems().get(i);
                if (!exUserInfo.isSticky)
                    continue;
                if (exUserInfo.sortLetter.equalsIgnoreCase(letter)) {
                    View viewByPosition = view.recyclerView.getLayoutManager().findViewByPosition(i);
                    if (viewByPosition != null) {
                        view.scrollView.smoothScrollTo(0, viewByPosition.getTop());
                    }
                    return;
                }
            }
        });

    }
}
