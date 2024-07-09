package io.crim.android.demo.ui;

import android.content.Context;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import java.util.ArrayList;
import java.util.Arrays;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.crim.android.demo.R;
import io.crim.android.demo.databinding.DialogHttpConfigBinding;
import io.crim.android.ouicore.base.BaseDialog;

/**
 * Created by zjw on 2024/5/13.
 */
class HttpConfigDialog extends BaseDialog {

    private DialogHttpConfigBinding view;
    private HttpTypeSelectListener mListener;

    public HttpConfigDialog(@NonNull Context context) {
        super(context);
        initView();
    }

    public HttpConfigDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        initView();
    }

    protected HttpConfigDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        initView();
    }

    public interface HttpTypeSelectListener {
        void selectType(int type);
    }

    public HttpConfigDialog setSelectListener(HttpTypeSelectListener listener) {
        mListener = listener;
        return this;
    }

    public HttpConfigDialog setType(int type) {
        view.wheelType.setSeletion(type - 1);
        return this;
    }

    public void initView() {
        Window win = this.getWindow();
        win.requestFeature(Window.FEATURE_NO_TITLE);
        view= DialogHttpConfigBinding.inflate(getLayoutInflater());
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
        String[] array = getContext().getResources().getStringArray(R.array.datenc_types);
        ArrayList<String> list = new ArrayList<String>(Arrays.asList(array));
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
                int index = HttpConfigDialog.this.view.wheelType.selectedIndex;
                mListener.selectType(index);
                dismiss();
            }
        });
    }
}
