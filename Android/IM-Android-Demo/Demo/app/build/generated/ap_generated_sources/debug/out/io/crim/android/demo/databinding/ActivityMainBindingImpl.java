package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityMainBindingImpl extends ActivityMainBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.fragment_container, 3);
        sViewsWithIds.put(R.id.menuGroup, 4);
        sViewsWithIds.put(R.id.men1, 5);
        sViewsWithIds.put(R.id.men2, 6);
        sViewsWithIds.put(R.id.men3, 7);
        sViewsWithIds.put(R.id.men4, 8);
    }
    // views
    @NonNull
    private final android.widget.LinearLayout mboundView0;
    @NonNull
    private final android.widget.RelativeLayout mboundView1;
    @Nullable
    private final io.crim.android.ouicore.databinding.ViewDividingLineBinding mboundView11;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers

    public ActivityMainBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 9, sIncludes, sViewsWithIds));
    }
    private ActivityMainBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 0
            , (android.widget.FrameLayout) bindings[3]
            , (android.widget.RadioButton) bindings[5]
            , (android.widget.RadioButton) bindings[6]
            , (android.widget.RadioButton) bindings[7]
            , (android.widget.RadioButton) bindings[8]
            , (android.widget.LinearLayout) bindings[4]
            );
        this.mboundView0 = (android.widget.LinearLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.mboundView1 = (android.widget.RelativeLayout) bindings[1];
        this.mboundView1.setTag(null);
        this.mboundView11 = (bindings[2] != null) ? io.crim.android.ouicore.databinding.ViewDividingLineBinding.bind((android.view.View) bindings[2]) : null;
        setRootTag(root);
        // listeners
        invalidateAll();
    }

    @Override
    public void invalidateAll() {
        synchronized(this) {
                mDirtyFlags = 0x2L;
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
        if (BR.MainVM == variableId) {
            setMainVM((io.crim.android.demo.vm.MainVM) variable);
        }
        else {
            variableSet = false;
        }
            return variableSet;
    }

    public void setMainVM(@Nullable io.crim.android.demo.vm.MainVM MainVM) {
        this.mMainVM = MainVM;
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
        flag 0 (0x1L): MainVM
        flag 1 (0x2L): null
    flag mapping end*/
    //end
}