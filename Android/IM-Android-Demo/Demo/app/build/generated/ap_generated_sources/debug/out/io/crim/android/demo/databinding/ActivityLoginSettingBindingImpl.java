package io.crim.android.demo.databinding;
import io.crim.android.demo.R;
import io.crim.android.demo.BR;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import android.view.View;
@SuppressWarnings("unchecked")
public class ActivityLoginSettingBindingImpl extends ActivityLoginSettingBinding  {

    @Nullable
    private static final androidx.databinding.ViewDataBinding.IncludedLayouts sIncludes;
    @Nullable
    private static final android.util.SparseIntArray sViewsWithIds;
    static {
        sIncludes = null;
        sViewsWithIds = new android.util.SparseIntArray();
        sViewsWithIds.put(R.id.back, 7);
        sViewsWithIds.put(R.id.layoutTitle, 8);
        sViewsWithIds.put(R.id.llBusServer, 9);
        sViewsWithIds.put(R.id.llServer, 10);
        sViewsWithIds.put(R.id.llType, 11);
        sViewsWithIds.put(R.id.tvType, 12);
        sViewsWithIds.put(R.id.llID, 13);
        sViewsWithIds.put(R.id.groupAppID, 14);
        sViewsWithIds.put(R.id.llSecret, 15);
        sViewsWithIds.put(R.id.llToken, 16);
        sViewsWithIds.put(R.id.llDefault, 17);
        sViewsWithIds.put(R.id.restart, 18);
    }
    // views
    @NonNull
    private final androidx.constraintlayout.widget.ConstraintLayout mboundView0;
    @NonNull
    private final android.widget.RelativeLayout mboundView1;
    // variables
    // values
    // listeners
    // Inverse Binding Event Handlers
    private androidx.databinding.InverseBindingListener etAppIDandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of loginSettingVM.APP_ID.getValue()
            //         is loginSettingVM.APP_ID.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(etAppID);
            // localize variables for thread safety
            // loginSettingVM.APP_ID != null
            boolean loginSettingVMAPPIDJavaLangObjectNull = false;
            // loginSettingVM.APP_ID.getValue()
            java.lang.String loginSettingVMAPPIDGetValue = null;
            // loginSettingVM
            io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
            // loginSettingVM != null
            boolean loginSettingVMJavaLangObjectNull = false;
            // loginSettingVM.APP_ID
            androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMAPPID = null;



            loginSettingVMJavaLangObjectNull = (loginSettingVM) != (null);
            if (loginSettingVMJavaLangObjectNull) {


                loginSettingVMAPPID = loginSettingVM.APP_ID;

                loginSettingVMAPPIDJavaLangObjectNull = (loginSettingVMAPPID) != (null);
                if (loginSettingVMAPPIDJavaLangObjectNull) {




                    loginSettingVMAPPID.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };
    private androidx.databinding.InverseBindingListener etBusinessServerandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of loginSettingVM.HEAD.getValue()
            //         is loginSettingVM.HEAD.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(etBusinessServer);
            // localize variables for thread safety
            // loginSettingVM.HEAD
            androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMHEAD = null;
            // loginSettingVM.HEAD != null
            boolean loginSettingVMHEADJavaLangObjectNull = false;
            // loginSettingVM
            io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
            // loginSettingVM.HEAD.getValue()
            java.lang.String loginSettingVMHEADGetValue = null;
            // loginSettingVM != null
            boolean loginSettingVMJavaLangObjectNull = false;



            loginSettingVMJavaLangObjectNull = (loginSettingVM) != (null);
            if (loginSettingVMJavaLangObjectNull) {


                loginSettingVMHEAD = loginSettingVM.HEAD;

                loginSettingVMHEADJavaLangObjectNull = (loginSettingVMHEAD) != (null);
                if (loginSettingVMHEADJavaLangObjectNull) {




                    loginSettingVMHEAD.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };
    private androidx.databinding.InverseBindingListener etSecretandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of loginSettingVM.APP_SECRET.getValue()
            //         is loginSettingVM.APP_SECRET.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(etSecret);
            // localize variables for thread safety
            // loginSettingVM
            io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
            // loginSettingVM.APP_SECRET != null
            boolean loginSettingVMAPPSECRETJavaLangObjectNull = false;
            // loginSettingVM.APP_SECRET.getValue()
            java.lang.String loginSettingVMAPPSECRETGetValue = null;
            // loginSettingVM != null
            boolean loginSettingVMJavaLangObjectNull = false;
            // loginSettingVM.APP_SECRET
            androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMAPPSECRET = null;



            loginSettingVMJavaLangObjectNull = (loginSettingVM) != (null);
            if (loginSettingVMJavaLangObjectNull) {


                loginSettingVMAPPSECRET = loginSettingVM.APP_SECRET;

                loginSettingVMAPPSECRETJavaLangObjectNull = (loginSettingVMAPPSECRET) != (null);
                if (loginSettingVMAPPSECRETJavaLangObjectNull) {




                    loginSettingVMAPPSECRET.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };
    private androidx.databinding.InverseBindingListener etServerandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of loginSettingVM.SERVER_IP.getValue()
            //         is loginSettingVM.SERVER_IP.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(etServer);
            // localize variables for thread safety
            // loginSettingVM
            io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
            // loginSettingVM != null
            boolean loginSettingVMJavaLangObjectNull = false;
            // loginSettingVM.SERVER_IP
            androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMSERVERIP = null;
            // loginSettingVM.SERVER_IP != null
            boolean loginSettingVMSERVERIPJavaLangObjectNull = false;
            // loginSettingVM.SERVER_IP.getValue()
            java.lang.String loginSettingVMSERVERIPGetValue = null;



            loginSettingVMJavaLangObjectNull = (loginSettingVM) != (null);
            if (loginSettingVMJavaLangObjectNull) {


                loginSettingVMSERVERIP = loginSettingVM.SERVER_IP;

                loginSettingVMSERVERIPJavaLangObjectNull = (loginSettingVMSERVERIP) != (null);
                if (loginSettingVMSERVERIPJavaLangObjectNull) {




                    loginSettingVMSERVERIP.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };
    private androidx.databinding.InverseBindingListener etTokenandroidTextAttrChanged = new androidx.databinding.InverseBindingListener() {
        @Override
        public void onChange() {
            // Inverse of loginSettingVM.LOGIN_TOKEN.getValue()
            //         is loginSettingVM.LOGIN_TOKEN.setValue((java.lang.String) callbackArg_0)
            java.lang.String callbackArg_0 = androidx.databinding.adapters.TextViewBindingAdapter.getTextString(etToken);
            // localize variables for thread safety
            // loginSettingVM
            io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
            // loginSettingVM.LOGIN_TOKEN != null
            boolean loginSettingVMLOGINTOKENJavaLangObjectNull = false;
            // loginSettingVM != null
            boolean loginSettingVMJavaLangObjectNull = false;
            // loginSettingVM.LOGIN_TOKEN
            androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMLOGINTOKEN = null;
            // loginSettingVM.LOGIN_TOKEN.getValue()
            java.lang.String loginSettingVMLOGINTOKENGetValue = null;



            loginSettingVMJavaLangObjectNull = (loginSettingVM) != (null);
            if (loginSettingVMJavaLangObjectNull) {


                loginSettingVMLOGINTOKEN = loginSettingVM.LOGIN_TOKEN;

                loginSettingVMLOGINTOKENJavaLangObjectNull = (loginSettingVMLOGINTOKEN) != (null);
                if (loginSettingVMLOGINTOKENJavaLangObjectNull) {




                    loginSettingVMLOGINTOKEN.setValue(((java.lang.String) (callbackArg_0)));
                }
            }
        }
    };

    public ActivityLoginSettingBindingImpl(@Nullable androidx.databinding.DataBindingComponent bindingComponent, @NonNull View root) {
        this(bindingComponent, root, mapBindings(bindingComponent, root, 19, sIncludes, sViewsWithIds));
    }
    private ActivityLoginSettingBindingImpl(androidx.databinding.DataBindingComponent bindingComponent, View root, Object[] bindings) {
        super(bindingComponent, root, 5
            , (bindings[7] != null) ? io.crim.android.ouicore.databinding.ViewBackBinding.bind((android.view.View) bindings[7]) : null
            , (android.widget.EditText) bindings[4]
            , (android.widget.EditText) bindings[2]
            , (android.widget.EditText) bindings[5]
            , (android.widget.EditText) bindings[3]
            , (android.widget.EditText) bindings[6]
            , (androidx.constraintlayout.widget.Group) bindings[14]
            , (androidx.cardview.widget.CardView) bindings[8]
            , (android.widget.LinearLayout) bindings[9]
            , (android.widget.LinearLayout) bindings[17]
            , (android.widget.LinearLayout) bindings[13]
            , (android.widget.LinearLayout) bindings[15]
            , (android.widget.LinearLayout) bindings[10]
            , (android.widget.LinearLayout) bindings[16]
            , (android.widget.LinearLayout) bindings[11]
            , (android.widget.Button) bindings[18]
            , (android.widget.TextView) bindings[12]
            );
        this.etAppID.setTag(null);
        this.etBusinessServer.setTag(null);
        this.etSecret.setTag(null);
        this.etServer.setTag(null);
        this.etToken.setTag(null);
        this.mboundView0 = (androidx.constraintlayout.widget.ConstraintLayout) bindings[0];
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
                mDirtyFlags = 0x40L;
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
        if (BR.loginSettingVM == variableId) {
            setLoginSettingVM((io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM) variable);
        }
        else {
            variableSet = false;
        }
            return variableSet;
    }

    public void setLoginSettingVM(@Nullable io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM LoginSettingVM) {
        this.mLoginSettingVM = LoginSettingVM;
        synchronized(this) {
            mDirtyFlags |= 0x20L;
        }
        notifyPropertyChanged(BR.loginSettingVM);
        super.requestRebind();
    }

    @Override
    protected boolean onFieldChange(int localFieldId, Object object, int fieldId) {
        switch (localFieldId) {
            case 0 :
                return onChangeLoginSettingVMAPPID((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
            case 1 :
                return onChangeLoginSettingVMHEAD((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
            case 2 :
                return onChangeLoginSettingVMLOGINTOKEN((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
            case 3 :
                return onChangeLoginSettingVMAPPSECRET((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
            case 4 :
                return onChangeLoginSettingVMSERVERIP((androidx.lifecycle.MutableLiveData<java.lang.String>) object, fieldId);
        }
        return false;
    }
    private boolean onChangeLoginSettingVMAPPID(androidx.lifecycle.MutableLiveData<java.lang.String> LoginSettingVMAPPID, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x1L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginSettingVMHEAD(androidx.lifecycle.MutableLiveData<java.lang.String> LoginSettingVMHEAD, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x2L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginSettingVMLOGINTOKEN(androidx.lifecycle.MutableLiveData<java.lang.String> LoginSettingVMLOGINTOKEN, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x4L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginSettingVMAPPSECRET(androidx.lifecycle.MutableLiveData<java.lang.String> LoginSettingVMAPPSECRET, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x8L;
            }
            return true;
        }
        return false;
    }
    private boolean onChangeLoginSettingVMSERVERIP(androidx.lifecycle.MutableLiveData<java.lang.String> LoginSettingVMSERVERIP, int fieldId) {
        if (fieldId == BR._all) {
            synchronized(this) {
                    mDirtyFlags |= 0x10L;
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
        java.lang.String loginSettingVMAPPIDGetValue = null;
        java.lang.String loginSettingVMHEADGetValue = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMAPPID = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMHEAD = null;
        io.crim.android.demo.ui.LoginSettingActivity.LoginSettingVM loginSettingVM = mLoginSettingVM;
        java.lang.String loginSettingVMAPPSECRETGetValue = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMLOGINTOKEN = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMAPPSECRET = null;
        java.lang.String loginSettingVMSERVERIPGetValue = null;
        java.lang.String loginSettingVMLOGINTOKENGetValue = null;
        androidx.lifecycle.MutableLiveData<java.lang.String> loginSettingVMSERVERIP = null;

        if ((dirtyFlags & 0x7fL) != 0) {


            if ((dirtyFlags & 0x61L) != 0) {

                    if (loginSettingVM != null) {
                        // read loginSettingVM.APP_ID
                        loginSettingVMAPPID = loginSettingVM.APP_ID;
                    }
                    updateLiveDataRegistration(0, loginSettingVMAPPID);


                    if (loginSettingVMAPPID != null) {
                        // read loginSettingVM.APP_ID.getValue()
                        loginSettingVMAPPIDGetValue = loginSettingVMAPPID.getValue();
                    }
            }
            if ((dirtyFlags & 0x62L) != 0) {

                    if (loginSettingVM != null) {
                        // read loginSettingVM.HEAD
                        loginSettingVMHEAD = loginSettingVM.HEAD;
                    }
                    updateLiveDataRegistration(1, loginSettingVMHEAD);


                    if (loginSettingVMHEAD != null) {
                        // read loginSettingVM.HEAD.getValue()
                        loginSettingVMHEADGetValue = loginSettingVMHEAD.getValue();
                    }
            }
            if ((dirtyFlags & 0x64L) != 0) {

                    if (loginSettingVM != null) {
                        // read loginSettingVM.LOGIN_TOKEN
                        loginSettingVMLOGINTOKEN = loginSettingVM.LOGIN_TOKEN;
                    }
                    updateLiveDataRegistration(2, loginSettingVMLOGINTOKEN);


                    if (loginSettingVMLOGINTOKEN != null) {
                        // read loginSettingVM.LOGIN_TOKEN.getValue()
                        loginSettingVMLOGINTOKENGetValue = loginSettingVMLOGINTOKEN.getValue();
                    }
            }
            if ((dirtyFlags & 0x68L) != 0) {

                    if (loginSettingVM != null) {
                        // read loginSettingVM.APP_SECRET
                        loginSettingVMAPPSECRET = loginSettingVM.APP_SECRET;
                    }
                    updateLiveDataRegistration(3, loginSettingVMAPPSECRET);


                    if (loginSettingVMAPPSECRET != null) {
                        // read loginSettingVM.APP_SECRET.getValue()
                        loginSettingVMAPPSECRETGetValue = loginSettingVMAPPSECRET.getValue();
                    }
            }
            if ((dirtyFlags & 0x70L) != 0) {

                    if (loginSettingVM != null) {
                        // read loginSettingVM.SERVER_IP
                        loginSettingVMSERVERIP = loginSettingVM.SERVER_IP;
                    }
                    updateLiveDataRegistration(4, loginSettingVMSERVERIP);


                    if (loginSettingVMSERVERIP != null) {
                        // read loginSettingVM.SERVER_IP.getValue()
                        loginSettingVMSERVERIPGetValue = loginSettingVMSERVERIP.getValue();
                    }
            }
        }
        // batch finished
        if ((dirtyFlags & 0x61L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.etAppID, loginSettingVMAPPIDGetValue);
        }
        if ((dirtyFlags & 0x40L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.etAppID, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, etAppIDandroidTextAttrChanged);
            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.etBusinessServer, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, etBusinessServerandroidTextAttrChanged);
            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.etSecret, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, etSecretandroidTextAttrChanged);
            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.etServer, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, etServerandroidTextAttrChanged);
            androidx.databinding.adapters.TextViewBindingAdapter.setTextWatcher(this.etToken, (androidx.databinding.adapters.TextViewBindingAdapter.BeforeTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.OnTextChanged)null, (androidx.databinding.adapters.TextViewBindingAdapter.AfterTextChanged)null, etTokenandroidTextAttrChanged);
        }
        if ((dirtyFlags & 0x62L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.etBusinessServer, loginSettingVMHEADGetValue);
        }
        if ((dirtyFlags & 0x68L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.etSecret, loginSettingVMAPPSECRETGetValue);
        }
        if ((dirtyFlags & 0x70L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.etServer, loginSettingVMSERVERIPGetValue);
        }
        if ((dirtyFlags & 0x64L) != 0) {
            // api target 1

            androidx.databinding.adapters.TextViewBindingAdapter.setText(this.etToken, loginSettingVMLOGINTOKENGetValue);
        }
    }
    // Listener Stub Implementations
    // callback impls
    // dirty flag
    private  long mDirtyFlags = 0xffffffffffffffffL;
    /* flag mapping
        flag 0 (0x1L): loginSettingVM.APP_ID
        flag 1 (0x2L): loginSettingVM.HEAD
        flag 2 (0x3L): loginSettingVM.LOGIN_TOKEN
        flag 3 (0x4L): loginSettingVM.APP_SECRET
        flag 4 (0x5L): loginSettingVM.SERVER_IP
        flag 5 (0x6L): loginSettingVM
        flag 6 (0x7L): null
    flag mapping end*/
    //end
}