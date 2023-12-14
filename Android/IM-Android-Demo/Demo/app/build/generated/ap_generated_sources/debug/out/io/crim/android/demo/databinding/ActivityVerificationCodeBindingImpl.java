package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityVerificationCodeBindingImpl extends ActivityVerificationCodeBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.codeEditText, 5);
        sViewsWithIds.put(R.id.resend, 6);
    }
    // views
    @NonNull
    private final android.widget.LinearLayout mboundView0;
    @NonNull
    private final android.widget.TextView mboundView4;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers
    private androidx.databinding.InverseBindingListener accountTvandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of LoginVM.account.getValue()
            //         is LoginVM.account.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(accountTv);
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

    public ActivityVerificationCodeBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 7, sIncludes, sViewsWithIds));
    }
    private ActivityVerificationCodeBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 3
            , (android.widget.TextView) bindings[2]
            , (io.crim.android.ouicore.widget.CodeEditText) bindings[5]
            , (android.widget.TextView) bindings[3]
            , (android.widget.TextView) bindings[6]
            , (android.widget.TextView) bindings[1]
            );
        this.accountTv.setTag(null);
        this.countdown.setTag(null);
        this.mboundView0 = (android.widget.LinearLayout) bindings[0];
        this.mboundView0.setTag(null);
        this.mboundView4 = (android.widget.TextView) bindings[4];
        this.mboundView4.setTag(null);
        this.tips.setTag(null);
        setRootTag(root);
        // listeners
        invalidateAll();
    }

    @Override
    public void invalidateAll() {
        synchronized(this) {
                mDirtyFlags = 0x10L;
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
            mDirtyFlags |= 0x8L;
        }
        notifyPropertyChanged(BR.LoginVM);
        super.requestRebind();
    }

    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
            case 0 :
                return onChangeLoginVMIsPhone((androidx.lifecycle.MutableLiveData<java.lang.Boolean>) object, fieldId);
            case 1 :
                return onChangeLoginVMCountdown((androidx.lifecycle.MutableLiveData<java.lang.Integer>) object, fieldId);
            case 2 :
                return onChangeLoginVMAccount((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
        }
        return false;
    }
    private boolean onChangeLoginVMIsPhone(androidx.lifecycle.MutableLiveData<java.lang.Boolean> LoginVMIsPhone, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x1L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginVMCountdown(androidx.lifecycle.MutableLiveData<java.lang.Integer> LoginVMCountdown, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x2L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginVMAccount(androidx.lifecycle.MutableLiveData<java.lang.String> LoginVMAccount, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x4L;
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
        java.lang.String loginVMCountdownToStringCharS = null;
        boolean androidxDatabindingViewDataBindingSafeUnboxLoginVMIsPhoneGetValue = false;
        boolean loginVMCountdownInt0 = false;
        int androidxDatabindingViewDataBindingSafeUnboxLoginVMCountdownGetValue = 0;
        java.lang.String loginVMIsPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendMail = null;
        androidx.lifecycle.MutableLiveData<java.lang.Boolean> loginVMIsPhone = null;
        androidx.lifecycle.MutableLiveData<java.lang.Integer> loginVMCountdown = null;
        io.crim.android.demo.vm.LoginVM loginVM = mLoginVM;
        int loginVMCountdownInt0ViewGONEViewVISIBLE = 0;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginVMAccount = null;
        java.lang.String loginVMAccountGetValue = null;
        java.lang.String loginVMCountdownToString = null;
        java.lang.Boolean loginVMIsPhoneGetValue = null;
        java.lang.Integer loginVMCountdownGetValue = null;

        if ((dirtyFlags & 0x1fL) != 0) {


            if ((dirtyFlags & 0x19L) != 0) {

                    if (loginVM != null) {
                        // read LoginVM.isPhone
                        loginVMIsPhone = loginVM.isPhone;
                    }
                    updateLiveDataRegistration(0, loginVMIsPhone);


                    if (loginVMIsPhone != null) {
                        // read LoginVM.isPhone.getValue()
                        loginVMIsPhoneGetValue = loginVMIsPhone.getValue();
                    }


                    // read androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.isPhone.getValue())
                    androidxDatabindingViewDataBindingSafeUnboxLoginVMIsPhoneGetValue = androidx.databinding.ViewDataBinding.safeUnbox(loginVMIsPhoneGetValue);
                if((dirtyFlags & 0x19L) != 0) {
                    if(androidxDatabindingViewDataBindingSafeUnboxLoginVMIsPhoneGetValue) {
                            dirtyFlags |= 0x40L;
                    }
                    else {
                            dirtyFlags |= 0x20L;
                    }
                }


                    // read androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.isPhone.getValue()) ? BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_phone) : BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_mail)
                    loginVMIsPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendMail = ((androidxDatabindingViewDataBindingSafeUnboxLoginVMIsPhoneGetValue) ? (io.crim.android.ouicore.base.BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_phone)) : (io.crim.android.ouicore.base.BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_mail)));
            }
            if ((dirtyFlags & 0x1aL) != 0) {

                    if (loginVM != null) {
                        // read LoginVM.countdown
                        loginVMCountdown = loginVM.countdown;
                    }
                    updateLiveDataRegistration(1, loginVMCountdown);


                    if (loginVMCountdown != null) {
                        // read LoginVM.countdown.getValue()
                        loginVMCountdownGetValue = loginVMCountdown.getValue();
                    }


                    // read androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.countdown.getValue())
                    androidxDatabindingViewDataBindingSafeUnboxLoginVMCountdownGetValue = androidx.databinding.ViewDataBinding.safeUnbox(loginVMCountdownGetValue);
                    if (loginVMCountdownGetValue != null) {
                        // read LoginVM.countdown.getValue().toString()
                        loginVMCountdownToString = loginVMCountdownGetValue.toString();
                    }


                    // read androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.countdown.getValue()) == 0
                    loginVMCountdownInt0 = (androidxDatabindingViewDataBindingSafeUnboxLoginVMCountdownGetValue) == (0);
                    // read (LoginVM.countdown.getValue().toString()) + ('s')
                    loginVMCountdownToStringCharS = (loginVMCountdownToString) + ('s');
                if((dirtyFlags & 0x1aL) != 0) {
                    if(loginVMCountdownInt0) {
                            dirtyFlags |= 0x100L;
                    }
                    else {
                            dirtyFlags |= 0x80L;
                    }
                }


                    // read androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.countdown.getValue()) == 0 ? View.GONE : View.VISIBLE
                    loginVMCountdownInt0ViewGONEViewVISIBLE = ((loginVMCountdownInt0) ? (android.view.View.GONE) : (android.view.View.VISIBLE));
            }
            if ((dirtyFlags & 0x1cL) != 0) {

                    if (loginVM != null) {
                        // read LoginVM.account
                        loginVMAccount = loginVM.account;
                    }
                    updateLiveDataRegistration(2, loginVMAccount);


                    if (loginVMAccount != null) {
                        // read LoginVM.account.getValue()
                        loginVMAccountGetValue = loginVMAccount.getValue();
                    }
            }
        }
        // batch finished
        if ((dirtyFlags & 0x1cL) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.accountTv, loginVMAccountGetValue);
        }
        if ((dirtyFlags & 0x10L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.accountTv, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, accountTvandroidTextAttrChanged);
        }
        if ((dirtyFlags & 0x1aL) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.countdown, loginVMCountdownToStringCharS);
            this.countdown.setVisibility(loginVMCountdownInt0ViewGONEViewVISIBLE);
            this.mboundView4.setVisibility(loginVMCountdownInt0ViewGONEViewVISIBLE);
        }
        if ((dirtyFlags & 0x19L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.tips, loginVMIsPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendPhoneBaseAppInstGetStringIoCrimAndroidDemoRStringVerificationCodeSendMail);
        }
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): LoginVM.isPhone
        flag 1 (0x2L): LoginVM.countdown
        flag 2 (0x3L): LoginVM.account
        flag 3 (0x4L): LoginVM
        flag 4 (0x5L): null
        flag 5 (0x6L): androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.isPhone.getValue()) ? BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_phone) : BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_mail)
        flag 6 (0x7L): androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.isPhone.getValue()) ? BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_phone) : BaseApp.inst().getString(io.crim.android.demo.R.string.verification_code_send_mail)
        flag 7 (0x8L): androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.countdown.getValue()) == 0 ? View.GONE : View.VISIBLE
        flag 8 (0x9L): androidx.databinding.ViewDataBinding.safeUnbox(LoginVM.countdown.getValue()) == 0 ? View.GONE : View.VISIBLE
    flag mapping end*/
    //end
}