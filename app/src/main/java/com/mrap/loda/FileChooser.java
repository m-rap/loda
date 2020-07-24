package com.mrap.loda;

import android.app.Activity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.io.File;

public class FileChooser {
    private Activity activity;
    private ViewGroup view;
    private ViewGroup root;
    private ViewGroup dirItemsContainer;
    private File choosenFile = null;
    Util.Consumer<File> onChoose;

    public FileChooser(Activity activity, ViewGroup root, ViewGroup.LayoutParams layoutParams, float z, Util.Consumer<File> onChoose) {
        //System.out.println("z: " + z);

        this.activity = activity;
        this.root = root;
        this.onChoose = onChoose;

        view = (ViewGroup)this.activity.getLayoutInflater().inflate(R.layout.file_chooser, root, false);
        view.setLayoutParams(layoutParams);
        view.setZ(z);

        //int color1 = Util.getThemeColor(activity, android.R.attr.windowBackground);
        //int color2 = Util.getThemeColor(activity, android.R.attr.popupBackground);
        int color3 = Util.getThemeColor(activity, android.R.attr.colorPrimary);
        //System.out.println(String.format("%08X", color1));
        //System.out.println(String.format("%08X", color2));
        view.setBackgroundColor(color3);

        root.addView(view);

        dirItemsContainer = view.findViewById(R.id.dirItemsContainer);

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
        TextView a = new TextView(this.activity);
        ViewGroup.LayoutParams aLp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        a.setClickable(true);
        a.setLayoutParams(aLp);
        a.setText(f.getName());
        if (f.isDirectory()) {
            a.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    cd(dir + "/" + f.getName());
                }
            });
        } else {
            a.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    choosenFile = f;
                    Util.saveCacheText(activity, "file_chooser-last_dir", dir);
                    onChoose.accept(f);
                    root.removeView(view);
                }
            });
        }
        return a;
    }

    void cd(String dirPath) {
        try {
            File dir = new File(dirPath);
            dirPath = dir.getCanonicalPath();

            System.out.println("cd to " + dirPath);
            File[] files = dir.listFiles();
            dirItemsContainer.removeAllViews();

            File checkParent = new File(dirPath + "/..");
            if (checkParent.canRead()) {
                dirItemsContainer.addView(createFileBtn(dirPath, checkParent));
            }
            for (final File f : files) {
                dirItemsContainer.addView(createFileBtn(dirPath, f));
            }
        } catch (SecurityException ex) {
            ex.printStackTrace();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
}
