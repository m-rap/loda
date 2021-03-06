package com.mrap.loda;

import android.Manifest;
import android.app.Activity;
import android.app.NativeActivity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.support.constraint.ConstraintLayout;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.io.IOException;

public class MainActivity extends Activity {

    TextView txtFile;
    TextView txtErrMsg;
    Button btnLoad;
    Button btnChooseFile;
    public ViewGroup root;
    FileChooser fileChooser;
    //Button btnLoda2;
    //Button btnMa2;

    final int LAUNCH_FILE_CHOOSER = 0;
    final int LAUNCH_LODA= 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        root = findViewById(R.id.mainRoot);
        txtFile = findViewById(R.id.txtFile);
        txtErrMsg = findViewById(R.id.txtErrMsg);

        final MainActivity that = this;

        btnChooseFile = findViewById(R.id.btnChooseFile);
        if (btnChooseFile != null) {
            btnChooseFile.setOnClickListener(new View.OnClickListener() {
                //@Override
                //public void onClick(View v) {
                //    LodaJni l = new LodaJni("libsampletoload.so");
                //    String res = l.runLoadLib();
                //    tv.setText(res);
                //}

                @Override
                public void onClick(View v) {
                    //ConstraintLayout.LayoutParams layoutParams = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
                    //layoutParams.leftToLeft = ConstraintLayout.LayoutParams.MATCH_PARENT;
                    //layoutParams.topToTop = ConstraintLayout.LayoutParams.MATCH_PARENT;
                    //layoutParams.leftMargin = (int)Util.fromDpi(that, 10);
                    //layoutParams.topMargin = (int)Util.fromDpi(that, 10);
                    //
                    //fileChooser = new FileChooser(that, root, layoutParams, root.getZ() + 10, new Util.Consumer<File>() {
                    //    @Override
                    //    public void accept(File file) {
                    //        try {
                    //            txtFile.setText(file.getCanonicalPath());
                    //        } catch (IOException e) {
                    //            e.printStackTrace();
                    //        }
                    //    }
                    //});

                    if (that.checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
                        Intent i = new Intent(that, FileChooser.class);
                        startActivityForResult(i, LAUNCH_FILE_CHOOSER);
                    } else {
                        openAppInfo();
                    }
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
                    try {
                        Intent intent = new Intent(that, LodaActivity.class);
                        //intent.putExtra("libpath", "/sdcard/loda/libtriangle.so");
                        //intent.putExtra("libpath", fileChooser.getChoosenFile().getCanonicalPath());
                        intent.putExtra("libpath", txtFile.getText());
                        startActivityForResult(intent, LAUNCH_LODA);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
        }

        //btnLoda2 = findViewById(R.id.btnLoda2);
        //if (btnLoda2 != null) {
        //    btnLoda2.setOnClickListener(new View.OnClickListener() {
        //        @Override
        //        public void onClick(View v) {
        //            Intent intent = new Intent(that, NativeActivity.class);
        //            startActivity(intent);
        //        }
        //    });
        //}
        //
        //btnMa2 = findViewById(R.id.btnMa2);
        //if (btnMa2 != null) {
        //    btnMa2.setOnClickListener(new View.OnClickListener() {
        //        @Override
        //        public void onClick(View v) {
        //            Intent intent = new Intent(that, MainActivity2.class);
        //            startActivity(intent);
        //        }
        //    });
        //}
    }

    public void openAppInfo() {
        try {
            //Open the specific App Info page:
            Intent intent = new Intent(android.provider.Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
            intent.setData(Uri.parse("package:com.mrap.loda"));
            startActivity(intent);
        } catch ( ActivityNotFoundException e ) {
            //e.printStackTrace();
            //Open the generic Apps page:
            Intent intent = new Intent(android.provider.Settings.ACTION_MANAGE_APPLICATIONS_SETTINGS);
            startActivity(intent);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == LAUNCH_FILE_CHOOSER && resultCode == RESULT_OK) {
            String path = data.getStringExtra("choosenFile");
            txtFile.setText(path);
        }

        if (requestCode == LAUNCH_LODA) {
            if (resultCode == RESULT_OK) {
                String msg = data.getStringExtra("errMsg");
                txtErrMsg.setText(msg);
            } else {
                txtErrMsg.setText("");
            }
        }
    }
}
