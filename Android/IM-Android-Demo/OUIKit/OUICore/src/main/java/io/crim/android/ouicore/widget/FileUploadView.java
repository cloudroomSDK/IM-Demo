package io.crim.android.ouicore.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;

import io.crim.android.ouicore.R;
import io.crim.android.ouicore.utils.GetFilePathFromUri;
import io.crim.android.ouicore.utils.MediaFileUtil;
import io.crim.android.ouicore.utils.OnDedrepClickListener;

public class FileUploadView extends RelativeLayout {
    private ImageView res;
//    private ImageView bgMask;
//    private CirclePgBar circlePgBar;
    private String localUrl = "";

    public FileUploadView(Context context) {
        super(context);
        init();
    }

    public FileUploadView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    void init() {
        res = new ImageView(getContext());
//        bgMask = new ImageView(getContext());
//        bgMask.setImageResource(R.mipmap.ic_file_mask);

        LayoutParams params =
            new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        params.addRule(RelativeLayout.CENTER_IN_PARENT, RelativeLayout.TRUE);
        params.width = 80;
        params.height = 80;
//        circlePgBar = new CirclePgBar(getContext());
//        circlePgBar.setLayoutParams(params);

        addView(res);
//        addView(bgMask);
//        addView(circlePgBar);

        setOnClickListener(new OnDedrepClickListener() {
            @Override
            public void click(View v) {
                GetFilePathFromUri.openFile(getContext(), localUrl);
            }
        });

    }

    public void setRes(String localUrl) {
        this.localUrl = localUrl;
        if (MediaFileUtil.isZIP(localUrl)) res.setImageResource(R.mipmap.ic_file_zip);
        else if (MediaFileUtil.isWord(localUrl)) res.setImageResource(R.mipmap.ic_file_word);
        else if (MediaFileUtil.isPPT(localUrl)) res.setImageResource(R.mipmap.ic_file_ppt);
        else if (MediaFileUtil.isPDF(localUrl)) res.setImageResource(R.mipmap.ic_file_pdf);
        else res.setImageResource(R.mipmap.ic_file_unknown);
    }

    public void setForegroundVisibility(boolean visibility) {
//        bgMask.setVisibility(visibility ? GONE : VISIBLE);
//        circlePgBar.setVisibility(visibility ? GONE : VISIBLE);
//        if (visibility)
//            circlePgBar.reset();
    }

    public void setProgress(int progress) {
//        circlePgBar.setTargetProgress(progress);
    }

}
