package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityMoreDataBindingImpl extends ActivityMoreDataBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.back, 3);
        sViewsWithIds.put(R.id.avatarLy, 9);
        sViewsWithIds.put(R.id.avatar, 10);
        sViewsWithIds.put(R.id.nickNameLy, 11);
        sViewsWithIds.put(R.id.nickName, 12);
        sViewsWithIds.put(R.id.genderLy, 13);
        sViewsWithIds.put(R.id.gender, 14);
        sViewsWithIds.put(R.id.birthdayLy, 15);
        sViewsWithIds.put(R.id.birthday, 16);
        sViewsWithIds.put(R.id.qrCode, 17);
        sViewsWithIds.put(R.id.phoneTv, 18);
        sViewsWithIds.put(R.id.idLy, 19);
        sViewsWithIds.put(R.id.mailTv, 20);
    }
    // views
    @NonNull
    private final android.widget.FrameLayout mboundView0;
    @NonNull
    private final android.widget.LinearLayout mboundView1;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView11;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView12;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView13;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView14;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView15;
    @NonNull
    private final android.widget.RelativeLayout mboundView2;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers

    public ActivityMoreDataBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 21, sIncludes, sViewsWithIds));
    }
    private ActivityMoreDataBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 0
            , (io.crim.android.ouicore.widget.AvatarImage) bindings[10]
            , (android.widget.LinearLayout) bindings[9]
            , (bindings[3] != null) ? io.crim.android.ouicore.databinding.ViewBackBinding.bind((android.view.View) bindings[3]) : null
            , (android.widget.TextView) bindings[16]
            , (android.widget.LinearLayout) bindings[15]
            , (android.widget.TextView) bindings[14]
            , (android.widget.LinearLayout) bindings[13]
            , (android.widget.LinearLayout) bindings[19]
            , (android.widget.TextView) bindings[20]
            , (android.widget.TextView) bindings[12]
            , (android.widget.LinearLayout) bindings[11]
            , (android.widget.TextView) bindings[18]
            , (android.widget.LinearLayout) bindings[17]
            );
        this.mboundView0 = (android.widget.FrameLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.mboundView1 = (android.widget.LinearLayout) bindings[1];
        this.mboundView1.setTag(null);
        this.mboundView11 = (bindings[4] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[4]) : null;
        this.mboundView12 = (bindings[5] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[5]) : null;
        this.mboundView13 = (bindings[6] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[6]) : null;
        this.mboundView14 = (bindings[7] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[7]) : null;
        this.mboundView15 = (bindings[8] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[8]) : null;
        this.mboundView2 = (android.widget.RelativeLayout) bindings[2];
        this.mboundView2.setTag(null);
        setRootTag(root);
        // listeners
        invalidateAll();
    }

    @Override
    public void invalidateAll() {
        synchronized(this) {
                mDirtyFlags = 0x1L;
        }
        requestRebind();
    }

    @Override
    public boolean hasPendingBindings() {
        synchronized(this) {
            if (mDirtyFlags != 0) {
                return true;
            }
        }
        return false;
    }

    @Override
    public boolean setVariable(int variableId, @Nullable Object variable)  {
        boolean variableSet = true;
            return variableSet;
    }

    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
        }
        return false;
    }

    @Override
    protected void executeBindings() {
        long dirtyFlags = 0;
        synchronized(this) {
            dirtyFlags = mDirtyFlags;
            mDirtyFlags = 0;
        }
        // batch finished
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): null
    flag mapping end*/
    //end
}