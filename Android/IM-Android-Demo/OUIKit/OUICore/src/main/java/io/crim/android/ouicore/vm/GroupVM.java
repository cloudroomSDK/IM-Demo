package io.crim.android.ouicore.vm;

import android.text.TextUtils;

import com.alibaba.android.arouter.launcher.ARouter;
import com.github.promeg.pinyinhelper.Pinyin;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import androidx.annotation.NonNull;
import androidx.lifecycle.MutableLiveData;
import io.crim.android.ouicore.R;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.Easy;
import io.crim.android.ouicore.entity.ExGroupMemberInfo;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.im.IMBack;
import io.crim.android.ouicore.im.IMUtil;
import io.crim.android.ouicore.services.IConversationBridge;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.widget.CommonDialog;
import io.crim.android.ouicore.widget.WaitDialog;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.enums.GrpRole;
import io.crim.android.sdk.enums.GrpType;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.FriendInfo;
import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;

public class GroupVM extends SocialityVM {
    //禁言状态
    public MutableLiveData<Integer> muteStatus = new MutableLiveData<>(-1);
    public MutableLiveData<String> groupName = new MutableLiveData<>("");
    public MutableLiveData<GrpInfo> groupsInfo = new MutableLiveData<>();
    //当前用户是否是群主
    public MutableLiveData<Boolean> isGroupOwner = new MutableLiveData<>(true);
    //群所有成员
    public MutableLiveData<List<GroupMembersInfo>> groupMembers =
            new MutableLiveData<>(new ArrayList<>());
    //超级群成员分页加载
    public MutableLiveData<List<ExGroupMemberInfo>> superGroupMembers =
            new MutableLiveData<>(new ArrayList<>());
    //封装过的群成员 用于字母导航
    public MutableLiveData<List<ExGroupMemberInfo>> exGroupMembers =
            new MutableLiveData<>(new ArrayList<>());
    //群管理
    public MutableLiveData<List<ExGroupMemberInfo>> exGroupManagement =
            new MutableLiveData<>(new ArrayList<>());
    //群字母导航
    public MutableLiveData<List<String>> groupLetters = new MutableLiveData<>(new ArrayList<>());
    //封装过的好友信息 用于字母导航
    public String groupId;
    public MutableLiveData<List<FriendInfo>> selectedFriendInfo =
            new MutableLiveData<>(new ArrayList<>());
    public LoginCertificate loginCertificate;

    public List<FriendInfo> selectedFriendInfoV3=new ArrayList<>();

    public int page = 0;
    public int pageSize = 20;
    //已读Ids
    public List<String> hasReadIDList = new ArrayList<>();

    @Override
    protected void viewCreate() {
        super.viewCreate();
        loginCertificate = LoginCertificate.getCache(getContext());
    }

    /**
     * 获取群组信息
     */
    public void getGroupsInfo() {
        List<String> groupIds = new ArrayList<>(); // 群ID集合
        groupIds.add(groupId);
        CRIMClient.getInstance().groupManager.getSpecifiedGrpsInfo(new OnBase<List<GrpInfo>>() {
            @Override
            public void onError(int code, String error) {

            }

            @Override
            public void onSuccess(List<GrpInfo> data) {
                if (data.isEmpty()) return;
                groupsInfo.setValue(data.get(0));
                isGroupOwner.setValue(isOwner());
            }
        }, groupIds);
    }

    /**
     * 创建群组
     */
    public void createGroup(boolean isWordGroup) {
        WaitDialog waitDialog = new WaitDialog(getContext());
        waitDialog.setNotDismiss();
        waitDialog.show();

        LoginCertificate loginCertificate = LoginCertificate.getCache(getContext());
        List<String> memberUserIDs = new ArrayList<>();
        for (FriendInfo friendInfo : selectedFriendInfo.getValue()) {
            if (!friendInfo.getUserID().equals(loginCertificate.userID)) {
                memberUserIDs.add(friendInfo.getUserID());
            }
        }
        GrpInfo groupInfo = new GrpInfo();
        groupInfo.setGroupName(groupName.getValue());
        groupInfo.setGroupType(GrpType.WORK);
        CRIMClient.getInstance().groupManager.createGrp(memberUserIDs, null, groupInfo,
                loginCertificate.userID, new OnBase<GrpInfo>() {
            @Override
            public void onError(int code, String error) {
                waitDialog.dismiss();
                getIView().onError(error);
            }

            @Override
            public void onSuccess(GrpInfo data) {
                Easy.delete(MultipleChoiceVM.class);
                getIView().onSuccess(data);
                Common.UIHandler.postDelayed(waitDialog::dismiss, 200);
            }
        });
    }

    public void selectMute(int status) {
        muteStatus.setValue(status);
    }

    /**
     * 更新群信息
     *
     * @param groupID      群ID
     * @param groupName    群名称
     * @param faceURL      群icon
     * @param notification 群公告
     * @param introduction 群简介
     * @param ex           其他信息
     */
    public void UPDATEGroup(String groupID, String groupName, String faceURL, String notification
            , String introduction, String ex) {

        GrpInfo groupInfo=new GrpInfo();
        groupInfo.setGroupID(groupID);
        groupInfo.setGroupName(groupName);
        groupInfo.setFaceURL(faceURL);
        groupInfo.setNotification(notification);
        groupInfo.setIntroduction(introduction);
        groupInfo.setEx(ex);
        CRIMClient.getInstance().groupManager.setGrpInfo(groupInfo,new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().onError(error);
            }

            @Override
            public void onSuccess(String data) {
                if (!TextUtils.isEmpty(groupName)) {
                    Obs.newMessage(Constant.Event.UPDATE_GROUP_INFO, groupName);
                }
                if (!TextUtils.isEmpty(notification)) {
                    Obs.newMessage(Constant.Event.SET_GROUP_NOTIFICATION);
                }
                getIView().onSuccess(data);
                getGroupsInfo();
            }
        });
    }

    /**
     * 修改所在群的昵称
     *
     * @param gid           群ID
     * @param uid           群成员userID
     * @param groupNickname 群内显示名称
     */
    public void setGroupMemberNickname(String gid, String uid, String groupNickname) {
        CRIMClient.getInstance().groupManager.setGrpMemberNickname(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().onError(error);
            }

            @Override
            public void onSuccess(String data) {
                getIView().onSuccess(data);
                getGroupsInfo();

            }
        }, gid, uid, groupNickname);
    }

    public void getSuperGroupMemberList() {
        int start = page * pageSize;
        CRIMClient.getInstance().groupManager.getGrpMemberList(new OnBase<List<GroupMembersInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + "-" + code);
            }

            @Override
            public void onSuccess(List<GroupMembersInfo> data) {
                if (data.isEmpty()) return;
                superGroupMembers.getValue().addAll(getExGroupMemberInfos(data));
                buildOwnSelect();
                superGroupMembers.setValue(superGroupMembers.getValue());
            }

            private void buildOwnSelect() {
                ExGroupMemberInfo own = new ExGroupMemberInfo();
                own.groupMembersInfo = new GroupMembersInfo();
                own.groupMembersInfo.setUserID(BaseApp.inst().loginCertificate.userID);
                int index = superGroupMembers.getValue().indexOf(own);
                if (index > -1) {
                    ExGroupMemberInfo exGroupMemberInfo = superGroupMembers.getValue().get(index);
                    exGroupMemberInfo.isSelect = true;
                    exGroupMemberInfo.isEnabled = false;
                }
            }
        }, groupId, 0, start, pageSize);
    }

    @NonNull
    private List<ExGroupMemberInfo> getExGroupMemberInfos(List<GroupMembersInfo> data) {
        List<ExGroupMemberInfo> exGroupMemberInfos = new ArrayList<>();
        for (GroupMembersInfo datum : data) {
            ExGroupMemberInfo exGroupMemberInfo = new ExGroupMemberInfo();
            exGroupMemberInfo.groupMembersInfo = datum;
            exGroupMemberInfos.add(exGroupMemberInfo);
        }
        return exGroupMemberInfos;
    }

    List<String> getNextHasReadIds() {
        int start = page * pageSize;
        if (start > hasReadIDList.size()) return null;
        int end = start + pageSize;
        if (end > hasReadIDList.size()) end = hasReadIDList.size();
        return hasReadIDList.subList(start, end);
    }

    public void loadHasReadGroupMembersInfo() {
        List<String> ids = getNextHasReadIds();
        if (null == ids) return;
        getGroupMembersInfo(ids, true);
    }

    public void getGroupMembersInfo(List<String> ids, boolean isAddUp) {
        CRIMClient.getInstance().groupManager.getSpecifiedGrpMembersInfo(new OnBase<List<GroupMembersInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(List<GroupMembersInfo> data) {
                if (data.isEmpty()) return;
                if (isAddUp) {
                    superGroupMembers.getValue().addAll(getExGroupMemberInfos(data));
                    superGroupMembers.setValue(superGroupMembers.getValue());
                } else superGroupMembers.setValue(getExGroupMemberInfos(data));
            }
        }, groupId, ids);
    }
    public void getGroupMemberList(){
        getGroupMemberList(0);
    }
    /**
     * 获取群成员信息
     */
    public void getGroupMemberList(int count) {
        if (!superGroupMembers.getValue().isEmpty()) return; //表示走了超级大群逻辑

        exGroupMembers.getValue().clear();
        exGroupManagement.getValue().clear();
        groupLetters.getValue().clear();
        CRIMClient.getInstance().groupManager.getGrpMemberList(new OnBase<List<GroupMembersInfo>>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + "-" + code);
            }

            @Override
            public void onSuccess(List<GroupMembersInfo> data) {
                if (data.isEmpty()) return;
                groupMembers.setValue(data);

                List<ExGroupMemberInfo> exGroupMemberInfos = new ArrayList<>();
                List<ExGroupMemberInfo> otGroupMemberInfos = new ArrayList<>();
                for (GroupMembersInfo datum : data) {
                    ExGroupMemberInfo exGroupMemberInfo = new ExGroupMemberInfo();
                    exGroupMemberInfo.groupMembersInfo = datum;
                    if (datum.getRoleLevel() != GrpRole.MEMBER) {
                        //群管理单独存放
                        exGroupMemberInfo.sortLetter = "";
                        exGroupManagement.getValue().add(exGroupMemberInfo);
                        continue;
                    }
                    String nickName = "0";
                    if (!TextUtils.isEmpty(datum.getNickname())) nickName = datum.getNickname();
                    String letter = Pinyin.toPinyin(nickName.charAt(0));
                    letter = (letter.charAt(0) + "").trim().toUpperCase();
                    if (!Common.isAlpha(letter)) {
                        exGroupMemberInfo.sortLetter = "#";
                        otGroupMemberInfos.add(exGroupMemberInfo);
                    } else {
                        exGroupMemberInfo.sortLetter = letter;
                        exGroupMemberInfos.add(exGroupMemberInfo);
                    }
                    if (!groupLetters.getValue().contains(exGroupMemberInfo.sortLetter))
                        groupLetters.getValue().add(exGroupMemberInfo.sortLetter);
                }

                Collections.sort(groupLetters.getValue(), new LettersPinyinComparator());
                groupLetters.getValue().add(0, "↑");
                groupLetters.setValue(groupLetters.getValue());

                exGroupMembers.getValue().addAll(exGroupMemberInfos);
                exGroupMembers.getValue().addAll(otGroupMemberInfos);

                Collections.sort(exGroupMembers.getValue(), new PinyinComparator());
                Collections.sort(exGroupManagement.getValue(), (o1, o2) -> {
                    if (o2.groupMembersInfo.getRoleLevel() < o1.groupMembersInfo.getRoleLevel())
                        return 0;
                    return -1;
                });
                exGroupMembers.setValue(exGroupMembers.getValue());
            }
        }, groupId, 0, 0, 0);
    }

    /**
     * 邀请入群
     */
    public void inviteUserToGroup(List<FriendInfo> friendInfos) {
        List<String> userIds = new ArrayList<>();
        for (FriendInfo friendInfo : friendInfos) {
            userIds.add(friendInfo.getUserID());
        }
        CRIMClient.getInstance().groupManager
            .inviteUserToGrp(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error);
            }

            @Override
            public void onSuccess(String data) {
                getIView().toast(getContext().getString(io.crim.android.ouicore.R.string.Invitation_succeeded));
                getGroupMemberList();
                getIView().onSuccess(null);

                Obs.newMessage(Constant.Event.UPDATE_GROUP_INFO, groupName);
            }
        }, groupId, userIds, "com");
    }

    /**
     * 踢出群
     */
    public void kickGroupMember(List<FriendInfo> friendInfos) {
        List<String> userIds = new ArrayList<>();
        for (FriendInfo friendInfo : friendInfos) {
            userIds.add(friendInfo.getUserID());
        }
        CRIMClient.getInstance().groupManager.kickGrpMember(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error);
            }

            @Override
            public void onSuccess(String data) {
                getIView().toast(getContext().getString(io.crim.android.ouicore.R.string.kicked_out));
                getGroupMemberList();
                getIView().onSuccess(null);

                Obs.newMessage(Constant.Event.UPDATE_GROUP_INFO, groupName);
            }
        }, groupId, userIds, "");
    }

    /**
     * 获取userID在群里的ExGroupMemberInfo 对象
     *
     * @return
     */
    public ExGroupMemberInfo getOwnInGroup(String userID) {
        ExGroupMemberInfo memberInfo = new ExGroupMemberInfo();
        GroupMembersInfo groupMembersInfo = new GroupMembersInfo();
        groupMembersInfo.setUserID(userID);
        memberInfo.groupMembersInfo = groupMembersInfo;
        int index = exGroupManagement.getValue().indexOf(memberInfo);
        if (index != -1) return exGroupManagement.getValue().get(index);

        index = exGroupMembers.getValue().indexOf(memberInfo);
        if (index != -1) return exGroupMembers.getValue().get(index);
        return null;
    }


    public boolean isOwner() {
        GrpInfo groupInfo = groupsInfo.getValue();
        if (null == groupInfo) return false;
        return groupInfo.getOwnerUserID().equals(loginCertificate.userID);
    }

    public void getGroupMembersInfo(OnBase<List<GroupMembersInfo>> onBase, List<String> uIds) {
        CRIMClient.getInstance().groupManager.getSpecifiedGrpMembersInfo(onBase, groupId, uIds);
    }

    public void dissolveGroup() {
        CommonDialog commonDialog = new CommonDialog(getContext());
        commonDialog.show();
        commonDialog.getMainView().tips.setText(io.crim.android.ouicore.R.string.dissolve_tips);
        commonDialog.getMainView().cancel.setOnClickListener(v -> commonDialog.dismiss());
        commonDialog.getMainView().confirm.setOnClickListener(v -> CRIMClient.getInstance().groupManager.dismissGrp(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                getIView().toast(getContext().getString(io.crim.android.ouicore.R.string.dissolve_tips2));
                close(commonDialog);
            }
        }, groupId));

    }

    public void quitGroup() {
        CommonDialog commonDialog = new CommonDialog(getContext());
        commonDialog.show();
        commonDialog.getMainView().tips.setText(io.crim.android.ouicore.R.string.quit_group_tips);
        commonDialog.getMainView().cancel.setOnClickListener(v -> commonDialog.dismiss());
        commonDialog.getMainView().confirm.setOnClickListener(v -> CRIMClient.getInstance().groupManager.quitGrp(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                getIView().toast(getContext().getString(io.crim.android.ouicore.R.string.quit_group_tips2));
                close(commonDialog);

            }
        }, groupId));
    }

    public void transferGroupOwner(String uid, IMUtil.OnSuccessListener OnSuccessListener) {
        CRIMClient.getInstance().groupManager.transferGrpOwner(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                getGroupsInfo();
                OnSuccessListener.onSuccess(data);
            }
        }, groupId, uid);
    }


    private void close(CommonDialog commonDialog) {
        IConversationBridge iConversationBridge =
                (IConversationBridge) ARouter.getInstance().build(Routes.Service.CONVERSATION).navigation();
        iConversationBridge.deleteConversationFromLocalAndSvr(groupId);
        iConversationBridge.closeChatPage();
        commonDialog.dismiss();
        getIView().close();
    }

    public void changeGroupMute(boolean isChecked, IMUtil.OnSuccessListener OnSuccessListener) {
        CRIMClient.getInstance().groupManager.changeGrpMute(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                if (null != OnSuccessListener) OnSuccessListener.onSuccess(data);
            }
        }, groupId, isChecked);
    }

    public void setGroupVerification(int needVerification,
                                     IMUtil.OnSuccessListener OnSuccessListener) {
        CRIMClient.getInstance().groupManager.setGrpVerification(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                if (null != OnSuccessListener) OnSuccessListener.onSuccess(data);
            }
        }, groupId, needVerification);
    }

    public void setGroupLookMemberInfo(int status, IMUtil.OnSuccessListener OnSuccessListener) {
        CRIMClient.getInstance().groupManager.setGrpLookMemberInfo(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                if (null != OnSuccessListener) OnSuccessListener.onSuccess(data);
            }
        }, groupId, status);
    }

    public void setGroupApplyMemberFriend(int status, IMUtil.OnSuccessListener OnSuccessListener) {
        CRIMClient.getInstance().groupManager.setGrpApplyMemberFriend(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(String data) {
                if (null != OnSuccessListener) OnSuccessListener.onSuccess(data);
            }
        }, groupId, status);
    }

    public void changeGroupMemberMute(IMBack<String> imBack, String uid, long seconds) {
        CRIMClient.getInstance().groupManager.changeGrpMemberMute(imBack, groupId, uid,
                seconds);
    }

    public void setMemberMute(IMBack<String> imBack, String uid, long seconds) {
        int status = muteStatus.getValue();
        if (status == -1 || (status == 0 && seconds == 0)) {
            getIView().toast(BaseApp.inst().getString(R.string.mute_tips));
            return;
        }
        if (status == 1) {
            seconds = 60 * 10;
        }
        if (status == 2) {
            seconds = 60 * 60;
        }
        if (status == 3) {
            seconds = 60 * 60 * 12;
        }
        if (status == 4) {
            seconds = 60 * 60 * 24;
        }
        if (status == 5) {
            seconds = 0;
        }
        changeGroupMemberMute(imBack, uid, seconds);
    }
}
