package io.crim.android.demo.vm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import io.crim.android.sdk.listener.OnBase;
import io.crim.android.sdk.models.UserInfo;
import io.crim.android.demo.repository.CRIMService;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.BaseViewModel;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.net.RXRetrofit.NetObserver;
import io.crim.android.ouicore.net.RXRetrofit.Parameter;
import io.crim.android.ouicore.net.bage.GsonHel;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Obs;
import io.crim.android.ouicore.widget.WaitDialog;

public class PersonalVM extends BaseViewModel {
    public WaitDialog waitDialog;
    public State<UserInfo> userInfo = new State<>();

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
            userInfo.update();

            updateConfig(userInfo.val());
            Obs.newMessage(Constant.Event.USER_INFO_UPDATE);
        }
    };

    public void updateConfig(UserInfo userInfo) {
        LoginCertificate certificate = BaseApp.inst().loginCertificate;
        if (!userInfo.getUserID().equals(certificate.userID))return;

        certificate.nickName = userInfo.getNickname();
        certificate.faceURL = userInfo.getFaceURL();

        certificate.globalRecvMsgOpt = userInfo.getGlobalRecvMsgOpt();
        certificate.allowAddFriend = userInfo.getAllowAddFriend() == 1;
        certificate.allowBeep = userInfo.getAllowBeep() == 1;
        certificate.allowVibration = userInfo.getAllowVibration() == 1;

        BaseApp.inst().loginCertificate.cache(BaseApp.inst());
    }

    public void getSelfUserInfo() {
        waitDialog.show();
        getExtendUserInfo(BaseApp.inst().loginCertificate.userID);
    }

    private void getExtendUserInfo(String uid) {
        List<String> ids = new ArrayList<>();
        ids.add(uid);
        Parameter parameter = new Parameter().add("userIDs", ids);
        N.API(CRIMService.class).getUsersFullInfo(parameter.buildJsonBody()).map(CRIMService.turn(HashMap.class)).compose(N.IOMain()).subscribe(new NetObserver<HashMap>(getContext()) {
            @Override
            protected void onFailure(Throwable e) {
                getIView().toast(e.getMessage());
                waitDialog.dismiss();
            }

            @Override
            public void onSuccess(HashMap map) {
                waitDialog.dismiss();
                try {
                    ArrayList arrayList = (ArrayList) map.get("users");
                    if (null == arrayList || arrayList.isEmpty()) return;

                    UserInfo u = GsonHel.getGson().fromJson(arrayList.get(0).toString(),
                        UserInfo.class);
                    userInfo.setValue(u);

                    updateConfig(userInfo.val());
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });
    }

    public void getUserInfo(String id) {
        waitDialog.show();
       getExtendUserInfo(id);
    }

    public void setSelfInfo(Parameter param) {
        waitDialog.show();
        N.API(CRIMService.class)
            .updateUserInfo(param.buildJsonBody())
            .compose(N.IOMain()).map(CRIMService.turn(Object.class))

            .subscribe(new NetObserver<Object>(getContext()) {
                @Override
                protected void onFailure(Throwable e) {
                    callBack.onError(-1, e.getMessage());
                }

                @Override
                public void onSuccess(Object o) {
                    callBack.onSuccess("");
                }
            });
    }

    public void setNickname(String nickname) {
        userInfo.val().setNickname(nickname);
        setSelfInfo(new Parameter().add("nickname",nickname).add("userID", BaseApp.inst().loginCertificate.userID));
    }

    public void setFaceURL(String faceURL) {
        userInfo.val().setFaceURL(faceURL);
        setSelfInfo(new Parameter().add("faceURL",faceURL).add("userID", BaseApp.inst().loginCertificate.userID));
    }

    public void setGender(int gender) {
        userInfo.val().setGender(gender);
        setSelfInfo(new Parameter().add("gender",gender).add("userID", BaseApp.inst().loginCertificate.userID));
    }

    public void setBirthday(long birth) {
        userInfo.val().setBirth(birth);
        setSelfInfo(new Parameter().add("birth",birth).add("userID", BaseApp.inst().loginCertificate.userID));
    }

    public void setEmail(String email) {
        userInfo.val().setEmail(email);
        setSelfInfo(new Parameter().add("email",email).add("userID", BaseApp.inst().loginCertificate.userID));
    }

}
