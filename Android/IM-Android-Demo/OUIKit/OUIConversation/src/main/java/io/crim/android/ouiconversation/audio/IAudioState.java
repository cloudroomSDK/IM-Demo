package io.crim.android.ouiconversation.audio;

public abstract class IAudioState {
    public IAudioState() {
    }

    void enter() {
    }

    abstract void handleMessage(AudioStateMessage var1);
}
