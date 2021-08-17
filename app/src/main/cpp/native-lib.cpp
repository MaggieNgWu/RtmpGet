#include <jni.h>
#include <string>
extern "C"{
#include "rtmp.h"
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rtmpget_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    char version[100];
    sprintf(version, "rtmpdump version： %d", RTMP_LibVersion());


    return env->NewStringUTF(version);
}