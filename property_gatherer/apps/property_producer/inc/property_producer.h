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
        int32_t PropertyProducer_SendMessage(const std::vector<uint8_t> &message);
        void PropertyProducer_OnPropertyListRequestReceived(std::vector<PropertyGatherer::PropertyDescriptor_T> &descList);
        PropertyGatherer::PropertyReplyStatus_E PropertyProducer_OnPropertyGetValueRequestRecieved(std::vector<PropertyGatherer::PropertyStorageVariant> &values);
        void HandleMessage();
        void Gui_OnGuiWindowClosed();
        void Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val);
        void RunProducerTest();
        std::shared_ptr<TransportInterface> _transport;
        std::shared_ptr<PropertyGatherer::PropertyProducer_C> _propertyProducer;
        PortInfo_T _propertyConsumerInfo;
        std::string _propertyConsumerDevKey;
        bool _isQuit = false;
        bool _widgetListReceived = false;
        uint32_t _rxBufferSize;
        bool _runSetValTest = false;
        DynamicGui_C _gui;
};

#endif // PROPERTY_PRODUCER_H