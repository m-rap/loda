package com.mrap.loda;

import android.app.NativeActivity;
import android.os.Bundle;

public class LodaActivity1 extends NativeActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    public String getStringExtra(String key) {
        return getIntent().getStringExtra(key);
    }
}
