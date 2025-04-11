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
    PrintVariant(val);
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
    uint16_t numProps = 0;
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor(numProps, false, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "Device Name"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), false, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "Device Manufacturer"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), false, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "Device Group"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::String, 30, "Test_String"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Unsigned8BitInt, 30, "Test_Unsigned8BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Unsigned16BitInt, 30, "Test_Unsigned16BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Unsigned32BitInt, 30, "Test_Unsigned32BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Unsigned64BitInt, 30, "Test_Unsigned64BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Signed8BitInt, 30, "Test_Signed8BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Signed16BitInt, 30, "Test_Signed16BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Signed32BitInt, 30, "Test_Signed32BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Signed64BitInt, 30, "Test_Signed64BitInt"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Float, 30, "Test_Float"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Double, 30, "Test_Double"));
    propertyDescList.push_back(PropertyGatherer::CreatePropertyDescriptor((++numProps), true, true, true, true, PropertyGatherer::PropertyStorageVariantType_E::Boolean, 30, "Test_Boolean"));
    std::cout << "Property List Size: " << numProps << "\n";

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

    /* Populate Property Values & Create Widget */
    for (auto& desc : propertyDescList)
    {
        std::cout << "Property ID: " << desc.propertyId << ", Name: " << desc.propertyName << "\n";
        
        std::shared_ptr<AddWidgetInfo_T> testStringAddWidgetInfo = std::make_shared<AddWidgetInfo_T>();
        testStringAddWidgetInfo->windowId = 0;
        testStringAddWidgetInfo->widgetType = WidgetTypes_E::Text;
        testStringAddWidgetInfo->widgetName = desc.propertyName;
        auto widgetDesc = _gui.AddWidgetToWindow(testStringAddWidgetInfo);
        std::string propertyDisplayInfoStr = "ID: " + std::to_string(desc.propertyId) + ", Name: " + desc.propertyName + ", Value: ";
        switch (desc.propertyType)
        {
            case PropertyGatherer::PropertyStorageVariantType_E::String:
            {
                _propertyValuesMap[desc.propertyId] = desc.propertyName;
                propertyDisplayInfoStr += desc.propertyName;
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned8BitInt:
            {
                auto val = static_cast<uint8_t>(UINT8_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned16BitInt:
            {
                auto val = static_cast<uint16_t>(UINT16_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned32BitInt:
            {
                auto val = static_cast<uint32_t>(UINT32_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned64BitInt:
            {
                auto val = static_cast<uint64_t>(UINT64_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed8BitInt:
            {
                auto val = static_cast<int8_t>(INT8_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed16BitInt:
            {
                auto val = static_cast<int16_t>(INT16_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed32BitInt:
            {
                auto val = static_cast<int32_t>(INT32_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed64BitInt:
            {
                auto val = static_cast<int64_t>(INT64_MAX);
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Float:
            {
                auto val = 1.1f;
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Double:
            {
                auto val = 2.2;
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += std::to_string(val);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Boolean:
            {
                auto val = true;
                _propertyValuesMap[desc.propertyId] = val;
                propertyDisplayInfoStr += (val ? "true" : "false");
                break;
            }
            default:
                std::cout << "Unknown property type\n";
                break;
        }
        _gui.SetWidgetValue(widgetDesc.widgetId, propertyDisplayInfoStr);
        std::cout << propertyDisplayInfoStr << "\n";
    }

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
            PrintVariant(value.value);
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
        PrintVariant(value.value);
        std::cout << "\n";

        // Update the GUI with the new value
        auto widgetIt = _gui.GetWidgetList().find((value.propertyId+1));
        if (widgetIt != _gui.GetWidgetList().end())
        {
            std::string propertyDisplayInfoStr = "ID: " + std::to_string(value.propertyId) + ", Name: " + widgetIt->second.widgetName + ", Value: ";
            std::visit([&propertyDisplayInfoStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    propertyDisplayInfoStr += arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    propertyDisplayInfoStr += arg ? "true" : "false";
                } else {
                    propertyDisplayInfoStr += std::to_string(arg);
                }
            }, value.value);
            _gui.SetWidgetValue(widgetIt->second.widgetId, propertyDisplayInfoStr);
        }
        else
        {
            std::cout << "No matching widget found for Property ID: " << value.propertyId << "\n";
        }
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
