#include "stdafx.h"
#include "property_producer.h"

PropertyProducerApp_C::PropertyProducerApp_C(PropertyProducerInitParams_C initParams) : 
    _propertyProducer(std::make_shared<PropertyGatherer::PropertyProducer_C>())
{
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myInfo.destIp, initParams.myInfo.destPort);
    _rxBufferSize = 2048;
    _propertyProducer->SetCallbacks({ std::bind(&PropertyProducerApp_C::PropertyProducer_SendMessage, this, std::placeholders::_1),
                                      std::bind(&PropertyProducerApp_C::PropertyProducer_OnPropertyListRequestReceived, this, std::placeholders::_1),
                                      std::bind(&PropertyProducerApp_C::PropertyProducer_OnPropertyGetValueRequestRecieved, this, std::placeholders::_1) });
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

void PropertyProducerApp_C::RunTest()
{
    std::cout << "Property Producer Test\n";
    if (false == _gui.InitializeGui())
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
    }
    _gui.SetCallbacks({ std::bind(&PropertyProducerApp_C::Gui_OnWidgetEvent, this, std::placeholders::_1, std::placeholders::_2), 
                        std::bind(&PropertyProducerApp_C::Gui_OnGuiWindowClosed, this) });
    std::thread producerThread(&PropertyProducerApp_C::RunProducerTest, this);  
    _gui.RunGui();
    producerThread.join();
}

void PropertyProducerApp_C::RunProducerTest()
{
    auto propertyDescList = std::vector<PropertyGatherer::PropertyDescriptor_T>();
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(0, false, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::STRING, 30, "Name"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(1, true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::STRING, 30, "TestString"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(2, true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::UNSIGNED_8_BIT_INT, 30, "TestInt"));
    _propertyProducer->SetPropertyList(propertyDescList);
    std::vector<std::pair<uint16_t, PropertyGatherer::PropertyStorageVariant>> propertyValues;
    propertyValues.push_back({0, "TestString"});
    propertyValues.push_back({1, "123"});
    propertyValues.push_back({2, static_cast<uint8_t>(255)});

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

    _propertyProducer->SetPropertyValue(propertyValues);
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

void PropertyProducerApp_C::PropertyProducer_OnPropertyListRequestReceived(std::vector<PropertyGatherer::PropertyDescriptor_T> &descList)
{

}

PropertyGatherer::PropertyReplyStatus_E PropertyProducerApp_C::PropertyProducer_OnPropertyGetValueRequestRecieved(std::vector<PropertyGatherer::PropertyStorageVariant> &values)
{
    return PropertyGatherer::PropertyReplyStatus_E::SET_VAL_SUCCESS;
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