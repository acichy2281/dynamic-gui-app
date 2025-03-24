#ifndef PROPERTY_CONSUMER_H
#define PROPERTY_CONSUMER_H

#include "stdafx.h"
#include "transport_factory.h"
#include "gui_client_api.h"
#include "dynamic_gui.h"

struct PropertyConsumerInitParams_C
{
    PortInfo_T producerInfo;
    PortInfo_T guiAppInfo;
    std::string myIp;
    uint16_t myPort;
};

class PropertyConsumer_C
{
    public:
        PropertyConsumer_C(PropertyConsumerInitParams_C initParams);
        ~PropertyConsumer_C();
        void RunTest();
        void RunSetValueTest();

    private:
        int32_t GuiClient_SendMessage(const std::vector<uint8_t> &message);
        void GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status);
        void GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList);
        void GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue);
        void HandleMessage();
        std::shared_ptr<TransportInterface> _transport;
        std::shared_ptr<GuiProtocol::GuiClient_C> _guiClient;
        PortInfo_T _producerInfo;
        PortInfo_T _guiAppInfo;
        bool _isQuit = false;
        bool _widgetListReceived = false;
        uint32_t _rxBufferSize;
        std::string _guiAppDevKey;
        std::string _producerAppDevKey;
        bool _runSetValTest = false;
        DynamicGui_C _gui;
};

#endif // PROPERTY_CONSUMER_H