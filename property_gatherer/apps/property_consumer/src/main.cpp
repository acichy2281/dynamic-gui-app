#include "stdafx.h"
#include "property_consumer.h"

int main(int argc, char **argv)
{
    PropertyConsumerInitParams_C initParams;
    initParams.guiAppInfo.destIp = "127.0.0.1";
    initParams.guiAppInfo.destPort = 8001;
    initParams.myInfo.destIp = "127.0.0.1";
    initParams.myInfo.destPort = 8000;
    initParams.producerInfo.destIp = "127.0.0.1";
    initParams.producerInfo.destPort = 8002;

    PropertyConsumerApp_C app(initParams);
    app.RunTest();
    return 0;
}