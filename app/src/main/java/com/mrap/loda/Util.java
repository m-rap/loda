package com.mrap.loda;

import android.app.Activity;
import android.graphics.Color;
import android.util.TypedValue;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;

public class Util {
    interface Consumer<T> {
        void accept(T t);
    }

    public static float fromDpi(Activity activity, float val) {
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, val, activity.getResources().getDisplayMetrics());
    }

    public static String getCacheText(Activity activity, String key) {
        try {
            File f = new File(activity.getCacheDir() + "/" + key);
            f.createNewFile();
            FileReader reader = new FileReader(f);
            StringBuilder sb = new StringBuilder();
            while (true) {
                int ch = reader.read();
                if (ch == -1) {
                    break;
                }
                sb.append((char)ch);
            }
            reader.close();
            return sb.toString();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }

    public static void saveCacheText(Activity activity, String key, String value) {
        try {
            File f = new File(activity.getCacheDir() + "/" + key);
            f.createNewFile();
            FileOutputStream fos = new FileOutputStream(f, false);
            PrintWriter writer = new PrintWriter(fos);
            writer.print(value);
            writer.close();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static TypedValue tv = new TypedValue();
    public static int getThemeColor(Activity activity, int attr) {
        activity.getTheme().resolveAttribute(android.R.attr.windowBackground, tv, true);
        if (tv.type >= TypedValue.TYPE_FIRST_COLOR_INT && tv.type <= TypedValue.TYPE_LAST_COLOR_INT) {
            return tv.data;
        } else {
            return Color.parseColor("#ffffff");
        }
    }
}
