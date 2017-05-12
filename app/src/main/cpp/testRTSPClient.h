#ifndef FFMPEGSAMPLE_TESTRTSPCLIENT_H
#define FFMPEGSAMPLE_TESTRTSPCLIENT_H

#include "LogUtils.h"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "UsageEnvironment.hh"
#include <pthread.h>
#include <mutex>
#include <string>

using namespace std;
/*
 * 把open和shutdown合在了一个class里面，然后启动一个线程
 * */
class CRTSPSession
{

public:
    function<void(int packet)> _decodeData_cb = NULL;

public:
    CRTSPSession();
    virtual ~CRTSPSession();
    int startRTSPClient(char const* progName, char const* rtspURL, int debugLevel);
    int stopRTSPClient();
    int openURL(UsageEnvironment& env, char const* progName, char const* rtspURL, int debugLevel);


    RTSPClient* m_rtspClient;

    char eventLoopWatchVariable;



    pthread_t tid;


    bool m_running;

    string m_rtspUrl;

    string m_progName;

    int m_debugLevel;


    static void *rtsp_thread_fun (void *param);

    void rtsp_fun();


};

#endif //FFMPEGSAMPLE_TESTRTSPCLIENT_H
