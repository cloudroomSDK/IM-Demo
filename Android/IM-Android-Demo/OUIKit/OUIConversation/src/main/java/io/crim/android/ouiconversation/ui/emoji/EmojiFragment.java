package io.crim.android.ouiconversation.ui.emoji;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import java.util.ArrayList;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import io.crim.android.ouiconversation.databinding.FragmentEmojiBinding;
import io.crim.android.ouiconversation.databinding.ItemEmojiBinding;
import io.crim.android.ouiconversation.vm.ChatVM;
import io.crim.android.ouicore.adapter.RecyclerViewAdapter;
import io.crim.android.ouicore.base.BaseFragment;
import io.crim.android.ouicore.utils.EmojiUtil;

/**
 * Created by zjw on 2024/3/27.
 */
public class EmojiFragment extends BaseFragment<ChatVM> {

    private FragmentEmojiBinding v;
    private EmojiClickListener mEmojiClickListener;

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        v = FragmentEmojiBinding.inflate(inflater);
        return v.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        init();
    }

    public void setChatVM(ChatVM vm) {
        this.vm = vm;
    }

    private void init() {
        v.customEmoji.setLayoutManager(new GridLayoutManager(getContext(), 8));
        RecyclerViewAdapter adapter = new RecyclerViewAdapter<Object, EmojieHolder>(EmojieHolder.class) {

            @Override
            public void onBindView(@NonNull EmojieHolder holder, Object data, int position) {
                int index = position + 1;
                String faceIndex;
                if (index < 10) {
                    faceIndex = "0" + index;
                } else {
                    faceIndex = "" + index;
                }
                String emojijName = "ic_face_" + faceIndex;
                Bitmap bitmap = EmojiUtil.getEmojiBitmap(emojijName);
                if (bitmap != null) {
                    holder.v.emojiIv.setImageBitmap(bitmap);
                }
                holder.v.emojiIv.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        if (mEmojiClickListener != null) {
                            mEmojiClickListener.click((String) data);
                        }
                    }
                });

            }
        };
        v.customEmoji.setAdapter(adapter);
        adapter.setItems(new ArrayList<>(EmojiUtil.emojiFaces.keySet()));
    }

    public static class EmojieHolder extends RecyclerView.ViewHolder {
        public ItemEmojiBinding v;

        public EmojieHolder(@NonNull View itemView) {
            super(ItemEmojiBinding.inflate(LayoutInflater.from(itemView.getContext())).getRoot());
            v = ItemEmojiBinding.bind(this.itemView);
        }
    }

    public interface EmojiClickListener {
        void click(String name);
    }

    public void setEmojiClickListener(EmojiClickListener listener) {
        mEmojiClickListener = listener;
    }
}
