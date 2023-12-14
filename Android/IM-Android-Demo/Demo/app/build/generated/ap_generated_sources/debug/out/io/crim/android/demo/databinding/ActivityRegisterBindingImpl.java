package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityRegisterBindingImpl extends ActivityRegisterBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.back, 2);
        sViewsWithIds.put(R.id.title, 3);
        sViewsWithIds.put(R.id.tips, 4);
        sViewsWithIds.put(R.id.clear, 5);
        sViewsWithIds.put(R.id.submit, 6);
        sViewsWithIds.put(R.id.protocolLy, 7);
        sViewsWithIds.put(R.id.protocol, 8);
    }
    // views
    @NonNull
    private final android.widget.LinearLayout mboundView0;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers
    private androidx.databinding.InverseBindingListener edt1androidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of LoginVM.account.getValue()
            //         is LoginVM.account.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(edt1);
            // localize variables for thread safety
            // LoginVM
            io.crim.android.demo.vm.LoginVM loginVM = mLoginVM;
            // LoginVM.account
            androidx.lifecycle.MutableLiveData<java.lang.String> loginVMAccount = null;
            // LoginVM.account.getValue()
            java.lang.String loginVMAccountGetValue = null;
            // LoginVM != null
            boolean loginVMJavaLangObjectNull = false;
            // LoginVM.account != null
            boolean loginVMAccountJavaLangObjectNull = false;



            loginVMJavaLangObjectNull = (loginVM) != (null);
            if (loginVMJavaLangObjectNull) {


                loginVMAccount = loginVM.account;

                loginVMAccountJavaLangObjectNull = (loginVMAccount) != (null);
                if (loginVMAccountJavaLangObjectNull) {




                    loginVMAccount.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };

    public ActivityRegisterBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 9, sIncludes, sViewsWithIds));
    }
    private ActivityRegisterBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 1
            , (android.widget.ImageView) bindings[2]
            , (android.widget.ImageView) bindings[5]
            , (android.widget.EditText) bindings[1]
            , (android.widget.CheckBox) bindings[8]
            , (android.widget.LinearLayout) bindings[7]
            , (android.widget.Button) bindings[6]
            , (android.widget.TextView) bindings[4]
            , (android.widget.TextView) bindings[3]
            );
        this.edt1.setTag(null);
        this.mboundView0 = (android.widget.LinearLayout) bindings[0];
        this.mboundView0.setTag(null);
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
        if (BR.LoginVM == variableId) {
            setLoginVM((io.crim.android.demo.vm.LoginVM) variable);
        }
        else {
            variableSet = false;
        }
            return variableSet;
    }

    public void setLoginVM(@Nullable io.crim.android.demo.vm.LoginVM LoginVM) {
        this.mLoginVM = LoginVM;
        synchronized(this) {
            mDirtyFlags |= 0x2L;
        }
        notifyPropertyChanged(BR.LoginVM);
        super.requestRebind();
    }

    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
            case 0 :
                return onChangeLoginVMAccount((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
        }
        return false;
    }
    private boolean onChangeLoginVMAccount(androidx.lifecycle.MutableLiveData<java.lang.String> LoginVMAccount, int fieldId) {
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
        io.crim.android.demo.vm.LoginVM loginVM = mLoginVM;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginVMAccount = null;
        java.lang.String loginVMAccountGetValue = null;

        if ((dirtyFlags & 0x7L) != 0) {



                if (loginVM != null) {
                    // read LoginVM.account
                    loginVMAccount = loginVM.account;
                }
                updateLiveDataRegistration(0, loginVMAccount);


                if (loginVMAccount != null) {
                    // read LoginVM.account.getValue()
                    loginVMAccountGetValue = loginVMAccount.getValue();
                }
        }
        // batch finished
        if ((dirtyFlags & 0x7L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.edt1, loginVMAccountGetValue);
        }
        if ((dirtyFlags & 0x4L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.edt1, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, edt1androidTextAttrChanged);
        }
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): LoginVM.account
        flag 1 (0x2L): LoginVM
        flag 2 (0x3L): null
    flag mapping end*/
    //end
}