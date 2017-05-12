#include <jni.h>
#include "testRTSPClient.h"

extern "C"{

JNIEXPORT void JNICALL
Java_com_loften_ffmpegsample_live555_MainLive555_startRTSPClient(JNIEnv *env, jobject instance,
                                                                 jobject surface, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    CRTSPSession *session = new CRTSPSession();
    session->startRTSPClient("hh", path, 4);

    session->_decodeData_cb = [&](int packet){

        LOGE("%d", packet);

    };
    env->ReleaseStringUTFChars(path_, path);
}

}
