package com.mrap.loda;

import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

    TextView tv;
    Button btnTriangle;
    Button btnLoad;
    Button btnLoda2;
    Button btnMa2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);

        final MainActivity that = this;

        btnTriangle = findViewById(R.id.btnTriangle);
        if (btnTriangle != null) {
            btnTriangle.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, LodaActivity.class);
                    intent.putExtra("libpath", "libtriangle.so");
                    startActivity(intent);
                }
            });
        }

        btnLoad = findViewById(R.id.btnLoad);
        if (btnLoad != null) {
            btnLoad.setOnClickListener(new View.OnClickListener() {
                //@Override
                //public void onClick(View v) {
                //    LodaJni l = new LodaJni("libsampletoload.so");
                //    String res = l.runLoadLib();
                //    tv.setText(res);
                //}

                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, LodaActivity.class);
                    intent.putExtra("libpath", "/sdcard/loda/libtriangle.so");
                    startActivity(intent);
                }
            });
        }

        btnLoda2 = findViewById(R.id.btnLoda2);
        if (btnLoda2 != null) {
            btnLoda2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, NativeActivity.class);
                    startActivity(intent);
                }
            });
        }

        btnMa2 = findViewById(R.id.btnMa2);
        if (btnMa2 != null) {
            btnMa2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, MainActivity2.class);
                    startActivity(intent);
                }
            });
        }
    }
}
