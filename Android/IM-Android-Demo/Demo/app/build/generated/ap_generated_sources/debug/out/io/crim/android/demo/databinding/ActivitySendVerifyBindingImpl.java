package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivitySendVerifyBindingImpl extends ActivitySendVerifyBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.back, 5);
        sViewsWithIds.put(R.id.send, 6);
    }
    // views
    @NonNull
    private final android.widget.FrameLayout mboundView0;
    @NonNull
    private final android.widget.RelativeLayout mboundView1;
    @NonNull
    private final android.widget.TextView mboundView2;
    @NonNull
    private final android.widget.TextView mboundView3;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers
    private androidx.databinding.InverseBindingListener hailandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of SearchVM.hail.getValue()
            //         is SearchVM.hail.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(hail);
            // localize variables for thread safety
            // SearchVM != null
            boolean searchVMJavaLangObjectNull = false;
            // SearchVM.hail != null
            boolean searchVMHailJavaLangObjectNull = false;
            // SearchVM.hail
            androidx.lifecycle.MutableLiveData<java.lang.String> searchVMHail = null;
            // SearchVM
            io.crim.android.ouicore.vm.SearchVM searchVM = mSearchVM;
            // SearchVM.hail.getValue()
            java.lang.String searchVMHailGetValue = null;



            searchVMJavaLangObjectNull = (searchVM) != (null);
            if (searchVMJavaLangObjectNull) {


                searchVMHail = searchVM.hail;

                searchVMHailJavaLangObjectNull = (searchVMHail) != (null);
                if (searchVMHailJavaLangObjectNull) {




                    searchVMHail.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };

    public ActivitySendVerifyBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 7, sIncludes, sViewsWithIds));
    }
    private ActivitySendVerifyBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 1
            , (bindings[5] != null) ? io.crim.android.ouicore.databinding.ViewBackBinding.bind((android.view.View) bindings[5]) : null
            , (android.widget.EditText) bindings[4]
            , (android.widget.Button) bindings[6]
            );
        this.hail.setTag(null);
        this.mboundView0 = (android.widget.FrameLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.mboundView1 = (android.widget.RelativeLayout) bindings[1];
        this.mboundView1.setTag(null);
        this.mboundView2 = (android.widget.TextView) bindings[2];
        this.mboundView2.setTag(null);
        this.mboundView3 = (android.widget.TextView) bindings[3];
        this.mboundView3.setTag(null);
        setRootTag(root);
        // listeners
        invalidateAll();
    }

    @Override
    public void invalidateAll() {
        synchronized(this) {
                mDirtyFlags = 0x4L;
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
            mDirtyFlags |= 0x2L;
        }
        notifyPropertyChanged(BR.SearchVM);
        super.requestRebind();
    }

    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
            case 0 :
                return onChangeSearchVMHail((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
        }
        return false;
    }
    private boolean onChangeSearchVMHail(androidx.lifecycle.MutableLiveData<java.lang.String> SearchVMHail, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x1L;
            }
            return true;
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
        java.lang.String searchVMIsPersonMboundView2AndroidStringFriendVerifyMboundView2AndroidStringGroupVerify = null;
        boolean searchVMIsPerson = false;
        io.crim.android.ouicore.vm.SearchVM searchVM = mSearchVM;
        java.lang.String searchVMIsPersonMboundView3AndroidStringSendFriendApplyMboundView3AndroidStringSendJoinGroupApply = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> searchVMHail = null;
        java.lang.String searchVMHailGetValue = null;

        if ((dirtyFlags & 0x7L) != 0) {


            if ((dirtyFlags & 0x6L) != 0) {

                    if (searchVM != null) {
                        // read SearchVM.isPerson
                        searchVMIsPerson = searchVM.isPerson;
                    }
                if((dirtyFlags & 0x6L) != 0) {
                    if(searchVMIsPerson) {
                            dirtyFlags |= 0x10L;
                            dirtyFlags |= 0x40L;
                    }
                    else {
                            dirtyFlags |= 0x8L;
                            dirtyFlags |= 0x20L;
                    }
                }


                    // read SearchVM.isPerson ? @android:string/friend_verify : @android:string/group_verify
                    searchVMIsPersonMboundView2AndroidStringFriendVerifyMboundView2AndroidStringGroupVerify = ((searchVMIsPerson) ? (mboundView2.getResources().getString(R.string.friend_verify)) : (mboundView2.getResources().getString(R.string.group_verify)));
                    // read SearchVM.isPerson ? @android:string/send_friend_apply : @android:string/send_join_group_apply
                    searchVMIsPersonMboundView3AndroidStringSendFriendApplyMboundView3AndroidStringSendJoinGroupApply = ((searchVMIsPerson) ? (mboundView3.getResources().getString(R.string.send_friend_apply)) : (mboundView3.getResources().getString(R.string.send_join_group_apply)));
            }

                if (searchVM != null) {
                    // read SearchVM.hail
                    searchVMHail = searchVM.hail;
                }
                updateLiveDataRegistration(0, searchVMHail);


                if (searchVMHail != null) {
                    // read SearchVM.hail.getValue()
                    searchVMHailGetValue = searchVMHail.getValue();
                }
        }
        // batch finished
        if ((dirtyFlags & 0x7L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.hail, searchVMHailGetValue);
        }
        if ((dirtyFlags & 0x4L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.hail, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, hailandroidTextAttrChanged);
        }
        if ((dirtyFlags & 0x6L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.mboundView2, searchVMIsPersonMboundView2AndroidStringFriendVerifyMboundView2AndroidStringGroupVerify);
            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.mboundView3, searchVMIsPersonMboundView3AndroidStringSendFriendApplyMboundView3AndroidStringSendJoinGroupApply);
        }
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): SearchVM.hail
        flag 1 (0x2L): SearchVM
        flag 2 (0x3L): null
        flag 3 (0x4L): SearchVM.isPerson ? @android:string/friend_verify : @android:string/group_verify
        flag 4 (0x5L): SearchVM.isPerson ? @android:string/friend_verify : @android:string/group_verify
        flag 5 (0x6L): SearchVM.isPerson ? @android:string/send_friend_apply : @android:string/send_join_group_apply
        flag 6 (0x7L): SearchVM.isPerson ? @android:string/send_friend_apply : @android:string/send_join_group_apply
    flag mapping end*/
    //end
}