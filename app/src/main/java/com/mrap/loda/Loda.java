package com.mrap.loda;

public class Loda {

    String _libPath;

    public Loda(String libPath) {
        _libPath = libPath;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String loadLib(String libPath);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
