#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
#include "pull/rtmpget.h"
#include "pthread.h"
//extern "C"{
//#include "rtmp.h"
//#include <log.h>
//}
//#define LOG "RTMPLOG"
//#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
pthread_t readThread;
RTMP* rtmp;

void *getRTMP(void* data){
    readRTMP(rtmp);
    pthread_exit(&readThread);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rtmpget_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    char version[100];
    sprintf(version, "rtmpdump version： %d", RTMP_LibVersion());
    rtmp = connectRTMP("rtmp://161.189.48.185:1935/rtmplive/stream_1");

    pthread_create(&readThread,NULL,getRTMP,NULL);

    sleep(5);
    closeRTMP(rtmp);
    return env->NewStringUTF(version);
}





