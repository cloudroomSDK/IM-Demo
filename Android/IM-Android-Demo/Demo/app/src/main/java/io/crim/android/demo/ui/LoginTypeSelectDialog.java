package io.crim.android.demo.ui;

import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.util.ArrayList;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.crim.android.demo.databinding.DialogLoginTypeSelectBinding;
import io.crim.android.ouicore.base.BaseDialog;

/**
 * Created by zjw on 2023/9/26.
 */
public class LoginTypeSelectDialog extends BaseDialog {

    private Context mContext = null;
    private DialogLoginTypeSelectBinding view;
    private LoginTypeSelectListener mListener;

    public LoginTypeSelectDialog(@NonNull Context context) {
        super(context);
        mContext = context;
        initView();
    }

    public LoginTypeSelectDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        mContext = context;
        initView();
    }

    protected LoginTypeSelectDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        mContext = context;
        initView();
    }

    public interface LoginTypeSelectListener {
        void selectType(int type);
    }

    public LoginTypeSelectDialog setSelectListener(LoginTypeSelectListener listener) {
        mListener = listener;
        return this;
    }

    public LoginTypeSelectDialog setType(int type) {
        view.wheelType.setSeletion(type - 1);
        return this;
    }

    public void initView() {
        Window win = this.getWindow();
        win.requestFeature(Window.FEATURE_NO_TITLE);
        view = DialogLoginTypeSelectBinding.inflate(getLayoutInflater());
        setContentView(view.getRoot());

        win.getDecorView().setPadding(0, 0, 0, 0);
        WindowManager.LayoutParams lp = win.getAttributes();
        lp.width = WindowManager.LayoutParams.MATCH_PARENT;
        lp.height = WindowManager.LayoutParams.WRAP_CONTENT;
        lp.windowAnimations = io.crim.android.ouicore.R.style.dialog_animation;
        lp.gravity = Gravity.BOTTOM;
        win.setAttributes(lp);
        win.setBackgroundDrawableResource(android.R.color.transparent);

        view.wheelType.offset = 1;
        ArrayList<String> list = new ArrayList<String>();
        list.add("账号密码鉴权");
        list.add("动态Token鉴权");
        view.wheelType.setItems(list);
        view.wheelType.setSeletion(0);

        view.tvCancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                dismiss();
            }
        });
        view.tvConfirm.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int index = LoginTypeSelectDialog.this.view.wheelType.selectedIndex;
                mListener.selectType(index);
                dismiss();
            }
        });
    }
}
