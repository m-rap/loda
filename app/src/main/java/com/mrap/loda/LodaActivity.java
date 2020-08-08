package com.mrap.loda;

import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;

public class LodaActivity extends NativeActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    public String getStringExtra(String key) {
        return getIntent().getStringExtra(key);
    }

    public void finishWithError(String errMsg) {
        Intent returnIntent = new Intent();
        returnIntent.putExtra("errMsg", errMsg);
        setResult(RESULT_OK, returnIntent);
        finish();
    }

    @Override
    public void onBackPressed() {
        Intent returnIntent = new Intent();
        setResult(RESULT_CANCELED, returnIntent);
        super.onBackPressed();
    }
}
