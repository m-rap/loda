package com.mrap.loda;

import java.io.PrintStream;

public class LodaJni {

    String _libPath;

    public LodaJni(String libPath) {
        _libPath = libPath;
    }

    private native String loadLib(String libPath, PrintStream printStream);

    public String runLoadLib() {
        System.out.println("runLoadLib");
        String res = loadLib(_libPath, System.out);
        return res;
    }

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}