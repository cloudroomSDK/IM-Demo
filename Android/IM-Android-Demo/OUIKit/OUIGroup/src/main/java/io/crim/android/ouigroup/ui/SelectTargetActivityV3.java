package io.crim.android.ouigroup.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;

import com.alibaba.android.arouter.core.LogisticsCenter;
import com.alibaba.android.arouter.facade.Postcard;
import com.alibaba.android.arouter.facade.annotation.Route;
import com.alibaba.android.arouter.launcher.ARouter;

import org.jetbrains.annotations.NotNull;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.adapter.ViewHol;
import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.databinding.LayoutPopSelectedFriendsBinding;
import io.crim.android.ouicore.entity.ExGroupMemberInfo;
import io.crim.android.ouicore.entity.ExUserInfo;
import io.crim.android.ouicore.entity.MsgConversation;
import io.crim.android.ouicore.ex.MultipleChoice;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.ContactListVM;
import io.crim.android.ouicore.vm.MultipleChoiceVM;
import io.crim.android.ouicore.vm.SelectTargetVM;
import io.crim.android.ouigroup.databinding.ActivityCreateGroupV3Binding;
import io.crim.android.sdk.enums.ConversationType;
import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.UserInfo;

@Route(path = Routes.Group.SELECT_TARGET)
public class SelectTargetActivityV3 extends BaseActivity<SelectTargetVM,
    ActivityCreateGroupV3Binding> {

    @NotNull("multipleChoiceVM cannot be empty")
    private MultipleChoiceVM multipleChoiceVM;
    private RecyclerViewAdapter<MsgConversation, ViewHol.ItemViewHo> adapterRect;
    private RecyclerViewAdapter<ExUserInfo, RecyclerView.ViewHolder> adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityCreateGroupV3Binding.inflate(getLayoutInflater()));
        bindVM(SelectTargetVM.class);
        multipleChoiceVM = Easy.find(MultipleChoiceVM.class);
        multipleChoiceVM.bindDataToView(view.bottom);
        multipleChoiceVM.showPopAllSelectFriends(view.bottom,
            LayoutPopSelectedFriendsBinding.inflate(getLayoutInflater()));
        multipleChoiceVM.submitTap(view.bottom.submit);
        vm.getAllFriend();
        initView();
        listener();
    }

    private void initView() {
        view.divider2.getRoot().setVisibility(!multipleChoiceVM.isCreateGroup || !multipleChoiceVM.invite ? View.GONE : View.VISIBLE);
        //创建群、邀请入群都不显示group
        view.group.setVisibility(multipleChoiceVM.isCreateGroup || multipleChoiceVM.invite ?
            View.GONE : View.VISIBLE);

        if (multipleChoiceVM.isCreateGroup) {
            view.title.setVisibility(View.VISIBLE);
            view.rlFriends.setVisibility(View.VISIBLE);
            view.scrollView.fullScroll(View.FOCUS_DOWN);
            view.recyclerView.setLayoutManager(new LinearLayoutManager(this));
            adapter = new RecyclerViewAdapter<ExUserInfo, RecyclerView.ViewHolder>() {
                private int STICKY = 1;
                private int ITEM = 2;

                private String lastSticky = "";

                @Override
                public void setItems(List<ExUserInfo> items) {
                    if (items.isEmpty()) return;
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
                public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent,
                                                                  int viewType) {
                    if (viewType == ITEM) return new ViewHol.ItemViewHo(parent);
                    return new ViewHol.StickyViewHo(parent);
                }

                @Override
                public void onBindView(@NonNull RecyclerView.ViewHolder holder, ExUserInfo data,
                                       int position) {
                    if (getItemViewType(position) == ITEM) {
                        ViewHol.ItemViewHo itemViewHo = (ViewHol.ItemViewHo) holder;
                        UserInfo friendInfo = data.userInfo;
                        itemViewHo.view.avatar.load(friendInfo.getFaceURL());
                        itemViewHo.view.nickName.setText(friendInfo.getNickname());
                        itemViewHo.view.select.setVisibility(View.VISIBLE);
                        itemViewHo.view.select.setChecked(data.isSelect);
                        if (!data.isEnabled) itemViewHo.view.item.setOnClickListener(null);
                        else itemViewHo.view.item.setOnClickListener(v -> {
                            data.isSelect = !data.isSelect;
                            notifyItemChanged(position);
//                            selected();
                            if (null != multipleChoiceVM) {
                                if (data.isSelect)
                                    multipleChoiceVM.addMetaData(data.userInfo.getUserID(),
                                        data.userInfo.getNickname(), data.userInfo.getFaceURL());
                                else
                                    multipleChoiceVM.removeMetaData(data.userInfo.getUserID());
                            }

                        });
                    } else {
                        ViewHol.StickyViewHo stickyViewHo = (ViewHol.StickyViewHo) holder;
                        stickyViewHo.view.title.setText(data.sortLetter);
                    }
                }
            };
            view.recyclerView.setAdapter(adapter);

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
                for (ExUserInfo exUserInfo : exUserInfos) {
                    ExGroupMemberInfo exGroupMemberInfo = new ExGroupMemberInfo();
                    exGroupMemberInfo.groupMembersInfo = new GroupMembersInfo();
                    exGroupMemberInfo.groupMembersInfo.setUserID(exUserInfo.userInfo.getUserID());

                    /*if (vm.exGroupMembers.getValue().contains(exGroupMemberInfo)
                        || vm.exGroupManagement.getValue().contains(exGroupMemberInfo)
                        || exUserInfo.userInfo.getUserID().equals(defSelectId)) {
                        exUserInfo.isEnabled = false;
                        exUserInfo.isSelect = true;
                    }*/

                    if (null != multipleChoiceVM) {
                        MultipleChoice data=new MultipleChoice();
                        data.key=exUserInfo.userInfo.getUserID();
                        exUserInfo.isSelect = multipleChoiceVM.contains(data);
                    }
                }
                adapter.setItems(exUserInfos);
            });
            view.sortView.setOnLetterChangedListener((letter, position) -> {
                for (int i = 0; i < adapter.getItems().size(); i++) {
                    ExUserInfo exUserInfo = adapter.getItems().get(i);
                    if (!exUserInfo.isSticky) continue;
                    if (exUserInfo.sortLetter.equalsIgnoreCase(letter)) {
                        View viewByPosition =
                            view.recyclerView.getLayoutManager().findViewByPosition(i);
                        if (viewByPosition != null) {
                            view.scrollView.smoothScrollTo(0, viewByPosition.getTop());
                        }
                        return;
                    }
                }
            });
        } else {
            view.title.setVisibility(View.GONE);
            view.rlFriends.setVisibility(View.GONE);
            view.recentContact.setVisibility(View.VISIBLE);
            view.recyclerViewRect.setLayoutManager(new LinearLayoutManager(this));
            view.recyclerViewRect.setAdapter(adapterRect = new RecyclerViewAdapter<MsgConversation,
                ViewHol.ItemViewHo>(ViewHol.ItemViewHo.class) {

                @Override
                public void onBindView(@NonNull ViewHol.ItemViewHo holder, MsgConversation data,
                                       int position) {
                    boolean isGroup =
                        data.conversationInfo.getConversationType() != ConversationType.SINGLE_CHAT;
                    String id = isGroup ? data.conversationInfo.getGroupID() :
                        data.conversationInfo.getUserID();
                    String faceURL = data.conversationInfo.getFaceURL();
                    String name = data.conversationInfo.getShowName();
                    holder.view.avatar.load(faceURL, isGroup, isGroup ? null : name);
                    holder.view.nickName.setText(name);

                    holder.view.select.setVisibility(View.VISIBLE);
                    if (multipleChoiceVM.contains(new MultipleChoice(id))) {
                        holder.view.select.setChecked(true);
                        for (MultipleChoice choice : multipleChoiceVM.metaData.val()) {
                            if (choice.key.equals(id)) {
                                holder.view.select.setEnabled(choice.isEnabled);
                            }
                        }
                    } else {
                        holder.view.select.setChecked(false);
                    }

                    holder.view.getRoot().setOnClickListener(v -> {
                        holder.view.select.setChecked(!holder.view.select.isChecked());

                        if (holder.view.select.isChecked()) {
                            MultipleChoice meta = new MultipleChoice(id);
                            meta.isGroup = isGroup;
                            meta.name = name;
                            meta.icon = faceURL;
                            multipleChoiceVM.metaData.val().add(meta);
                            multipleChoiceVM.metaData.update();
                        } else {
                            multipleChoiceVM.removeMetaData(id);
                        }
                    });
                }
            });
            ContactListVM vmByCache = BaseApp.inst().getVMByCache(ContactListVM.class);
            List<MsgConversation> conversations = new ArrayList<>();
            if (multipleChoiceVM.invite || multipleChoiceVM.isCreateGroup) {
                //只保留单聊
                for (MsgConversation msgConversation : vmByCache.conversations.getValue()) {
                    if (msgConversation.conversationInfo.getConversationType() == ConversationType.SINGLE_CHAT) {
                        conversations.add(msgConversation);
                    }
                }
            } else {
                conversations.addAll(vmByCache.conversations.getValue());
            }
            adapterRect.setItems(conversations);
        }
    }

    private void listener() {
        multipleChoiceVM.metaData.observe(this, v -> {
            if (null != adapterRect) adapterRect.notifyDataSetChanged();
            if (null != adapter) adapter.notifyDataSetChanged();
        });
        view.myFriends.setOnClickListener(v -> {
            ARouter.getInstance().build(Routes.Group.CREATE_GROUP).withBoolean(Constant.K_RESULT,
                true).withBoolean("fromSelectTarget", true).navigation();
        });

        view.group.setOnClickListener(v -> {
            startActivity(new Intent(this, AllGroupActivity.class));
        });
        view.searchView.setOnClickListener(v -> {
            List<String> ids = new ArrayList<>();
            for (MultipleChoice multipleChoice : multipleChoiceVM.metaData.val()) {
                ids.add(multipleChoice.key);
            }
            Postcard postcard = ARouter.getInstance().build(Routes.Contact.SEARCH_FRIENDS_GROUP);
            LogisticsCenter.completion(postcard);
            launcher.launch(new Intent(this, postcard.getDestination())
                .putExtra(Constant.K_RESULT, (Serializable) ids)
                .putExtra(Constant.IS_SELECT_FRIEND, multipleChoiceVM.isCreateGroup));
        });
    }

    /*private void selected() {
        selectMemberNum = getSelectNum();
        view.bottom.selectNum.setText(String.format(getString(io.crim.android.ouicore.R.string.selected_tips), selectMemberNum));
        view.bottom.submit.setText("确定");
        view.bottom.submit.setEnabled(selectMemberNum > 0);
    }*/

    /*private int getSelectNum() {
        List<FriendInfo> friendInfos = new ArrayList<>();
        vm.selectedFriendInfoV3.clear();
        int num = 0;
        for (ExUserInfo item : adapter.getItems()) {
            if (item.isSelect) {
                num++;
                friendInfos.add(item.userInfo.getFriendInfo());

                if (item.isEnabled)
                    vm.selectedFriendInfoV3.add(item.userInfo.getFriendInfo());
            }
        }
        vm.selectedFriendInfo.setValue(friendInfos);
        return num;
    }*/

    private ActivityResultLauncher<Intent> launcher =
        registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), v -> {
            if (v.getResultCode() != RESULT_OK) return;
            Intent intent = v.getData();
            Set<MultipleChoice> set =
                (Set<MultipleChoice>) intent.getSerializableExtra(Constant.K_RESULT);
            for (MultipleChoice data : set) {
                if (data.isSelect) {
                    if (!multipleChoiceVM.contains(data)) {
                        multipleChoiceVM.metaData.val().add(data);
                        multipleChoiceVM.metaData.update();
                    }
                } else {
                    multipleChoiceVM.removeMetaData(data.key);
                }
            }
        });

    @Override
    protected void fasterDestroy() {
        super.fasterDestroy();
        Easy.delete(MultipleChoiceVM.class);
    }
}
