package io.crim.android.ouicore.base;

import android.widget.Toast;

/**
 * View
 */
public interface IView {
    void onError(String error);

    void onSuccess(Object body);

    void toast(String tips);

    void close();
}
