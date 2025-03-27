#include "stdafx.h"
#include "property_producer.h"

int main(int argc, char** argv)
{
    PropertyProducerInitParams_C initParams;
    initParams.myInfo.destIp = "127.0.0.1";
    initParams.myInfo.destPort = 8002;

    PropertyProducerApp_C app(initParams);
    app.RunTest();
    return 0;
}