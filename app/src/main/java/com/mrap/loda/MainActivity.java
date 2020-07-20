package com.mrap.loda;

import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
//import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

//public class MainActivity extends AppCompatActivity {
public class MainActivity extends Activity {

    TextView tv;
    Button btnLoad;
    Button btnLoda1;
    Button btnLoda2;
    Button btnMa2;
    Button btnNatAct;
    Button btnTriangle;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI());

        btnLoad = findViewById(R.id.btnLoad);
        if (btnLoad != null) {
            btnLoad.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Loda l = new Loda("libsampletoload.so");
                    String res = l.runLoadLib();
                    tv.setText(res);
                }
            });
        }

        final MainActivity that = this;

        btnLoda1 = findViewById(R.id.btnLoda1);
        if (btnLoda1 != null) {
            btnLoda1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                    Intent intent = new Intent(that, LodaActivity1.class);
                    intent.putExtra("libpath", "libsampletoload.so");
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

        btnNatAct = findViewById(R.id.btnNatAct);
        if (btnNatAct != null) {
            btnNatAct.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, LodaActivity1.class);
                    intent.putExtra("libpath", "libnative-activity.so");
                    startActivity(intent);
                }
            });
        }

        btnTriangle = findViewById(R.id.btnTriangle);
        if (btnTriangle != null) {
            btnTriangle.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(that, LodaActivity1.class);
                    intent.putExtra("libpath", "libtriangle.so");
                    startActivity(intent);
                }
            });
        }
    }
}
