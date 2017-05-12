package com.loften.ffmpegsample.live555;

import android.view.Surface;

/**
 * Live555.
 */

public class MainLive555 {
    static {
        System.loadLibrary("ffmpeg-lib");
    }

    public native void startRTSPClient(Surface surface, String path);

}
