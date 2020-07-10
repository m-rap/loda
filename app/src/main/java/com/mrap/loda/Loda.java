package com.mrap.loda;

import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Loda {

    String _libPath;
    byte[] _buff = new byte[512];
    long[] _buffTime = new long[] {0};
    long _prevBuffTime = 0;
    ScheduledExecutorService _es;

    Runnable _printerRunner = new Runnable() {
        @Override
        public void run() {
            if (_prevBuffTime != _buffTime[0]) {
                int len = 0;
                for (; len < 512 && _buff[len] != '\0'; len++);
                System.out.println("from native: " + _buffTime[0] + " " + new String(_buff, 0, len));
                _prevBuffTime = _buffTime[0];
            }
        }
    };

    public Loda(String libPath) {
        _libPath = libPath;

        _buff[0] = '\0';

        _es = Executors.newSingleThreadScheduledExecutor();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    private native String loadLib(String libPath, byte[] buff, long[] buffTime);

    public String runLoadLib() {
        System.out.println("runLoadLib");
        _es.scheduleAtFixedRate(_printerRunner, 0, 1, TimeUnit.MILLISECONDS);
        String res = loadLib(_libPath, _buff, _buffTime);
        _es.shutdown();
        _es = Executors.newSingleThreadScheduledExecutor();
        return res;
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
