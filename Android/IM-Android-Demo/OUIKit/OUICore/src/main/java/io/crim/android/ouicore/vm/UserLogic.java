package io.crim.android.ouicore.vm;

import io.crim.android.ouicore.R;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.base.vm.injection.BaseVM;
import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.utils.L;

public class UserLogic extends BaseVM {
    public enum ConnectStatus {
        DEFAULT(""),
        CONNECTING(BaseApp.inst().getString(R.string.connecting)),
        CONNECT_ERR(BaseApp.inst().getString(R.string.conn_failed)),
        SYNCING(BaseApp.inst().getString(R.string.syncing)),
        SYNC_ERR(BaseApp.inst().getString(R.string.sync_err));
        public final String value;

        ConnectStatus(String value) {
            this.value = value;
        }
    }

    public State<ConnectStatus> connectStatus = new State<>(ConnectStatus.DEFAULT);


}
