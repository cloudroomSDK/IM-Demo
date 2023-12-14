package io.crim.android.ouicore.vm;

import java.util.HashMap;

import io.crim.android.ouicore.base.vm.State;
import io.crim.android.ouicore.base.vm.injection.BaseVM;
import io.crim.android.ouicore.net.RXRetrofit.N;
import io.crim.android.ouicore.net.RXRetrofit.NetObserver;
import io.crim.android.ouicore.net.RXRetrofit.Parameter;
import io.crim.android.ouicore.api.OneselfService;
import io.crim.android.sdk.CRIMClient;
import io.crim.android.sdk.listener.OnCustomBusinessListener;

public class NotificationVM extends BaseVM implements OnCustomBusinessListener {

    public State<String> customBusinessMessage = new State<>();
    public State<Integer> momentsUnread = new State<>();

    public NotificationVM() {
        CRIMClient.getInstance().setCustomBusinessListener(this);
    }

    @Override
    public void onRecvCustomBusinessMessage(String s) {
        getWorkMomentsUnReadCount();
        customBusinessMessage.setValue(s);
    }

    public void getWorkMomentsUnReadCount() {
        N.API(OneselfService.class).getMomentsUnreadCount(new Parameter().buildJsonBody())
            .map(OneselfService.turn(HashMap.class))
            .compose(N.IOMain()).subscribe(new NetObserver<HashMap>(tag()) {
            @Override
            public void onSuccess(HashMap map) {
                try {
                    int size = (int) map.get("total");
                    momentsUnread.setValue(size);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }

            @Override
            protected void onFailure(Throwable e) {
                toast(e.getMessage());
            }
        });
    }

    @Override
    protected void onCleared() {
        super.onCleared();
        N.clearDispose(tag());
    }
}
