package io.crim.android.ouiconversation.adapter;

import android.util.Log;
import android.view.ViewGroup;

import com.yanzhenjie.permission.AndPermission;
import com.yanzhenjie.permission.runtime.Permission;

import java.util.List;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.sdk.models.Msg;
import io.crim.android.ouicore.base.BaseApp;
import io.crim.android.ouicore.utils.Constant;

public class MessageAdapter extends RecyclerView.Adapter {

    private RecyclerView recyclerView;

    List<Msg> messages;
    boolean hasStorage;

    public MessageAdapter() {
        hasStorage = AndPermission.hasPermissions(BaseApp.inst(), Permission.Group.STORAGE);
    }

    public void setMessages(List<Msg> messages) {
        Log.d("eeeeeee","setMessages=="+messages.size());
        this.messages = messages;
    }

    @Override
    public int getItemViewType(int position) {
//        Log.d("eeeeee","getItemViewType==position="+position+"==getContentType="+messages.get(position).getContentType());
        return messages.get(position).getContentType();
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        return MessageViewHolder.createViewHolder(parent, viewType);
    }

    @Override
    public void onBindViewHolder(@NonNull RecyclerView.ViewHolder holder, int position) {
        Msg message = messages.get(position);

        if (getItemViewType(position) != Constant.LOADING) {
            MessageViewHolder.MsgViewHolder msgViewHolder = (MessageViewHolder.MsgViewHolder) holder;
            msgViewHolder.setMessageAdapter(this);
            msgViewHolder.bindData(message, position);
            if (null != recyclerView)
                msgViewHolder.bindRecyclerView(recyclerView);
        }

    }

    @Override
    public int getItemCount() {
        Log.d("eeeeeeeee","getItemCount=="+( null == messages ? 0 : messages.size()));
        return null == messages ? 0 : messages.size();
    }

    public List<Msg> getMessages() {
        return messages;
    }

    public void bindRecyclerView(RecyclerView recyclerView) {
        this.recyclerView = recyclerView;
    }

    public RecyclerView getRecyclerView() {
        return recyclerView;
    }
}
