#ifndef PROPERTY_CONSUMER_H
#define PROPERTY_CONSUMER_H

#include "stdafx.h"
#include "transport_factory.h"
#include "gui_client_api.h"
#include "dynamic_gui.h"

#define PROPERTY_VALUE_OPTIONS_WIDGET_NAME "Property Value Options"
#define PROPERTY_SET_VALUE_INPUT_WIDGET_NAME "Set Property Input"
#define WIDGET_VALUE_OPTIONS_WIDGET_NAME "Widget Value Options"
#define WIDGET_SET_VALUE_INPUT_WIDGET_NAME "Set Widget Input"

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

    private:
        void HandleMessage();
        void MessageHandlerThread();
        
        /* GUI Client Functions */
        void RunGuiClientTest();
        int32_t GuiClient_SendMessage(const std::vector<uint8_t> &message);
        void GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status);
        void GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList);
        void GuiClient_OnWidgetGetValueReplyReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue, GuiProtocol::WidgetReplyStatus_E status);
        void GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue);

        /* GUI Functions */
        void Gui_OnGuiWindowClosed();
        void Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val);
        void Gui_OnConfigFileSet(bool status);
        void RunSetWidgetValueTest();
        std::unordered_map<uint32_t, std::function<void(WidgetValueVariant_T)>> _widgetCallbacks;
        std::unordered_map<std::string, uint32_t> _widgetNameToIdMap;
        std::string _configFile;

        /* Property Consumer Fucntions */
        void RunPropertyConsumerTest();
        int32_t PropertyConsumer_SendMessage(const std::vector<uint8_t> &message);
        void PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T> &descList);
        void PropertyConsumer_OnPropertyGetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyStorageVariant> &values);
        void PropertyConsumer_OnPropertySetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, PropertyGatherer::PropertyStorageVariant &values);

        std::shared_ptr<TransportInterface> _transport;
        std::shared_ptr<GuiProtocol::GuiClient_C> _guiClient;
        std::shared_ptr<PropertyGatherer::PropertyConsumer_C> _propertyConsumer;
        PortInfo_T _producerInfo;
        PortInfo_T _guiAppInfo;
        std::map<uint16_t, PropertyGatherer::PropertyDescriptor_T> _propertyList;
        bool _isQuit = false;
        bool _widgetListReceived = false;
        uint32_t _rxBufferSize;
        std::string _guiAppDevKey;
        std::string _producerAppDevKey;
        bool _runSetValTest = false;
        DynamicGui_C _gui;
};

#endif // PROPERTY_CONSUMER_H