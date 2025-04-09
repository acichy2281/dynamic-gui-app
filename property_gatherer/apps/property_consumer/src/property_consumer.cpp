#include "stdafx.h"
#include "property_consumer.h"

PropertyConsumerApp_C::PropertyConsumerApp_C(PropertyConsumerInitParams_C initParams) : 
    _producerInfo(initParams.producerInfo), _guiAppInfo(initParams.guiAppInfo),     
    _guiClient(std::make_shared<GuiProtocol::GuiClient_C>()),
    _propertyConsumer(std::make_shared<PropertyGatherer::PropertyConsumer_C>())
{
    /* Initialize Transport */
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myInfo.destIp, initParams.myInfo.destPort);
    _rxBufferSize = 2048;
    _guiAppDevKey = _guiAppInfo.destIp + ":" + std::to_string(_guiAppInfo.destPort);
    _producerAppDevKey = _producerInfo.destIp + ":" + std::to_string(_producerInfo.destPort);

    /* Set Gui Client Callbacks */
    auto guiClientCallBacks = GuiProtocol::GuiClientCallbacks_T();
    guiClientCallBacks.sendMessage = std::bind(&PropertyConsumerApp_C::GuiClient_SendMessage, this, std::placeholders::_1);
    guiClientCallBacks.onWidgetListReplyReceived = std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetListReplyReceived, this, std::placeholders::_1);
    guiClientCallBacks.onWidgetSetValueReplyReceived = std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetSetValueReplyReceived, this, std::placeholders::_1, std::placeholders::_2);
    guiClientCallBacks.onWidgetGetValueReplyReceived = std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetGetValueReplyReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    guiClientCallBacks.onWidgetEventNotificationReceived = std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetEventNotificationReceived, this, std::placeholders::_1, std::placeholders::_2);

    GuiProtocol::GuiClientInitParams_T guiClientInitParams{guiClientCallBacks};
    _guiClient->Initialize(guiClientInitParams);

    /* Set Property Consumer Callbacks */
    auto propertyConsumerCallBacks = PropertyGatherer::PropertyConsumerCallbacks_T();
    propertyConsumerCallBacks.sendMessage = std::bind(&PropertyConsumerApp_C::PropertyConsumer_SendMessage, this, std::placeholders::_1);
    propertyConsumerCallBacks.onPropertyListReplyReceived = std::bind(&PropertyConsumerApp_C::PropertyConsumer_OnPropertyListReplyReceived, this, std::placeholders::_1, std::placeholders::_2);
    propertyConsumerCallBacks.onPropertyGetValueReplyRecieved = std::bind(&PropertyConsumerApp_C::PropertyConsumer_OnPropertyGetValueReplyRecieved, this, std::placeholders::_1, std::placeholders::_2);
    propertyConsumerCallBacks.onPropertySetValueReplyRecieved = std::bind(&PropertyConsumerApp_C::PropertyConsumer_OnPropertySetValueReplyRecieved, this, std::placeholders::_1, std::placeholders::_2);
    
    PropertyGatherer::PropertyConsumerInitParams_T propertyConsumerInitParams{propertyConsumerCallBacks};
    _propertyConsumer->PropertyConsumer_Initialize(propertyConsumerInitParams);

    if (false == initParams.configFile.empty()) _configFile = initParams.configFile;;
}

PropertyConsumerApp_C::~PropertyConsumerApp_C()
{

}

void PropertyConsumerApp_C::Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val)
{
    std::cout << "Widget event callback: Widget ID = " << widgetDesc.widgetId << ", " << widgetDesc.widgetName << ", Value = ";
    PrintVariant(val);
    std::cout << "\n";

    auto it = _widgetCallbacks.find(widgetDesc.widgetId);
    if (it != _widgetCallbacks.end()) 
    {
        it->second(val);
    }
    else if (widgetDesc.widgetName == "Get Property Value")
    {
        std::cout << "Requesting Property Get Value\n";
        
        auto widgetIt = _widgetNameToIdMap.find(PROPERTY_VALUE_OPTIONS_WIDGET_NAME);
        if (widgetIt == _widgetNameToIdMap.end())
        {
            std::cout << "Widget ID not found for Get Property Value Options\n";
            return;
        }
        auto optionWidgetId = widgetIt->second;
        auto widgetValue = static_cast<uint16_t>(std::get<int>(_gui.GetWidgetValue(optionWidgetId)));

        auto propValReqStatus = _propertyConsumer->SendGetValueRequest(255, {widgetValue});
        if (PropertyGatherer::PropertyConsumerStatus_E::Success != propValReqStatus)
        {
            std::cout << "Failed to request property value, error: " << static_cast<uint8_t>(propValReqStatus) << "\n";
        }
    }
    else if (widgetDesc.widgetName == "Set Property Value")
    {
        std::cout << "Requesting Property Set Value\n";
        
        auto widgetIt = _widgetNameToIdMap.find(PROPERTY_VALUE_OPTIONS_WIDGET_NAME);
        if (widgetIt == _widgetNameToIdMap.end())
        {
            std::cout << "Widget ID not found for Set Property Value Options\n";
            return;
        }
        auto optionWidgetId = widgetIt->second;
        auto propId = static_cast<uint16_t>(std::get<int>(_gui.GetWidgetValue(optionWidgetId)));

        auto propIt = _propertyList.find(propId);
        if (propIt == _propertyList.end())
        {
            std::cout << "Property ID not found for Set Property Value Options\n";
            return;
        }
        auto propertyDesc = propIt->second;
        PropertyGatherer::PropertyStorageVariant val;
        switch (propertyDesc.propertyType)
        {
            case PropertyGatherer::PropertyStorageVariantType_E::String:
            {
                val = "UpdatedString";
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned8BitInt:
            {
                val = static_cast<uint8_t>(UINT8_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned16BitInt:
            {
                val = static_cast<uint16_t>(UINT16_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned32BitInt:
            {
                val = static_cast<uint32_t>(UINT32_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Unsigned64BitInt:
            {
                val = static_cast<uint64_t>(UINT64_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed8BitInt:
            {
                val = static_cast<int8_t>(INT8_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed16BitInt:
            {
                val = static_cast<int16_t>(INT16_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed32BitInt:
            {
                val = static_cast<int32_t>(INT32_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Signed64BitInt:
            {
                val = static_cast<int64_t>(INT64_MAX / 2);
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Float:
            {
                val = 3.3f;
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Double:
            {
                val = 4.4;
                break;
            }
            case PropertyGatherer::PropertyStorageVariantType_E::Boolean:
            {
                val = true;
                break;
            }
            default:
                std::cout << "Unknown property type\n";
                break;
        }

        std::cout << "Requesting Set Value for Property " << propertyDesc.propertyId << ":" << propertyDesc.propertyName << " with value: ";
        PrintVariant(val);
        std::cout << "\n";
        auto propValReqStatus = _propertyConsumer->SendSetValueRequest((propId), val);
        if (PropertyGatherer::PropertyConsumerStatus_E::Success != propValReqStatus)
        {
            std::cout << "Failed to request property set value, error: " << static_cast<uint8_t>(propValReqStatus) << "\n";
        }
    }
    else if (widgetDesc.widgetName == "Get Widget Value")
    {
        std::cout << "Requesting Widget Value\n";
        
        auto widgetIt = _widgetNameToIdMap.find(WIDGET_VALUE_OPTIONS_WIDGET_NAME);
        if (widgetIt == _widgetNameToIdMap.end())
        {
            std::cout << "Widget ID not found for Get Widget Value Options\n";
            return;
        }
        auto optionWidgetId = widgetIt->second;
        auto widgetId = static_cast<uint16_t>(std::get<int>(_gui.GetWidgetValue(optionWidgetId)));

        auto guiClientStatus = _guiClient->SendGetValueRequest(widgetId);
        if (GuiProtocol::GuiClientStatus_E::Success != guiClientStatus)
        {
            std::cout << "Failed to request widget value, error: " << static_cast<uint8_t>(guiClientStatus) << "\n";
        }
    }
    else if (widgetDesc.widgetName == "Set Widget Value")
    {
        RunSetWidgetValueTest();
    }
}

void PropertyConsumerApp_C::Gui_OnGuiWindowClosed()
{
    std::cout << "GUI window closed\n";
    _isQuit = true;
}

void PropertyConsumerApp_C::Gui_OnConfigFileSet(bool status)
{
    if (true == status)
    {
        std::cout << "Config file set successfully\n";
        // Create an unordered_map to store widget callback functions
        
        for (const auto& [widgetId, widgetDesc] : _gui.GetWidgetList())
        {
            std::cout << "Widget ID: " << widgetId << ", Widget Name: " << widgetDesc.widgetName << "\n";
            
            _widgetNameToIdMap[widgetDesc.widgetName] = widgetId;
            if (widgetDesc.widgetName == "Get Property List") 
            {
                _widgetCallbacks[widgetId] = [this](WidgetValueVariant_T) {
                    auto propListReqStatus = _propertyConsumer->SendPropertyListRequest();
                    if (PropertyGatherer::PropertyConsumerStatus_E::Success != propListReqStatus) {
                    std::cout << "Failed to request property list\n";
                    }
                };
            }
            else if (widgetDesc.widgetName == "Get Widget List")
            {
                _widgetCallbacks[widgetId] = [this](WidgetValueVariant_T) {
                    std::cout << "Requesting Widget List\n";
                    auto guiClientStatus = _guiClient->SendWidgetListRequest();
                    if (GuiProtocol::GuiClientStatus_E::Success != guiClientStatus)
                    {
                        std::cout << "Failed to request widget list, error: " << static_cast<uint8_t>(guiClientStatus) << "\n";
                    }
                };
            }
            else if (widgetDesc.widgetName == "Start Gui Client Thread")
            {
                _widgetCallbacks[widgetId] = [this](WidgetValueVariant_T) {
                    std::cout << "Starting Gui Client Thread\n";
                    std::thread guiClientThread(&PropertyConsumerApp_C::RunGuiClientTest, this);  
                    guiClientThread.detach();
                };
            }
            else if (widgetDesc.widgetName == "Start Property Consumer Thread")
            {
                _widgetCallbacks[widgetId] = [this](WidgetValueVariant_T) {
                    std::cout << "Starting Property Consumer Thread\n";
                    std::thread propertyConsumerThread(&PropertyConsumerApp_C::RunPropertyConsumerTest, this);  
                    propertyConsumerThread.detach();
                };
            }
        }
    }
    else
    {
        std::cout << "Failed to set config file\n";
    }
}

void PropertyConsumerApp_C::RunTest()
{
    DynamicGuiInitParams_T initParams;
    initParams.callbacks.onWidgetEventOccured = std::bind(&PropertyConsumerApp_C::Gui_OnWidgetEvent, this, std::placeholders::_1, std::placeholders::_2);
    initParams.callbacks.onWindowClose = std::bind(&PropertyConsumerApp_C::Gui_OnGuiWindowClosed, this);
    initParams.callbacks.onConfigFileSet = std::bind(&PropertyConsumerApp_C::Gui_OnConfigFileSet, this, std::placeholders::_1);
    if (false == _gui.InitializeGui(initParams))
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
    }
    else 
    {
        std::thread messageHandlerThread(&PropertyConsumerApp_C::MessageHandlerThread, this);  
        if (false == _configFile.empty()) _gui.SetConfigFile(_configFile);
        _gui.RunGui();
        messageHandlerThread.join();
    }
}

void PropertyConsumerApp_C::RunGuiClientTest()
{
    while (false == _isQuit)
    {
        _guiClient->ProcessTimedActivities();
        if (true == IsUserQuit())
        {
            _isQuit = true;
        }
    }
}

void PropertyConsumerApp_C::RunPropertyConsumerTest()
{
    while (false == _isQuit)
    {
        _propertyConsumer->ProcessTimedActivities();
        if (true == IsUserQuit())
        {
            _isQuit = true;
        }
    }
}

void PropertyConsumerApp_C::MessageHandlerThread()
{
    while (false == _isQuit)
    {
        if (true == _transport->PollReceiveSocket())
        {
            HandleMessage();
        }
        if (true == IsUserQuit())
        {
            _isQuit = true;
        }
    }
    _gui.CloseGui();
}

void PropertyConsumerApp_C::HandleMessage()
{
    std::string senderIp;
    uint16_t senderPort;
    auto msgBuf = std::make_unique<char []>(_rxBufferSize);
    auto msgSize = _transport->ReceiveMessage(msgBuf, _rxBufferSize, senderIp, senderPort);
    std::string devKey = senderIp + ":" + std::to_string(senderPort);

    std::cout << "Received " << msgSize << " bytes from " << devKey << "\n";
    if (_guiAppDevKey == devKey)
    {
        std::cout << "Processing Gui Client msg\n";
        _guiClient->ProcessReceivedMessage(msgBuf, msgSize);
    }
    else if (_producerAppDevKey == devKey)
    {
        std::cout << "Processing Property Producer msg\n";
        _propertyConsumer->ProcessReceivedMessage(msgBuf, msgSize);
    }
    else
    {
        std::cout << "Unknown sender, known senders: " << _guiAppDevKey << ", " << _producerAppDevKey << "\n";
    }
}

void PropertyConsumerApp_C::RunSetWidgetValueTest()
{
    std::cout << "Running Set Value test\n";
    std::vector<std::pair<uint32_t, WidgetValueVariant_T>> setWidgetList;
    for (const auto& [widgetId, widgetDesc] : _guiClient->WidgetList())
    {
        if (0 == (widgetDesc.flags & Writeable))
        {
            std::cout << "Widget " << widgetId << " is not writable, skipping...\n";
        }
        else if (widgetDesc.widgetType == static_cast<uint8_t>(WidgetTypes_E::Text))
        {
            std::string newWidgetValue = "Consumer Set Widget " + widgetDesc.widgetName;
            setWidgetList.push_back({widgetId, newWidgetValue});
            std::cout << newWidgetValue << "\n";
        }
    }

    auto setValReturn = _guiClient->SendSetValueRequest(setWidgetList);
    if (GuiProtocol::GuiClientStatus_E::Success != setValReturn)
    {
        std::cout << "Set Value Request failed with " << static_cast<uint8_t>(setValReturn) << "\n";
    }
}

/* GUI Client callback implementations */
int32_t PropertyConsumerApp_C::GuiClient_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_guiAppInfo.destIp, _guiAppInfo.destPort, message);
}

void PropertyConsumerApp_C::GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status)
{
    if (GuiProtocol::WidgetReplyStatus_E::Success == status)
    {
        std::cout << "Widget List reply received with status success!\n";

        auto widget = _gui.GetWidget(WIDGET_VALUE_OPTIONS_WIDGET_NAME);
        if (widget && std::dynamic_pointer_cast<WidgetRadio_C>(widget))
        {
            auto radioWidget = std::dynamic_pointer_cast<WidgetRadio_C>(widget);
            for (const auto& [widgetId, widgetDesc] : _guiClient->WidgetList())
            {
                radioWidget->AddOption(widgetDesc.widgetName);
                std::cout << "Added Option for widget ID: " << widgetDesc.widgetId << ", Name: " << widgetDesc.widgetName << "\n";
            }
        }
        else
        {
            std::cout << "No Radio widget found for Widget Value Options\n";
        }
        _widgetListReceived = true;
    }
    else
    {
        std::cout << "Widget List reply status was not success!\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList)
{
    if (GuiProtocol::WidgetReplyStatus_E::Success == status)
    {
        std::cout << "Widget Set Value reply received with status success!\n";
    }
    else
    {
        std::cout << "Widget Set Value reply status was not success!\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnWidgetGetValueReplyReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue, GuiProtocol::WidgetReplyStatus_E status)
{
    if (GuiProtocol::WidgetReplyStatus_E::Success == status)
    {
        std::cout << "Widget Get Value reply received with status success!\n";
        std::cout << "Updated value for widget ID: " << widgetId << ", Value: ";
        PrintVariant(updatedValue);
        std::cout << "\n";
    }
    else
    {
        std::cout << "Widget Get Value reply status was not success!\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue)
{
    std::cout << "Widget Event Notification received for widgetId: " << widgetId << "\n";
    std::cout << "Updated value: ";
    PrintVariant(updatedValue);
    std::cout << "\n";
}

int32_t PropertyConsumerApp_C::PropertyConsumer_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_producerInfo.destIp, _producerInfo.destPort, message);
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T>& descList)
{
    std::cout << "Property List Reply received\n";

    auto widget = _gui.GetWidget(PROPERTY_VALUE_OPTIONS_WIDGET_NAME);
    if (widget && std::dynamic_pointer_cast<WidgetRadio_C>(widget))
    {
        auto radioWidget = std::dynamic_pointer_cast<WidgetRadio_C>(widget);
        for (const auto& desc : descList)
        {
            radioWidget->AddOption(desc.propertyName);
            std::cout << "Added Option for property ID: " << desc.propertyId << ", Name: " << desc.propertyName << "\n";
            _propertyList[desc.propertyId] = desc;
        }
    }
    else
    {
        std::cout << "No Radio widget found for Property Value Options\n";
    }
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertyGetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyStorageVariant>& values)
{
    std::cout << "Property Get Value Reply received\n";

    for (const auto& value : values)
    {
        std::cout << "Property Value: ";
        PrintVariant(value);
        std::cout << "\n";
    }
    // Handle the reply here
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertySetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, PropertyGatherer::PropertyStorageVariant& value)
{
    std::cout << "Property Set Value Reply received\n";
    std::cout << "Property Value: ";
    PrintVariant(value);
    std::cout << "\n";
}