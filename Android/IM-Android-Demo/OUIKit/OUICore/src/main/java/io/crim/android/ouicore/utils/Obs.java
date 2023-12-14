package io.crim.android.ouicore.utils;

import java.util.Observable;
import java.util.Observer;

/**
 * 观察者
 */
public class Obs extends Observable {
    private static Obs observer = null;

    public synchronized static Obs inst() {
        if (observer == null) {
            observer = new Obs();
        }
        return observer;
    }

    public static void newMessage(int tag) {
        inst().setMessage(new Msg(tag));
    }

    public static void newMessage(int tag, Object object) {
        inst().setMessage(new Msg(tag, object));
    }

    private void setMessage(Msg message) {
        observer.setChanged();
        observer.notifyObservers(message);
    }

    public static class Msg {
        public int tag;
        public Object object;

        public Msg(int tag) {
            this.tag = tag;
        }

        public Msg(int tag, Object object) {
            this.tag = tag;
            this.object = object;
        }
    }

}

