#ifndef PROPERTY_PRODUCER_H
#define PROPERTY_PRODUCER_H

#include "stdafx.h"
#include "transport_factory.h"
#include "property_producer_api.h"
#include "dynamic_gui.h"

struct PropertyProducerInitParams_C
{
    PortInfo_T myInfo;
    std::string configFile;
};

class PropertyProducerApp_C
{
    public:
        PropertyProducerApp_C(PropertyProducerInitParams_C initParams);
        ~PropertyProducerApp_C();
        void RunTest();

    private:
        /* App Functions & Variables */
        void HandleMessage();
        std::shared_ptr<TransportInterface> _transport;
        bool _isQuit = false;
        uint32_t _rxBufferSize;

        /* Property Producer Test Functions & Variables */
        void RunProducerTest();
        int32_t PropertyProducer_SendMessage(const std::vector<uint8_t> &message);
        void PropertyProducer_OnPropertyListRequestReceived(const std::vector<PropertyGatherer::PropertyDescriptor_T> &descList);
        PropertyGatherer::PropertyGathererReplyStatus_E PropertyProducer_OnPropertyGetValueRequestRecieved(std::vector<PropertyGatherer::PropertyValueContainer_T>& values);
        PropertyGatherer::PropertyGathererReplyStatus_E PropertyProducer_OnPropertySetValueRequestRecieved(PropertyGatherer::PropertyValueContainer_T& value);
        std::shared_ptr<PropertyGatherer::PropertyProducer_C> _propertyProducer;
        PortInfo_T _propertyConsumerInfo;
        std::string _propertyConsumerDevKey;
        std::map<uint16_t, PropertyGatherer::PropertyStorageVariant> _propertyValuesMap;
        bool _runSetValTest = false;
        
        /* Gui Test Functions & Variables */
        void Gui_OnGuiWindowClosed();
        void Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val);
        DynamicGui_C _gui;
        bool _widgetListReceived = false;
};

#endif // PROPERTY_PRODUCER_H