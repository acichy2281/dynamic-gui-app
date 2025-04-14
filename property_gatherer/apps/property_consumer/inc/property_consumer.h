#ifndef PROPERTY_CONSUMER_H
#define PROPERTY_CONSUMER_H

#include "stdafx.h"
#include "transport_factory.h"
#include "gui_client_api.h"
#include "dynamic_gui.h"

/* Property Consumer widget macros */
#define PROPERTY_CONSUMER_THREAD_BUTTON "Start Property Consumer Thread"
#define GET_PROPERTY_LIST_BUTTON "Get Property List"
#define PROPERTY_VALUE_OPTIONS_WIDGET_NAME "Property Value Options"
#define PROPERTY_VALUE_OUTPUT_WIDGET_NAME "Get Property Value Output"
#define GET_PROPERTY_VALUE_BUTTON "Get Property Value"
#define PROPERTY_SET_VALUE_INPUT_WIDGET_NAME "Set Property Input"
#define SET_PROPERTY_VALUE_BUTTON "Set Property Value"

/* GUI Client Widget macros */
#define GUI_CLIENT_THREAD_BUTTON "Start Gui Client Thread"
#define GET_WIDGET_LIST_BUTTON "Get Widget List"
#define WIDGET_VALUE_OPTIONS_WIDGET_NAME "Widget Value Options"
#define WIDGET_VALUE_OUTPUT_WIDGET_NAME "Get Widget Value Output"
#define GET_WIDGET_VALUE_BUTTON "Get Widget Value"
#define WIDGET_SET_VALUE_INPUT_WIDGET_NAME "Set Widget Input"
#define SET_WIDGET_VALUE_BUTTON "Set Widget Value"
#define ADD_WIDGET_WIDGET_NAME "Add Widget Button"

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
        void GuiClient_OnAddWidgetReplyReceived(GuiProtocol::WidgetReplyStatus_E status);

        /* GUI Client Test Functions */
        void RunWidgetListRequestTest();
        void RunWidgetGetValueRequestTest();
        void RunWidgetSetValueRequestTest();
        void RunAddWidgetRequestTest();
        std::vector<nlohmann::json> _propertyWidgetDataList;

        /* GUI Functions */
        void Gui_OnGuiWindowClosed();
        void Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val);
        void Gui_OnConfigFileSet(bool status);
        std::unordered_map<uint32_t, std::function<void(WidgetValueVariant_T)>> _widgetCallbacks;
        std::unordered_map<std::string, uint32_t> _widgetNameToIdMap;
        std::string _configFile;

        /* Property Consumer Fucntions */
        void RunPropertyConsumerTest();
        int32_t PropertyConsumer_SendMessage(const std::vector<uint8_t> &message);
        void PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T> &descList);
        void PropertyConsumer_OnPropertyGetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyStorageVariant> &values);
        void PropertyConsumer_OnPropertySetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, PropertyGatherer::PropertyStorageVariant &values);

        /* Property Consumer Test Functions */
        void RunPropertyListRequestTest();
        void RunPropertyGetValueRequestTest();
        void RunPropertySetValueRequestTest();
        


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
        bool _propertyConsumerStarted = false;
        bool _guiClientStarted = false;
        DynamicGui_C _gui;
};

#endif // PROPERTY_CONSUMER_H