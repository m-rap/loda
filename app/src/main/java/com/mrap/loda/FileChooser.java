package com.mrap.loda;

import android.app.Activity;
import android.graphics.Color;
import android.support.constraint.ConstraintLayout;
import android.util.TypedValue;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class FileChooser {
    Activity owner;
    View view;
    ViewGroup dirItemsContainer;

    float fromDpi(float val) {
        return TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, val, owner.getResources().getDisplayMetrics());
    }

    public FileChooser(Activity activity, ViewGroup root) {
        owner = activity;
        view = owner.getLayoutInflater().inflate(R.layout.file_chooser, root, false);

        ConstraintLayout.LayoutParams layoutParams = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        layoutParams.leftToLeft = ConstraintLayout.LayoutParams.MATCH_PARENT;
        layoutParams.topToTop = ConstraintLayout.LayoutParams.MATCH_PARENT;
        layoutParams.leftMargin = (int)fromDpi(10);
        layoutParams.topMargin = (int)fromDpi(10);

        view.setLayoutParams(layoutParams);
        view.setElevation((int)fromDpi(2));
        TypedValue tv = new TypedValue();
        owner.getTheme().resolveAttribute(android.R.attr.windowBackground, tv, true);
        if (tv.type >= TypedValue.TYPE_FIRST_COLOR_INT && tv.type <= TypedValue.TYPE_LAST_COLOR_INT) {
            view.setBackgroundColor(tv.data);
        } else {
            view.setBackgroundColor(Color.parseColor("#fff"));
        }

        root.addView(view);
        dirItemsContainer = view.findViewById(R.id.dirItemsContainer);
        TextView a = new TextView(owner);
        a.setText("lalalala");
        dirItemsContainer.addView(a);
    }
}
