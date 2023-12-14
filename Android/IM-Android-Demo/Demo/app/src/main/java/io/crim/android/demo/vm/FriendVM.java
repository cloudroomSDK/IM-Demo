package io.crim.android.demo.vm;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import androidx.lifecycle.MutableLiveData;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.widget.WaitDialog;

public class FriendVM extends BaseViewModel {
    public MutableLiveData<List<UserInfo>> friendsInfo = new MutableLiveData<>(new ArrayList<>());
    public MutableLiveData<List<UserInfo>> blackListUser = new MutableLiveData<>(new ArrayList<>());
    public WaitDialog waitDialog;


    @Override
    protected void viewCreate() {
        super.viewCreate();
        waitDialog = new WaitDialog(getContext());
    }


    public void addBlacklist(String uid) {
        waitDialog.show();
        CRIMClient.getInstance().friendshipManager.addBlacklist(new OnCallBack<String>() {
            @Override
            public void onSuccess(String data) {
                waitDialog.dismiss();
                UserInfo userInfo = new UserInfo();
                userInfo.setUserID(uid);
                blackListUser.getValue().add(userInfo);
                blackListUser.setValue(blackListUser.getValue());
            }
        }, uid);

    }

    public void removeBlacklist(String uid) {
        waitDialog.show();
        CRIMClient.getInstance().friendshipManager.removeBlacklist(new OnCallBack<String>() {
            @Override
            public void onSuccess(String data) {
                waitDialog.dismiss();
                Iterator<UserInfo> iterator = blackListUser.getValue().iterator();
                while (iterator.hasNext()) {
                    if (iterator.next().getUserID().equals(uid))
                        iterator.remove();
                }
                blackListUser.setValue(blackListUser.getValue());
            }
        }, uid);
    }

    abstract class OnCallBack<T> implements OnBase<T> {

        @Override
        public void onError(int code, String error) {
            waitDialog.dismiss();
            getIView().toast(error + code);
        }
    }

    public void getFriendInfo(String... ids) {
        waitDialog.show();
        List<String> uids = new ArrayList<>();
        uids.add(ids[0]);
        CRIMClient.getInstance().friendshipManager.getSpecifiedFriendsInfo(new OnCallBack<List<UserInfo>>() {
            @Override
            public void onSuccess(List<UserInfo> data) {
                waitDialog.dismiss();
                if (data.isEmpty()) return;
                friendsInfo.setValue(data);
            }
        }, uids);
    }

    public void getBlacklist() {
        waitDialog.show();
        CRIMClient.getInstance().friendshipManager.getBlackList(new OnCallBack<List<UserInfo>>() {

            @Override
            public void onSuccess(List<UserInfo> data) {
                waitDialog.dismiss();
                blackListUser.setValue(null == data ? new ArrayList<>() : data);
            }
        });
    }

    /**
     * 移除好友
     *
     * @param uid
     */
    public void deleteFriend(String uid) {
        CRIMClient.getInstance().friendshipManager.deleteFriend(new OnBase<String>() {
            @Override
            public void onError(int code, String error) {
                getIView().toast(error);
            }

            @Override
            public void onSuccess(String data) {
                getIView().toast(getContext().getString(io.crim.android.ouicore.R.string.delete_friend));
                getIView().onSuccess(data);
            }
        }, uid);
    }
}
