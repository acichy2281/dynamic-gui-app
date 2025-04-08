#include "stdafx.h"
#include "property_producer.h"

PropertyProducerApp_C::PropertyProducerApp_C(PropertyProducerInitParams_C initParams) : 
    _propertyProducer(std::make_shared<PropertyGatherer::PropertyProducer_C>())
{
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myInfo.destIp, initParams.myInfo.destPort);
    _rxBufferSize = 2048;
    if (false == initParams.configFile.empty()) _gui.SetConfigFile(initParams.configFile);
}

PropertyProducerApp_C::~PropertyProducerApp_C()
{

}

void PropertyProducerApp_C::Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val)
{
    std::cout << "Widget event callback: Widget ID = " << widgetDesc.widgetId << ", Value = ";
    std::visit([](auto&& arg) { std::cout << arg; }, val);
    std::cout << "\n";
}

void PropertyProducerApp_C::Gui_OnGuiWindowClosed()
{
    std::cout << "GUI window closed\n";
    _isQuit = true;
}

void PropertyProducerApp_C::Gui_OnConfigFileSet(bool status)
{
    if (true == status)
    {
        std::cout << "Config file set successfully\n";
    }
}

void PropertyProducerApp_C::RunTest()
{
    std::cout << "Property Producer Test\n";
    
    DynamicGuiInitParams_T initParams;
    initParams.callbacks.onWidgetEventOccured = std::bind(&PropertyProducerApp_C::Gui_OnWidgetEvent, this, std::placeholders::_1, std::placeholders::_2);
    initParams.callbacks.onWindowClose = std::bind(&PropertyProducerApp_C::Gui_OnGuiWindowClosed, this);
    initParams.callbacks.onConfigFileSet = std::bind(&PropertyProducerApp_C::Gui_OnConfigFileSet, this, std::placeholders::_1);
    if (false == _gui.InitializeGui(initParams))
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
    }
    else
    {
        std::thread producerThread(&PropertyProducerApp_C::RunProducerTest, this);  
        _gui.RunGui();
        producerThread.join();
    }
}

void PropertyProducerApp_C::RunProducerTest()
{
    /* Generate Test Property List */
    auto propertyDescList = std::vector<PropertyGatherer::PropertyDescriptor_T>();
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(0, false, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "Name"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(1, true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "TestString"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(2, true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Unsigned8BitInt, 30, "TestInt"));
    
    /* Populate Callbacks for Property Gatherer */
    auto callBacks = PropertyGatherer::PropertyProducerCallbacks_T();
    callBacks.sendMessage = std::bind(&PropertyProducerApp_C::PropertyProducer_SendMessage, this, std::placeholders::_1);
    callBacks.onPropertyListRequestReceived = std::bind(&PropertyProducerApp_C::PropertyProducer_OnPropertyListRequestReceived, this, std::placeholders::_1);
    callBacks.onPropertyGetValueRequestRecieved = std::bind(&PropertyProducerApp_C::PropertyProducer_OnPropertyGetValueRequestRecieved, this, std::placeholders::_1);
    callBacks.onPropertySetValueRequestRecieved = std::bind(&PropertyProducerApp_C::PropertyProducer_OnPropertySetValueRequestRecieved, this, std::placeholders::_1);

    PropertyGatherer::PropertyProducerInitParams_T propertyProducerInitParams{propertyDescList, callBacks};
    if (PropertyGatherer::PropertyProducerStatus_E::Success != _propertyProducer->PropertyProducer_Initialize(propertyProducerInitParams))
    {
        std::cerr << "Error: Failed to initialize Property Producer\n";
        _gui.CloseGui();
        return;
    }

    /* Populate Property Values */
    _propertyValuesMap[0] = "Device Name";
    _propertyValuesMap[1] = "TestString";
    _propertyValuesMap[2] = static_cast<uint8_t>(255);

    _gui.SetWidgetValue(1, "Name: Device Name");
    std::shared_ptr<AddWidgetInfo_T> testStringAddWidgetInfo = std::make_shared<AddWidgetInfo_T>();
    testStringAddWidgetInfo->windowId = 0;
    testStringAddWidgetInfo->widgetName = "TestString";
    testStringAddWidgetInfo->flags = (Readable | Writeable);
    testStringAddWidgetInfo->widgetType = WidgetTypes_E::Text;
    testStringAddWidgetInfo->dataType = WidgetDataTypes_E::String;
    auto testStringDesc = _gui.AddWidgetToWindow(testStringAddWidgetInfo);
    _gui.SetWidgetValue(testStringDesc.widgetId, "TestString: 123");

    // AddWidgetInfo_T testIntAddWidgetInfo;
    // testIntAddWidgetInfo.windowId = 0;
    // testIntAddWidgetInfo.widgetName = "TestInt";
    // testIntAddWidgetInfo.isReadable = true;
    // testIntAddWidgetInfo.isWritable = true;
    // testIntAddWidgetInfo.isInteractable = false;
    // testIntAddWidgetInfo.isStaticField = false;
    // testIntAddWidgetInfo.type = WidgetTypes_E::TEXT;
    // testIntAddWidgetInfo.dataType = WidgetDataTypes_E::String;
    // auto testIntDesc = _gui.AddWidgetToWindow(testIntAddWidgetInfo);
    // _gui.SetWidgetValue(testIntDesc.widgetId, "TestInt: 255");

    while (false == _isQuit)
    {
        if (true == _transport->PollReceiveSocket())
        {
            HandleMessage();
        }
        _propertyProducer->PropertyProducer_ProcessTimedActivities();
        if (true == IsUserQuit())
        {
            _isQuit = true;
        }
    }
    _gui.CloseGui();
}

int32_t PropertyProducerApp_C::PropertyProducer_SendMessage(const std::vector<uint8_t> &message)
{
    std::cout << "Property Producer Send Message\n";

    if (_propertyConsumerDevKey.empty())
    {
        std::cout << "No Property Consumer connected\n";
        return -1;
    }
    return _transport->TransportSendMessage(_propertyConsumerInfo.destIp, _propertyConsumerInfo.destPort, message);
}

void PropertyProducerApp_C::PropertyProducer_OnPropertyListRequestReceived(const std::vector<PropertyGatherer::PropertyDescriptor_T> &descList)
{
    // Do nothing as the Property Producer handles this internally
    std::cout << "Property Producer Property List Request Received\n";
}

PropertyGatherer::PropertyGathererReplyStatus_E PropertyProducerApp_C::PropertyProducer_OnPropertyGetValueRequestRecieved(std::vector<PropertyGatherer::PropertyValueContainer_T>& values)
{
    std::cout << "Property Producer Get Value Request Received\n";
    auto retVal = PropertyGatherer::PropertyGathererReplyStatus_E::Success;
    for (auto& value : values)
    {
        auto it = _propertyValuesMap.find(value.propertyId);
        if (it != _propertyValuesMap.end())
        {
            value.value = it->second;
            std::cout << "Property ID: " << value.propertyId << ", Value: ";
            std::visit([](auto&& arg) { std::cout << arg; }, value.value);
            std::cout << "\n";
        }
        else
        {
            std::cout << "Property ID: " << value.propertyId << " not found\n";
            retVal = PropertyGatherer::PropertyGathererReplyStatus_E::InvalidPropertyId;
        }
    }
    return retVal;
}

PropertyGatherer::PropertyGathererReplyStatus_E PropertyProducerApp_C::PropertyProducer_OnPropertySetValueRequestRecieved(PropertyGatherer::PropertyValueContainer_T& value)
{
    std::cout << "Property Producer Set Value Request Received\n";
    auto retVal = PropertyGatherer::PropertyGathererReplyStatus_E::Success;

    auto it = _propertyValuesMap.find(value.propertyId);
    if (it != _propertyValuesMap.end())
    {
        it->second = value.value;
        std::cout << "Property ID: " << value.propertyId << ", Value: ";
        std::visit([](auto&& arg) { std::cout << arg; }, it->second);
        std::cout << "\n";
    }
    else
    {
        std::cout << "Property ID: " << value.propertyId << " not found\n";
        retVal = PropertyGatherer::PropertyGathererReplyStatus_E::InvalidPropertyId;
    }
    return retVal;
}

void PropertyProducerApp_C::HandleMessage()
{
    std::cout << "Property Producer Handle Message\n";
    std::string senderIp;
    uint16_t senderPort;
    auto msgBuf = std::make_unique<char []>(_rxBufferSize);
    auto msgSize = _transport->ReceiveMessage(msgBuf, _rxBufferSize, senderIp, senderPort);
    std::string devKey = senderIp + ":" + std::to_string(senderPort);

    std::cout << "Received " << msgSize << " bytes from " << devKey << "\n";
    if (_propertyConsumerDevKey == devKey)
    {
        std::cout << "Processing Gui Client msg\n";
        _propertyProducer->PropertyProducer_ProcessReceivedMessage(msgBuf, msgSize);
    }
    else if (_propertyConsumerDevKey.empty()) 
    {
        _propertyConsumerDevKey = devKey;
        std::cout << "Connected to Property Consumer. Device Key: " << _propertyConsumerDevKey << "\n";
        _propertyConsumerInfo.destIp = senderIp;
        _propertyConsumerInfo.destPort = senderPort;
        _propertyProducer->PropertyProducer_ProcessReceivedMessage(msgBuf, msgSize);
    }
    else
    {
        std::cout << "Unknown sender, known senders: " << _propertyConsumerDevKey << "\n";
    }
}