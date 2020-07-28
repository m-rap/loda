package com.mrap.loda;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class FileChooser extends Activity {
    private Activity activity;
    private ViewGroup view;
    private ViewGroup root;
    private ViewGroup dirItemsContainer;
    private File choosenFile = null;
    Util.Consumer<File> onChoose;
    boolean isModal = false;
    Button btnGotoParent;
    TextView txtCurrDir;

    public FileChooser() {
        super();

        activity = this;
    }

    public FileChooser(Activity activity, ViewGroup root, ViewGroup.LayoutParams layoutParams, float z, Util.Consumer<File> onChoose) {
        //System.out.println("z: " + z);
        isModal = true;

        this.activity = activity;
        this.root = root;
        this.onChoose = onChoose;

        view = (ViewGroup)this.activity.getLayoutInflater().inflate(R.layout.file_chooser, root, false);
        view.setLayoutParams(layoutParams);
        //view.setZ(z); // require api 21

        //int color1 = Util.getThemeColor(activity, android.R.attr.windowBackground);
        //int color2 = Util.getThemeColor(activity, android.R.attr.popupBackground);
        int color3 = Util.getThemeColor(activity, android.R.attr.colorPrimary);
        //System.out.println(String.format("%08X", color1));
        //System.out.println(String.format("%08X", color2));
        view.setBackgroundColor(color3);

        root.addView(view);
        root.invalidate();

        dirItemsContainer = view.findViewById(R.id.dirItemsContainer);
        btnGotoParent = view.findViewById(R.id.btnGotoParent);
        txtCurrDir = view.findViewById(R.id.txtCurrDir);

        String dirPath = Util.getCacheText(activity, "file_chooser-last_dir");
        if (dirPath.isEmpty()) {
            dirPath = "/sdcard";
        }

        cd(dirPath);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.file_chooser);

        dirItemsContainer = findViewById(R.id.dirItemsContainer);
        btnGotoParent = findViewById(R.id.btnGotoParent);
        txtCurrDir = findViewById(R.id.txtCurrDir);

        String dirPath = Util.getCacheText(activity, "file_chooser-last_dir");
        if (dirPath.isEmpty()) {
            dirPath = "/sdcard";
        }

        cd(dirPath);
    }

    public File getChoosenFile() {
        return choosenFile;
    }

    View createFileBtn(final String dir, final File f) {
        LinearLayout l = new LinearLayout(activity);
        l.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        l.setOrientation(LinearLayout.HORIZONTAL);
        l.setBackgroundResource(R.drawable.touchable);
        l.setClickable(true);
        l.setPadding((int)Util.fromDpi(activity, 5), (int)Util.fromDpi(activity, 3), (int)Util.fromDpi(activity, 5), (int)Util.fromDpi(activity, 3));

        TextView textView = new TextView(this.activity);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.setMarginStart((int)Util.fromDpi(activity, 10));
        textView.setLayoutParams(lp);
        textView.setText(f.getName());
        textView.setGravity(Gravity.CENTER_VERTICAL);
        textView.setBackgroundColor(Color.TRANSPARENT);

        ImageView icon = new ImageView(activity);
        icon.setMaxHeight(32);

        if (f.isDirectory()) {
            l.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    cd(dir + "/" + f.getName());
                }
            });
            icon.setImageResource(R.drawable.dir_48);
        } else {
            l.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    choosenFile = f;
                    Util.saveCacheText(activity, "file_chooser-last_dir", dir);
                    if (isModal) {
                        onChoose.accept(f);
                        root.removeView(view);
                    } else {
                        System.out.println("returning");
                        Intent returnIntent = new Intent();
                        try {
                            returnIntent.putExtra("choosenFile", f.getCanonicalPath());
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        setResult(RESULT_OK, returnIntent);
                        finish();
                    }
                }
            });
            icon.setImageResource(R.drawable.file_48);
        }
        l.addView(icon);
        l.addView(textView);
        return l;
    }

    void cd(String dirPath) {
        try {
            File dir = new File(dirPath);
            dirPath = dir.getCanonicalPath();

            //System.out.println("cd to " + dirPath);
            txtCurrDir.setText(dirPath);

            File[] files = dir.listFiles();
            dirItemsContainer.removeAllViews();

            File checkParent = new File(dirPath + "/..");
            if (checkParent.canRead()) {
                final String dst = dirPath + "/..";
                btnGotoParent.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        cd(dst);
                    }
                });
            } else {
                btnGotoParent.setOnClickListener(null);
            }

            List<File> fileList = Arrays.asList(files);
            Collections.sort(fileList, new Comparator<File>() {
                @Override
                public int compare(File o1, File o2) {
                    if (o1.isDirectory() && !o2.isDirectory()) {
                        return -1;
                    }
                    if (!o1.isDirectory() && o2.isDirectory()) {
                        return 1;
                    }
                    return o1.getName().toUpperCase().compareTo(o2.getName().toUpperCase());
                }
            });

            for (final File f : fileList) {
                dirItemsContainer.addView(createFileBtn(dirPath, f));
            }
        } catch (SecurityException ex) {
            ex.printStackTrace();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void onBackPressed() {
        System.out.println("returning");
        Intent returnIntent = new Intent();
        returnIntent.putExtra("choosenFile", "");
        setResult(RESULT_CANCELED, returnIntent);
        super.onBackPressed();
    }
}
