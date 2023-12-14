package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityPersonDetailBindingImpl extends ActivityPersonDetailBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.back, 2);
        sViewsWithIds.put(R.id.avatar, 3);
        sViewsWithIds.put(R.id.nickName, 4);
        sViewsWithIds.put(R.id.userId, 5);
        sViewsWithIds.put(R.id.addFriend, 6);
        sViewsWithIds.put(R.id.groupInfo, 7);
        sViewsWithIds.put(R.id.groupNickName, 8);
        sViewsWithIds.put(R.id.time, 9);
        sViewsWithIds.put(R.id.joinMethodLy, 10);
        sViewsWithIds.put(R.id.joinMethod, 11);
        sViewsWithIds.put(R.id.userInfo, 12);
        sViewsWithIds.put(R.id.bottomMenu, 13);
        sViewsWithIds.put(R.id.call, 14);
        sViewsWithIds.put(R.id.sendMsg, 15);
    }
    // views
    @NonNull
    private final android.widget.FrameLayout mboundView0;
    @NonNull
    private final android.widget.RelativeLayout mboundView1;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers

    public ActivityPersonDetailBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 16, sIncludes, sViewsWithIds));
    }
    private ActivityPersonDetailBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 0
            , (android.widget.FrameLayout) bindings[6]
            , (io.crim.android.ouicore.widget.AvatarImage) bindings[3]
            , (bindings[2] != null) ? io.crim.android.ouicore.databinding.ViewBackBinding.bind((android.view.View) bindings[2]) : null
            , (android.widget.LinearLayout) bindings[13]
            , (android.widget.LinearLayout) bindings[14]
            , (android.widget.LinearLayout) bindings[7]
            , (android.widget.TextView) bindings[8]
            , (android.widget.TextView) bindings[11]
            , (android.widget.LinearLayout) bindings[10]
            , (android.widget.TextView) bindings[4]
            , (android.widget.LinearLayout) bindings[15]
            , (android.widget.TextView) bindings[9]
            , (android.widget.TextView) bindings[5]
            , (android.widget.LinearLayout) bindings[12]
            );
        this.mboundView0 = (android.widget.FrameLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.mboundView1 = (android.widget.RelativeLayout) bindings[1];
        this.mboundView1.setTag(null);
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