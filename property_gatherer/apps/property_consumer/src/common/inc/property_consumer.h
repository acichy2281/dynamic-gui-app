#ifndef PROPERTY_CONSUMER_H
#define PROPERTY_CONSUMER_H

#include "stdafx.h"
#include "transport_factory.h"

struct PropertyConsumerInitParams_C
{
    PortInfo_T producerInfo;
    PortInfo_T guiAppInfo;
    std::string myIp;
    uint16_t myPort;
};

class PropertyConsumer_C : GuiProtocol::GuiClient_C
{
    public:
        PropertyConsumer_C(PropertyConsumerInitParams_C initParams);
        ~PropertyConsumer_C();
        void RunTest();
        void UpdateGuiWidget();

    private:
        int32_t GuiClient_SendMessage(const std::vector<uint8_t> &message) override;
        void GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status) override;
        void HandleMessage();
        std::shared_ptr<TransportInterface> _transport;
        PortInfo_T _producerInfo;
        PortInfo_T _guiAppInfo;
        bool _isQuit = false;
        bool _widgetListReceived = false;
        uint32_t _rxBufferSize;
        std::string _guiAppDevKey;
        std::string _producerAppDevKey;
};

#endif // PROPERTY_CONSUMER_H