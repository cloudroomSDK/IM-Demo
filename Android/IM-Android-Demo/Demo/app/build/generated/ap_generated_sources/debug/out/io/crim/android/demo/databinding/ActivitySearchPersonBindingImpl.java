package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivitySearchPersonBindingImpl extends ActivitySearchPersonBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.sink, 2);
        sViewsWithIds.put(R.id.searchView, 3);
        sViewsWithIds.put(R.id.cancel, 4);
        sViewsWithIds.put(R.id.recyclerView, 5);
    }
    // views
    @NonNull
    private final android.widget.LinearLayout mboundView0;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers

    public ActivitySearchPersonBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 6, sIncludes, sViewsWithIds));
    }
    private ActivitySearchPersonBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 0
            , (android.widget.TextView) bindings[4]
            , (android.widget.TextView) bindings[1]
            , (androidx.recyclerview.widget.RecyclerView) bindings[5]
            , (io.crim.android.ouicore.widget.SearchView) bindings[3]
            , (android.widget.LinearLayout) bindings[2]
            );
        this.mboundView0 = (android.widget.LinearLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.notFind.setTag(null);
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
        if (BR.SearchVM == variableId) {
            setSearchVM((io.crim.android.ouicore.vm.SearchVM) variable);
        }
        else {
            variableSet = false;
        }
            return variableSet;
    }

    public void setSearchVM(@Nullable io.crim.android.ouicore.vm.SearchVM SearchVM) {
        this.mSearchVM = SearchVM;
        synchronized(this) {
            mDirtyFlags |= 0x1L;
        }
        notifyPropertyChanged(BR.SearchVM);
        super.requestRebind();
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
        boolean searchVMIsPerson = false;
        io.crim.android.ouicore.vm.SearchVM searchVM = mSearchVM;
        int searchVMIsPersonIoCrimAndroidDemoRStringNotFindUserIoCrimAndroidDemoRStringNotFindGroup = 0;

        if ((dirtyFlags & 0x3L) != 0) {



                if (searchVM != null) {
                    // read SearchVM.isPerson
                    searchVMIsPerson = searchVM.isPerson;
                }
            if((dirtyFlags & 0x3L) != 0) {
                if(searchVMIsPerson) {
                        dirtyFlags |= 0x8L;
                }
                else {
                        dirtyFlags |= 0x4L;
                }
            }


                // read SearchVM.isPerson ? io.crim.android.demo.R.string.not_find_user : io.crim.android.demo.R.string.not_find_group
                searchVMIsPersonIoCrimAndroidDemoRStringNotFindUserIoCrimAndroidDemoRStringNotFindGroup = ((searchVMIsPerson) ? (io.crim.android.demo.R.string.not_find_user) : (io.crim.android.demo.R.string.not_find_group));
        }
        // batch finished
        if ((dirtyFlags & 0x3L) != 0) {
            // api target 1

            this.notFind.setText(searchVMIsPersonIoCrimAndroidDemoRStringNotFindUserIoCrimAndroidDemoRStringNotFindGroup);
        }
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): SearchVM
        flag 1 (0x2L): null
        flag 2 (0x3L): SearchVM.isPerson ? io.crim.android.demo.R.string.not_find_user : io.crim.android.demo.R.string.not_find_group
        flag 3 (0x4L): SearchVM.isPerson ? io.crim.android.demo.R.string.not_find_user : io.crim.android.demo.R.string.not_find_group
    flag mapping end*/
    //end
}