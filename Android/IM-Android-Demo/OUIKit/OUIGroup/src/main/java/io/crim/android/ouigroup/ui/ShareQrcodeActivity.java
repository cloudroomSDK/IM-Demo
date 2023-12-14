package io.crim.android.ouigroup.ui;


import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.View;

import com.alibaba.android.arouter.facade.annotation.Route;
import com.yzq.zxinglibrary.encode.CodeCreator;

import io.crim.android.ouicore.base.BaseActivity;
import io.crim.android.ouicore.entity.LoginCertificate;
import io.crim.android.ouicore.utils.Common;
import io.crim.android.ouicore.utils.Constant;
import io.crim.android.ouicore.utils.Routes;
import io.crim.android.ouicore.vm.GroupVM;
import io.crim.android.ouigroup.R;
import io.crim.android.ouigroup.databinding.ActivityGroupQrCodeBinding;
import io.crim.android.sdk.models.GrpInfo;

@Route(path = Routes.Group.SHARE_QRCODE)
public class ShareQrcodeActivity extends BaseActivity<GroupVM, ActivityGroupQrCodeBinding> {

    public static final String IS_QRCODE = "is_qrcode";
    private Bitmap qrCodeBitmap;
    private String tips;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        bindVMByCache(GroupVM.class);
        super.onCreate(savedInstanceState);
        bindViewDataBinding(ActivityGroupQrCodeBinding.inflate(getLayoutInflater()));
        boolean isQrcode = getIntent().getBooleanExtra(IS_QRCODE, true);
        String shareContent;
        if (null == vm) {
            //这里表示个人二维码分享 个人信息复制给群信息用于显示
            bindVM(GroupVM.class);
            LoginCertificate loginCertificate = LoginCertificate.getCache(this);
            if (loginCertificate != null) {
                shareContent = Constant.QR.QR_ADD_FRIEND + "/" + loginCertificate.userID;
                GrpInfo groupInfo = new GrpInfo();
                groupInfo.setGroupName(loginCertificate.nickName);
                groupInfo.setFaceURL(loginCertificate.faceURL);
                vm.groupsInfo.setValue(groupInfo);
            } else {
                shareContent = "";
            }
            tips = getString(io.crim.android.ouicore.R.string.qr_tips2);
            view.title.setText(io.crim.android.ouicore.R.string.qr_code);
        } else {
            shareContent = Constant.QR.QR_JOIN_GROUP + "/" + vm.groupId;
            if (isQrcode) {
                view.title.setText(io.crim.android.ouicore.R.string.group_qrcode);
                tips = getString(io.crim.android.ouicore.R.string.share_group_tips2);
            } else {
                view.title.setText(R.string.group_id);
                tips = getString(io.crim.android.ouicore.R.string.share_group_tips1);
            }
        }
        sink();
        view.setGroupVM(vm);
        view.tips.setText(tips);
        if (isQrcode) {
            bindData(shareContent);
        } else {
            view.qrCodeRl.setVisibility(View.GONE);
            view.groupId.setVisibility(View.VISIBLE);
            view.copy.setOnClickListener(v -> {
                Common.copy(vm.groupId);
                toast(getString(io.crim.android.ouicore.R.string.copy_succ));
            });
        }
        vm.groupsInfo.observe(this, groupInfo -> {
            bindData(shareContent);
        });
    }

    private void bindData(String shareContent) {
        try {
            view.avatar.load(vm.groupsInfo.getValue().getFaceURL());
            qrCodeBitmap = CodeCreator.createQRCode(shareContent, Common.dp2px(182),
                Common.dp2px(182), null);
            view.qrCode.setImageBitmap(qrCodeBitmap);
        } catch (Exception E) {
            E.printStackTrace();
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (null != qrCodeBitmap)
            qrCodeBitmap.recycle();
    }
}
