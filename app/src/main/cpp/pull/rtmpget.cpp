//
// Created by Maggie WU on 2021/8/19.
//
#include "rtmpget.h"

void rtmpLogCallBack(int level, const char* fmt, va_list arg){
    char log[1024];
    vsprintf(log,fmt,arg);
    LOGD("[RTMPLOG] %s",log);
}

RTMP* connectRTMP(char * url){
    RTMP_LogSetCallback((RTMP_LogCallback*)rtmpLogCallBack);
    RTMP_debuglevel = RTMP_LOGINFO; // 这里可以修改日志级别打印更多RTMP日志

    LOGD("rtmpdump version：%d",RTMP_LibVersion());

    int res = 0;
    RTMP* rtmp = RTMP_Alloc();
    RTMP_Init(rtmp);

    //rtmp->Link.timeout = 5;
    // rtmp://host[:port]/app[/appinstance][/...]
    // application = app[/appinstance]
    res = RTMP_SetupURL(rtmp, url);
    if (res == 0){
        LOGD("RTMP_SetupURL error.\n") ;
        closeRTMP(rtmp);
        return nullptr;
    };

    res = RTMP_Connect(rtmp, NULL);
    if (res == 0){
        LOGD("RTMP_Connect error.  %s \n",rtmp->Link.hostname.av_val) ;
        closeRTMP(rtmp);
        return nullptr;
    };

    res = RTMP_ConnectStream(rtmp,0);
    if (res == 0){
        LOGD("RTMP_ConnectStream error.\n") ;
        closeRTMP(rtmp);
        return nullptr;
    };
    return rtmp;
}

void readRTMP(RTMP* rtmp){
    RTMPPacket pc = { 0 };
    bool bFirst = true;
    unsigned char result = 0;
    char *data = NULL;
    bool bVideoFirst = true;
    FILE * fp = fopen("/sdcard/receive.h264","wb");
    if(!fp){
        LOGD("Open File Error.\n");
        return;
    }

    LOGD("********** Start Read RTMP Paket ********\n");
    while (RTMP_ReadPacket(rtmp,&pc)){
        if (RTMPPacket_IsReady(&pc))  //是否读取完毕
        {
            if (pc.m_packetType == RTMP_PACKET_TYPE_VIDEO && RTMP_ClientPacket(rtmp, &pc))
            {
                bool bIsKeyFrame = false;
                LOGD("Receive: vedio packet\n");
                result = pc.m_body[0];
                data = pc.m_body;
                if (result == 0x17)//I frame
                {
                    bIsKeyFrame = true;
                }
                else if (result == 0x27)
                {
                    bIsKeyFrame = false;
                }
                static unsigned char const start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
                if (bVideoFirst)
                {
                    fwrite(start_code, 1, 4, fp);
                    int spsnum = data[10] & 0x1f;
                    int number_sps = 11;
                    int count_sps = 1;
                    while (count_sps <= spsnum){
                        int spslen = (data[number_sps] & 0x000000FF) << 8 | (data[number_sps + 1] & 0x000000FF);
                        number_sps += 2;
                        fwrite(data + number_sps, 1, spslen, fp);
                        fwrite(start_code, 1, 4, fp);
                        number_sps += spslen;
                        count_sps++;
                    }
                    //Get PPS
                    int ppsnum = data[number_sps] & 0x1f;
                    int number_pps = number_sps + 1;
                    int count_pps = 1;
                    while (count_pps <= ppsnum){
                        int ppslen = (data[number_pps] & 0x000000FF) << 8 | data[number_pps + 1] & 0x000000FF;
                        number_pps += 2;
                        fwrite(data + number_pps, 1, ppslen, fp);
                        fwrite(start_code, 1, 4, fp);
                        number_pps += ppslen;
                        count_pps++;
                    }
                    bVideoFirst = false;
                }else
                {
                    //AVCNALU
                    int len = 0;
                    int num = 5;
                    while (num<pc.m_nBodySize)
                    {
                        len = (data[num] & 0x000000FF) << 24 | (data[num + 1] & 0x000000FF) << 16 | (data[num + 2] & 0x000000FF) << 8 | data[num + 3] & 0x000000FF;
                        num = num + 4;
                        fwrite(data + num, 1, len, fp);
                        fwrite(start_code, 1, 4, fp);
                        num = num + len;
                    }
                }

            }
        }
    };
}

void closeRTMP(RTMP* rtmp){
    RTMP_Close(rtmp);
    RTMP_Free(rtmp);
};

