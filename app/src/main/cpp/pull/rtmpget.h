//
// Created by Maggie WU on 2021/8/19.
//

#ifndef RTMPGET_RTMPGET_H
#define RTMPGET_RTMPGET_H

#endif //RTMPGET_RTMPGET_H

#include "../include/librtmp/rtmp.h"
#include <string.h>
#include <android/log.h>
#include <unistd.h>
#include "stddef.h"
#include "stdint.h"
#include "../include/librtmp/rtmp.h"
#include "../include/librtmp/log.h"
#include "unistd.h"
#include <fcntl.h>
#include <stdio.h>

#define LOG "RTMPLOG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)

RTMP* connectRTMP(char * url);
void closeRTMP(RTMP* rtmp);
void readRTMP(RTMP* rtmp);
void rtmpLogCallBack(int level, const char* fmt, va_list arg);

