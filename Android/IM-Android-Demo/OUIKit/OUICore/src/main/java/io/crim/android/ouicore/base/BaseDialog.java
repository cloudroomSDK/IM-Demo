package io.crim.android.ouicore.base;

import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.Gravity;
import android.view.ViewGroup;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class BaseDialog extends Dialog implements DialogInterface {
    public BaseDialog(@NonNull Context context) {
        super(context);
    }

    public BaseDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
    }

    protected BaseDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
    }

    public void setGravityBottom(){
        WindowManager.LayoutParams params = getWindow().getAttributes();
        params.gravity = Gravity.BOTTOM;
        params.height = ViewGroup.LayoutParams.WRAP_CONTENT;
        params.width = ViewGroup.LayoutParams.MATCH_PARENT;
        params.windowAnimations = io.crim.android.ouicore.R.style.dialog_animation;
        getWindow().setAttributes(params);
        getWindow().setBackgroundDrawableResource(android.R.color.transparent);
    }

    @Override
    public void show() {
        try {
            if (!isShowing())
                super.show();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void dismiss() {
        try {
            if (isShowing())
                super.dismiss();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }
}
