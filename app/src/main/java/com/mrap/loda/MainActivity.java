package com.mrap.loda;

import android.app.NativeActivity;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    TextView tv;
    Button btnLoad;
    Button btnLoda1;
    Button btnLoda2;
    Button btnMa2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = findViewById(R.id.sample_text);
        //tv.setText(stringFromJNI());

        btnLoad = findViewById(R.id.btnLoad);
        btnLoad.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Loda l = new Loda("libsampletoload.so");
                String res = l.runLoadLib();
                tv.setText(res);
            }
        });

        final MainActivity that = this;

        btnLoda1 = findViewById(R.id.btnLoda1);
        btnLoda1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(that, LodaActivity1.class);
                startActivity(intent);
            }
        });

        btnLoda2 = findViewById(R.id.btnLoda2);
        btnLoda2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(that, NativeActivity.class);
                startActivity(intent);
            }
        });

        btnMa2 = findViewById(R.id.btnMa2);
        btnMa2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(that, MainActivity2.class);
                startActivity(intent);
            }
        });
    }
}
