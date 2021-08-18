#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/log.h>
extern "C"{
#include "rtmp.h"
#include <log.h>
}
#define LOG "RTMPLOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
void Logcb(int level, const char* fmt, va_list arg){
    char log[1024];
    vsprintf(log,fmt,arg);
    LOGI("[RTMPLOG] %s",log);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_rtmpget_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    RTMP_LogSetCallback((RTMP_LogCallback*)Logcb);
    RTMP_debuglevel = RTMP_LOGALL;

    char version[100];
    sprintf(version, "rtmpdump version： %d", RTMP_LibVersion());
    LOGI("rtmpdump version：%d",RTMP_LibVersion());

    int res = 0;
    RTMP* rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);

    //rtmp->Link.timeout = 5;
    // rtmp://host[:port]/app[/appinstance][/...]
    // application = app[/appinstance]
    res = RTMP_SetupURL(rtmp, "rtmp://161.189.48.185:1935/rtmplive/stream_1");
    if (res == 0){
        LOGI("RTMP_SetupURL error.\n") ;
    };

    res = RTMP_Connect(rtmp, NULL);
    if (res == 0){
        LOGI("RTMP_Connect error.  %s \n",rtmp->Link.hostname.av_val) ;
    };


    res = RTMP_ConnectStream(rtmp,0);
    if (res == 0){
        LOGI("RTMP_ConnectStream error.\n") ;
    };

    int nRead = 0, NRead = 0;
    int bufsize = 1280*720;

    char* buf = (char*)malloc(bufsize);
    FILE* fp_save = fopen("save.flv", "wb");
    while(nRead=RTMP_Read(rtmp,buf,bufsize))
    {
        fwrite(buf,1,nRead,fp_save);
        NRead += nRead;
        LOGI("Receive: %5dByte, Total: %5.2fkB\n",nRead,NRead*1.0/1024) ;
        RTMP_LogPrintf("Receive: %5dByte, Total: %5.2fkB\n",nRead,NRead*1.0/1024);
    }
    sleep(5);
    RTMP_Close(rtmp);
    RTMP_Free(rtmp);
    return env->NewStringUTF(version);
}

