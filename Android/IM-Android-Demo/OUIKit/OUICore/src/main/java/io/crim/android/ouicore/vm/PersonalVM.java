package io.crim.android.ouicore.vm;

import java.util.ArrayList;
import java.util.List;

import androidx.lifecycle.MutableLiveData;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.widget.WaitDialog;

public class PersonalVM extends BaseViewModel {
    public WaitDialog waitDialog;
    public MutableLiveData<UserInfo> userInfo = new MutableLiveData<>();

    @Override
    protected void viewCreate() {
        super.viewCreate();
        waitDialog = new WaitDialog(getContext());
    }

    OnBase<String> callBack = new OnBase<String>() {
        @Override
        public void onError(int code, String error) {
            waitDialog.dismiss();
            getIView().toast(error + code);
        }

        @Override
        public void onSuccess(String data) {
            waitDialog.dismiss();
            userInfo.setValue(userInfo.getValue());

            BaseApp.inst().loginCertificate.nickName = userInfo.getValue().getNickname();
            BaseApp.inst().loginCertificate.faceURL = userInfo.getValue().getFaceURL();
            Obs.newMessage(Constant.Event.USER_INFO_UPDATE);
        }
    };

    public void getSelfUserInfo() {
        waitDialog.show();
        CRIMClient.getInstance().userInfoManager.getSelfInfo(new OnBase<UserInfo>() {
            @Override
            public void onError(int code, String error) {
                waitDialog.dismiss();
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(UserInfo data) {
                waitDialog.dismiss();
                userInfo.setValue(data);
            }
        });

    }

    public void getUserInfo(String id) {
        waitDialog.show();
        List<String> ids = new ArrayList<>();
        ids.add(id);
        CRIMClient.getInstance().userInfoManager.getUsersInfo(new OnBase<List<UserInfo>>() {
            @Override
            public void onError(int code, String error) {
                waitDialog.dismiss();
                getIView().toast(error + code);
            }

            @Override
            public void onSuccess(List<UserInfo> data) {
                waitDialog.dismiss();
                if (data.isEmpty()) return;
                userInfo.setValue(data.get(0));
            }
        }, ids);
    }

    public void setSelfInfo(String nickname, String faceURL, int gender, int appMangerLevel, String phoneNumber, long birth, String email, String ex) {
        waitDialog.show();
        CRIMClient.getInstance().userInfoManager.setSelfInfo(callBack, nickname, faceURL, gender, appMangerLevel, phoneNumber, birth, email, ex);
    }

    public void setNickname(String nickname) {
        userInfo.getValue().setNickname(nickname);
        setSelfInfo(nickname, null, 0, 0, null, 0, null, null);
    }

    public void setFaceURL(String faceURL) {
        userInfo.getValue().setFaceURL(faceURL);
        setSelfInfo(null, faceURL, 0, 0, null, 0, null, null);
    }

    public void setGender(int gender) {
        userInfo.getValue().setGender(gender);
        setSelfInfo(null, null, gender, 0, null, 0, null, null);
    }

    public void setBirthday(long birth) {
        userInfo.getValue().setBirth(birth);
        setSelfInfo(null, null, 0, 0, null, birth, null, null);
    }
}
