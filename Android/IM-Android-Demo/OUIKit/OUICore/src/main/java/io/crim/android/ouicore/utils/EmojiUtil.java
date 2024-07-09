package io.crim.android.ouicore.utils;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.method.LinkMovementMethod;
import android.text.style.ImageSpan;
import android.widget.TextView;

import java.util.LinkedHashMap;

public class EmojiUtil {

    private static Context mContext;

    public static void init(Context context) {
        mContext = context;
    }

    public static Bitmap getEmojiBitmap(String emojiName) {
        Bitmap bitmap = null;
        String facePath = "emoji/" + emojiName + ".png";
        try {
            bitmap = BitmapFactory.decodeStream(mContext.getAssets().open(facePath));
        } catch (Exception e) {
            e.printStackTrace();
        }
        return bitmap;
    }

    public static void showSpanTextview(TextView view, String content) {
        SpannableStringBuilder msp = new SpannableStringBuilder(content);
        setEmojiSapn(msp, content);
        view.setText(msp);
        view.setMovementMethod(LinkMovementMethod.getInstance());
    }

    public static void setEmojiSapn(SpannableStringBuilder spannableStringBuilder, String content) {
        for (String key : emojiFaces.keySet()) {
            if (content.contains(key)) {
                String emojiName = emojiFaces.get(key);
                Bitmap bitmap = getEmojiBitmap(emojiName);
                if (bitmap != null) {
                    int startIndex = 0;
                    int index = content.indexOf(key, startIndex);
                    while (index != -1) {
                        int endIndex = index + key.length();
                        startIndex = endIndex;
                        spannableStringBuilder.setSpan(new ImageSpan(bitmap), index, endIndex, Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                        index = content.indexOf(key, startIndex);
                    }
                }
            }
        }
    }

    public static LinkedHashMap<String, String> emojiFaces = new LinkedHashMap<String, String>() {
        {
            put("[天使]", "ic_face_01");
            put("[生气]", "ic_face_02");
            put("[中毒-1]", "ic_face_03");
            put("[中毒]", "ic_face_04");
            put("[迷茫]", "ic_face_05");
            put("[酷-1]", "ic_face_06");
            put("[酷]", "ic_face_07");
            put("[哭-1]", "ic_face_08");
            put("[哭]", "ic_face_09");
            put("[魔鬼]", "ic_face_10");
            put("[头晕]", "ic_face_11");
            put("[面无表情]", "ic_face_12");
            put("[懵逼]", "ic_face_13");
            put("[开心-2]", "ic_face_14");
            put("[开心-1]", "ic_face_15");
            put("[开心]", "ic_face_16");
            put("[热恋]", "ic_face_17");
            put("[受伤]", "ic_face_18");
            put("[哭哭]", "ic_face_19");
            put("[亲吻-1]", "ic_face_20");
            put("[亲吻-2]", "ic_face_21");
            put("[亲吻]", "ic_face_22");
            put("[口罩]", "ic_face_23");
            put("[静音]", "ic_face_24");
            put("[面无表情-1]", "ic_face_25");
            put("[难过-1]", "ic_face_26");
            put("[难过]", "ic_face_27");
            put("[害怕-1]", "ic_face_28");
            put("[害怕]", "ic_face_29");
            put("[闭嘴]", "ic_face_30");
            put("[震惊-1]", "ic_face_31");
            put("[生病]", "ic_face_32");
            put("[睡觉]", "ic_face_33");
            put("[笑-1]", "ic_face_34");
            put("[笑]", "ic_face_35");
            put("[微笑-1]", "ic_face_36");
            put("[眼红]", "ic_face_37");
            put("[奸笑]", "ic_face_38");
            put("[震惊]", "ic_face_39");
            put("[流汗]", "ic_face_40");
            put("[思考]", "ic_face_41");
            put("[疲惫]", "ic_face_42");
            put("[吐舌-2]", "ic_face_43");
            put("[吐舌-1]", "ic_face_44");
            put("[吐舌]", "ic_face_45");
            put("[斜眼]", "ic_face_46");
            put("[呕吐-1]", "ic_face_47");
            put("[暴怒]", "ic_face_48");
            put("[眨眼]", "ic_face_49");
            put("[僵尸]", "ic_face_50");
        }
    };
}
