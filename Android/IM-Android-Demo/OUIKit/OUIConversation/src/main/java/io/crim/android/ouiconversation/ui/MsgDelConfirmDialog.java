package io.crim.android.ouiconversation.ui;

import android.content.Context;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.constraintlayout.widget.ConstraintLayout;
import io.crim.android.ouiconversation.R;
import io.crim.android.ouiconversation.databinding.DialogMsgDelConfirmBinding;
import io.crim.android.ouicore.base.BaseDialog;

/**
 * Created by zjw on 2024/2/26.
 */
public class MsgDelConfirmDialog extends BaseDialog {

    private MsgDelListener mListener = null;

    public MsgDelConfirmDialog(@NonNull Context context) {
        super(context);
        initView();
    }

    public MsgDelConfirmDialog(@NonNull Context context, int themeResId) {
        super(context, themeResId);
        initView();
    }

    protected MsgDelConfirmDialog(@NonNull Context context, boolean cancelable, @Nullable OnCancelListener cancelListener) {
        super(context, cancelable, cancelListener);
        initView();
    }

    private void initView() {
        DialogMsgDelConfirmBinding msgDelConfirmBinding = DialogMsgDelConfirmBinding.inflate(getLayoutInflater());
        ConstraintLayout root = msgDelConfirmBinding.getRoot();
        setContentView(root);
        root.findViewById(R.id.confirm).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click(true);
                dismiss();
            }
        });
        root.findViewById(R.id.cancel).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                click(false);
                dismiss();
            }
        });
        setGravityBottom();
    }

    private void click(boolean coonfirm) {
        if (mListener != null) {
            mListener.confirm(coonfirm);
        }
    }

    public interface MsgDelListener {
        void confirm(boolean confirm);
    }

    public MsgDelConfirmDialog setClickListener(MsgDelListener listener) {
        mListener = listener;
        return this;
    }

}
