package io.crim.android.ouicontact.vm;

import android.util.Log;

import java.util.List;

import androidx.lifecycle.MutableLiveData;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.listener.OnFriendshipListener;
import io.crim.android.sdk.listener.OnGrpListener;
import io.crim.android.sdk.models.BlacklistInfo;
import io.crim.android.sdk.models.FriendInfo;
import io.crim.android.sdk.models.FriendReqInfo;
import io.crim.android.sdk.models.GroupMembersInfo;
import io.crim.android.sdk.models.GrpInfo;
import io.crim.android.sdk.models.GrpReqInfo;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.im.IMEvent;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.L;
import io.crim.android.ouicore.utils.SharedPreferencesUtil;

public class ContactVM extends BaseViewModel implements OnGrpListener, OnFriendshipListener {
    //群红点数量
    public State<Integer> groupDotNum = new State<>(0);
    //好友通知红点
    public State<Integer> friendDotNum = new State<>(0);
    //申请列表
    public MutableLiveData<List<GrpReqInfo>> groupApply = new MutableLiveData<>();
    //好友申请列表
    public MutableLiveData<List<FriendReqInfo>> friendApply = new MutableLiveData<>();
    //申请详情
    public MutableLiveData<GrpReqInfo> groupDetail = new MutableLiveData<>();
    //好友申请详情
    public MutableLiveData<FriendReqInfo> friendDetail = new MutableLiveData<>();
    //常联系的好友
    public MutableLiveData<UserInfo> frequentContacts = new MutableLiveData<>();


    @Override
    protected void viewCreate() {
        super.viewCreate();
        IMEvent.getInstance().addGroupListener(this);
        IMEvent.getInstance().addFriendListener(this);
        int requestNum = SharedPreferencesUtil.get(getContext()).getInteger(Constant.K_FRIEND_NUM);
        int groupNum = SharedPreferencesUtil.get(getContext()).getInteger(Constant.K_GROUP_NUM);
        friendDotNum.setValue(requestNum);
        groupDotNum.setValue(groupNum);
    }

    @Override
    protected void releaseRes() {
        super.releaseRes();
        IMEvent.getInstance().removeGroupListener(this);
        IMEvent.getInstance().removeFriendListener(this);
    }

    //个人申请列表
    public void getRecvFriendApplicationList() {
        CRIMClient.getInstance().friendshipManager.getFriendReqListAsRecipient(new OnBase<List<FriendReqInfo>>() {
            @Override
            public void onError(int code, String error) {

            }

            @Override
            public void onSuccess(List<FriendReqInfo> data) {
                if (data.isEmpty()) return;
                friendApply.setValue(data);
            }
        });
    }

    //群申请列表
    public void getRecvGroupApplicationList() {
        CRIMClient.getInstance().groupManager.getGrpReqListAsRecipient(new OnBase<List<GrpReqInfo>>() {
            @Override
            public void onError(int code, String error) {
                L.e("");
            }

            @Override
            public void onSuccess(List<GrpReqInfo> data) {
                L.e("");
                if (!data.isEmpty())
                    groupApply.setValue(data);
            }
        });
    }

    private OnBase onBase = new OnBase<String>() {
        @Override
        public void onError(int code, String error) {
            getIView().toast(error);
        }

        @Override
        public void onSuccess(String data) {
            if (null != groupDetail)
                getRecvGroupApplicationList();
            if (null != friendDetail)
                getRecvFriendApplicationList();
            getIView().onSuccess(null);
        }
    };

    //好友通过
    public void friendPass() {
        CRIMClient.getInstance().friendshipManager.acceptFriendReq(onBase, friendDetail.getValue().getFromUserID(), "");
    }


    //好友拒绝
    public void friendRefuse() {
        CRIMClient.getInstance().friendshipManager.refuseFriendReq(onBase, friendDetail.getValue().getFromUserID(), "");
    }

    //群通过
    public void pass() {
        CRIMClient.getInstance().groupManager.acceptGrpReq(onBase, groupDetail.getValue().getGroupID(), groupDetail.getValue().getUserID(), "");
    }

    //群拒绝
    public void refuse() {
        CRIMClient.getInstance().groupManager.refuseGrpReq(onBase,
            groupDetail.getValue().getGroupID(), groupDetail.getValue().getUserID(), "");
    }

    @Override
    public void onGrpReqAccepted(GrpReqInfo info) {
        cacheGroupDot(info);
    }

    @Override
    public void onGrpReqAdded(GrpReqInfo info) {
        cacheGroupDot(info);
    }

    @Override
    public void onGrpReqDeleted(GrpReqInfo info) {

    }

    @Override
    public void onGrpReqRejected(GrpReqInfo info) {
        cacheGroupDot(info);
    }

    @Override
    public void onGrpDismissed(GrpInfo info) {

    }

    @Override
    public void onGrpInfoChanged(GrpInfo info) {

    }

    @Override
    public void onGrpMemberAdded(GroupMembersInfo info) {

    }

    @Override
    public void onGrpMemberDeleted(GroupMembersInfo info) {

    }

    @Override
    public void onGrpMemberInfoChanged(GroupMembersInfo info) {

    }

    @Override
    public void onJoinedGrpAdded(GrpInfo info) {

    }

    private void cacheGroupDot(GrpReqInfo info) {
        if (info.getHandleResult() == 0
            && !info.getUserID().equals(BaseApp.inst().loginCertificate.userID)) {
            Log.d("eeeeeee","contact vm=cacheGroupDot=="+friendDotNum.getValue());
            groupDotNum.setValue(friendDotNum.getValue() + 1);
            SharedPreferencesUtil.get(getContext()).setCache(Constant.K_GROUP_NUM,
                groupDotNum.getValue());
        }
    }

    private void cacheFriendDot(FriendReqInfo u) {
        if (u.getHandleResult() == 0
            && !u.getFromUserID().equals(BaseApp.inst().loginCertificate.userID)) {
            Log.d("eeeeeee","contact vm=cacheFriendDot=="+friendDotNum.getValue());
            friendDotNum.setValue(friendDotNum.getValue() + 1);
            SharedPreferencesUtil.get(getContext()).setCache(Constant.K_FRIEND_NUM,
                friendDotNum.getValue());
        }
    }

    @Override
    public void onJoinedGrpDeleted(GrpInfo info) {

    }

    @Override
    public void onBlacklistAdded(BlacklistInfo u) {

    }

    @Override
    public void onBlacklistDeleted(BlacklistInfo u) {

    }

    @Override
    public void onFriendApplicationAccepted(FriendReqInfo u) {
        cacheFriendDot(u);
    }

    @Override
    public void onFriendApplicationAdded(FriendReqInfo u) {
        cacheFriendDot(u);
    }

    @Override
    public void onFriendApplicationDeleted(FriendReqInfo u) {
    }

    @Override
    public void onFriendApplicationRejected(FriendReqInfo u) {
        cacheFriendDot(u);
    }

    @Override
    public void onFriendInfoChanged(FriendInfo u) {

    }

    @Override
    public void onFriendAdded(FriendInfo u) {

    }

    @Override
    public void onFriendDeleted(FriendInfo u) {

    }
}
