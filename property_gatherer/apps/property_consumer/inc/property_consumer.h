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
    PortInfo_T myInfo;
    std::string configFile;
};

class PropertyConsumerApp_C
{
    public:
        PropertyConsumerApp_C(PropertyConsumerInitParams_C initParams);
        ~PropertyConsumerApp_C();
        void RunTest();
        void RunSetValueTest();

    private:
        void HandleMessage();
        void MessageHandlerThread();
        
        /* GUI Client Functions */
        void RunGuiClientTest();
        int32_t GuiClient_SendMessage(const std::vector<uint8_t> &message);
        void GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status);
        void GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList);
        void GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue);

        /* GUI Functions */
        void Gui_OnGuiWindowClosed();
        void Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val);

        /* Property Consumer Fucntions */
        void RunPropertyConsumerTest();
        int32_t PropertyConsumer_SendMessage(const std::vector<uint8_t> &message);
        void PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T> &descList);
        void PropertyConsumer_OnPropertyGetValueReplyRecieved(PropertyGatherer::PropertyReplyStatus_E status, std::vector<PropertyGatherer::PropertyStorageVariant> &values);

        std::shared_ptr<TransportInterface> _transport;
        std::shared_ptr<GuiProtocol::GuiClient_C> _guiClient;
        std::shared_ptr<PropertyGatherer::PropertyConsumer_C> _propertyConsumer;
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