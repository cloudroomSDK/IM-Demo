package io.crim.android.ouiconversation.widget;

import android.content.Context;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.DialogLocationConfirmBinding;
import io.crim.android.ouicore.base.BaseDialog;

/**
 * Created by zjw on 2024/2/21.
 */
public class LocationConfirmDialog extends BaseDialog {

    private OnLocationConfirmListener mListener = null;

    public interface OnLocationConfirmListener {
        void confirm(boolean confirm);
    }

    public LocationConfirmDialog(@NonNull Context context) {
        super(context);
        initView();
    }

    public LocationConfirmDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        initView();
    }

    protected LocationConfirmDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        initView();
    }

    private void initView() {
        DialogLocationConfirmBinding view = DialogLocationConfirmBinding.inflate(getLayoutInflater());
        setContentView(view.getRoot());

        view.getRoot().findViewById(R.id.send).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mListener != null) {
                    mListener.confirm(true);
                }
                dismiss();
            }
        });
        view.getRoot().findViewById(R.id.cancel).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (mListener != null) {
                    mListener.confirm(false);
                }
                dismiss();
            }
        });

        setGravityBottom();
    }

    public LocationConfirmDialog setClickListener(OnLocationConfirmListener listener) {
        mListener = listener;
        return this;
    }
}
