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
    guiClientCallBacks.onAddWidgetReplyReceived = std::bind(&PropertyConsumerApp_C::GuiClient_OnAddWidgetReplyReceived, this, std::placeholders::_1);

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

    if (widgetDesc.widgetName == PROPERTY_CONSUMER_THREAD_BUTTON && false == _propertyConsumerStarted)
    {
        std::cout << "Starting Property Consumer Thread\n";
        std::thread propertyConsumerThread(&PropertyConsumerApp_C::RunPropertyConsumerTest, this);  
        propertyConsumerThread.detach();
        _propertyConsumerStarted = true;
    }
    if (widgetDesc.widgetName == GET_PROPERTY_LIST_BUTTON)
    {
        RunPropertyListRequestTest();
    }
    else if (widgetDesc.widgetName == GET_PROPERTY_VALUE_BUTTON)
    {
        RunPropertyGetValueRequestTest();
    }
    else if (widgetDesc.widgetName == SET_PROPERTY_VALUE_BUTTON)
    {
        RunPropertySetValueRequestTest();
    }
    else if (widgetDesc.widgetName == GUI_CLIENT_THREAD_BUTTON && false == _guiClientStarted)
    {
        std::cout << "Starting Gui Client Thread\n";
        std::thread guiClientThread(&PropertyConsumerApp_C::RunGuiClientTest, this);  
        guiClientThread.detach();
        _guiClientStarted = true;
    }
    else if (widgetDesc.widgetName == GET_WIDGET_LIST_BUTTON)
    {
        RunWidgetListRequestTest();
    }
    else if (widgetDesc.widgetName == GET_WIDGET_VALUE_BUTTON)
    {
        RunWidgetGetValueRequestTest();
    }
    else if (widgetDesc.widgetName == SET_WIDGET_VALUE_BUTTON)
    {
        RunWidgetSetValueRequestTest();
    }
    else if (widgetDesc.widgetName == ADD_WIDGET_WIDGET_NAME)
    {
        RunAddWidgetRequestTest();
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
            if (widgetDesc.widgetName == GUI_CLIENT_THREAD_BUTTON)
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
    auto guiClientTestWindow = _gui.CreateGuiWindow("Gui Client Tester");

    std::vector<nlohmann::json> widgetDataList;
    nlohmann::json widgetListButton = {
        {"Name", GET_WIDGET_LIST_BUTTON},
        {"Type", "button"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(widgetListButton);
    nlohmann::json addWidgetButton = {
        {"Name", ADD_WIDGET_WIDGET_NAME},
        {"Type", "button"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(addWidgetButton);
    nlohmann::json widgetValRadio = {
        {"Name", WIDGET_VALUE_OPTIONS_WIDGET_NAME},
        {"Type", "radio"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(widgetValRadio);
    nlohmann::json getValText = {
        {"Name", WIDGET_VALUE_OUTPUT_WIDGET_NAME},
        {"Type", "text"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(getValText);
    nlohmann::json getValButton = {
        {"Name", GET_WIDGET_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(getValButton);
    nlohmann::json setValInputText = {
        {"Name", WIDGET_SET_VALUE_INPUT_WIDGET_NAME},
        {"Type", "input_text"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(setValInputText);
    nlohmann::json setValButton = {
        {"Name", SET_WIDGET_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", guiClientTestWindow}
    };
    widgetDataList.push_back(setValButton);

    for (auto& widgetData : widgetDataList)
    {
        auto widgetDesc = _gui.AddWidgetToWindow(widgetData);
        _widgetNameToIdMap[widgetDesc.widgetName] = widgetDesc.widgetId;
    }
    
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
    auto propertyConsumerTestWindow = _gui.CreateGuiWindow("Property Consumer Tester");

    std::vector<nlohmann::json> widgetDataList;
    nlohmann::json propertyListButton = {
        {"Name", GET_PROPERTY_LIST_BUTTON},
        {"Type", "button"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(propertyListButton);
    nlohmann::json widgetValRadio = {
        {"Name", PROPERTY_VALUE_OPTIONS_WIDGET_NAME},
        {"Type", "radio"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(widgetValRadio);
    nlohmann::json getValText = {
        {"Name", PROPERTY_VALUE_OUTPUT_WIDGET_NAME},
        {"Type", "text"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(getValText);
    nlohmann::json getValButton = {
        {"Name", GET_PROPERTY_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(getValButton);
    nlohmann::json setValInputText = {
        {"Name", PROPERTY_SET_VALUE_INPUT_WIDGET_NAME},
        {"Type", "input_text"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(setValInputText);
    nlohmann::json setValButton = {
        {"Name", SET_PROPERTY_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", propertyConsumerTestWindow}
    };
    widgetDataList.push_back(setValButton);

    for (auto& widgetData : widgetDataList)
    {
        auto widgetDesc = _gui.AddWidgetToWindow(widgetData);
        _widgetNameToIdMap[widgetDesc.widgetName] = widgetDesc.widgetId;
    }

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
    auto desc = _guiClient->WidgetList().find(widgetId);
    if (desc != _guiClient->WidgetList().end())
    {
        std::cout << "Widget Event Notification received for widgetId: " << desc->second.widgetName << "\n";
        std::cout << "Updated value: ";
        PrintVariant(updatedValue);
        std::cout << "\n";

        /* Update Local Radio Widget */
        if (PROPERTY_VALUE_OPTIONS_WIDGET_NAME == desc->second.widgetName)
        {
            auto localRadioWidget = std::dynamic_pointer_cast<WidgetRadio_C>(_gui.GetWidget(PROPERTY_VALUE_OPTIONS_WIDGET_NAME));
            if (localRadioWidget)
            {
                localRadioWidget->SetWidgetValue(updatedValue);
            }
            else
            {
                std::cout << "No Radio widget found for Widget ID: " << widgetId << "\n";
            }
        }
        /* Update Local Input Text Box */
        else if (PROPERTY_SET_VALUE_INPUT_WIDGET_NAME == desc->second.widgetName)
        {
            auto localInputTextWidget = std::dynamic_pointer_cast<WidgetInputText_C>(_gui.GetWidget(PROPERTY_SET_VALUE_INPUT_WIDGET_NAME));
            if (localInputTextWidget)
            {
                localInputTextWidget->SetWidgetValue(updatedValue);
            }
            else
            {
                std::cout << "No Input Text widget found for Widget ID: " << widgetId << "\n";
            }
        }
        else if (GET_PROPERTY_VALUE_BUTTON == desc->second.widgetName)
        {
            RunPropertyGetValueRequestTest();
        }
        else if (SET_PROPERTY_VALUE_BUTTON == desc->second.widgetName)
        {
            RunPropertySetValueRequestTest();
        }
    }
    else
    {
        std::cout << "No widget found for Widget ID: " << widgetId << "\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnAddWidgetReplyReceived(GuiProtocol::WidgetReplyStatus_E status)
{
    if (GuiProtocol::WidgetReplyStatus_E::Success == status)
    {
        std::cout << "Add Widget reply received with status success!\n";
    }
    else
    {
        std::cout << "Add Widget reply status was not success!\n";
    }
}

int32_t PropertyConsumerApp_C::PropertyConsumer_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_producerInfo.destIp, _producerInfo.destPort, message);
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyGathererReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T>& descList)
{
    std::cout << "Property List Reply received\n";

    /* Create Remote GUI json objects */
    uint16_t guiServerTestWindowId = 0;
    nlohmann::json widgetValRadio = {
        {"Name", PROPERTY_VALUE_OPTIONS_WIDGET_NAME},
        {"Type", "radio"},
        {"Options", nlohmann::json::array()},
        {"window_id", guiServerTestWindowId}
    };
    nlohmann::json getValText = {
        {"Name", PROPERTY_VALUE_OUTPUT_WIDGET_NAME},
        {"Type", "text"},
        {"window_id", guiServerTestWindowId}
    };
    nlohmann::json getValButton = {
        {"Name", GET_PROPERTY_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", guiServerTestWindowId}
    };
    nlohmann::json setValInputText = {
        {"Name", PROPERTY_SET_VALUE_INPUT_WIDGET_NAME},
        {"Type", "input_text"},
        {"window_id", guiServerTestWindowId}
    };
    nlohmann::json setValButton = {
        {"Name", SET_PROPERTY_VALUE_BUTTON},
        {"Type", "button"},
        {"window_id", guiServerTestWindowId}
    };

    /* Set Local GUI Menu */
    auto widget = _gui.GetWidget(PROPERTY_VALUE_OPTIONS_WIDGET_NAME);
    if (widget && std::dynamic_pointer_cast<WidgetRadio_C>(widget))
    {
        auto radioWidget = std::dynamic_pointer_cast<WidgetRadio_C>(widget);
        for (const auto& desc : descList)
        {
            /* Add option for Local GUI */
            radioWidget->AddOption(desc.propertyName);
            std::cout << "Added Option for property ID: " << desc.propertyId << ", Name: " << desc.propertyName << "\n";
            _propertyList[desc.propertyId] = desc;

            /* Add option for Remote GUI */
            widgetValRadio["Options"].push_back(desc.propertyName);
        }
    }
    else
    {
        std::cout << "No Radio widget found for Property Value Options\n";
    }

    /* Set Remote GUI Menu */
    _propertyWidgetDataList.push_back(widgetValRadio);
    _propertyWidgetDataList.push_back(getValText);
    _propertyWidgetDataList.push_back(getValButton);
    _propertyWidgetDataList.push_back(setValInputText);
    _propertyWidgetDataList.push_back(setValButton);
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

    auto widget = _gui.GetWidget(PROPERTY_VALUE_OUTPUT_WIDGET_NAME);
    if (widget && std::dynamic_pointer_cast<WidgetText_C>(widget))
    {
        auto textWidget = std::dynamic_pointer_cast<WidgetText_C>(widget);
        std::string propertyDisplayInfoStr = "Property Value: [";
        std::visit([&propertyDisplayInfoStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                propertyDisplayInfoStr += arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                propertyDisplayInfoStr += arg ? "true" : "false";
            } else {
                propertyDisplayInfoStr += std::to_string(arg);
            }
        }, values[0]);
        propertyDisplayInfoStr += "]";
        textWidget->SetWidgetValue(propertyDisplayInfoStr);

        /* Set Value on GUI Server */
        for (const auto& [widgetId, widgetDesc] : _guiClient->WidgetList())
        {
            if (PROPERTY_VALUE_OUTPUT_WIDGET_NAME == widgetDesc.widgetName)
            {
                GuiProtocol::WidgetSetValueIdentifier_T setValueList = {{widgetId, propertyDisplayInfoStr}};
                auto guiClientReqStatus = _guiClient->SendSetValueRequest(setValueList);
                if (GuiProtocol::GuiClientStatus_E::Success != guiClientReqStatus)
                {
                    std::cout << "Failed to request widget value, error: " << static_cast<uint8_t>(guiClientReqStatus) << "\n";
                }
            }
        }
    }
    else
    {
        std::cout << "No Text widget found for Get Value Output\n";
    }
    

}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertySetValueReplyRecieved(PropertyGatherer::PropertyGathererReplyStatus_E status, PropertyGatherer::PropertyStorageVariant& value)
{
    std::cout << "Property Set Value Reply received\n";
    std::cout << "Property Value: ";
    PrintVariant(value);
    std::cout << "\n";
}

/* GUI Client Test Functions */
void PropertyConsumerApp_C::RunWidgetListRequestTest()
{
    std::cout << "Requesting Widget List\n";
    auto widgetListReqStatus = _guiClient->SendWidgetListRequest();
    if (GuiProtocol::GuiClientStatus_E::Success != widgetListReqStatus)
    {
        std::cout << "Failed to request widget list, error: " << static_cast<uint8_t>(widgetListReqStatus) << "\n";
    }
}

void PropertyConsumerApp_C::RunWidgetGetValueRequestTest()
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

void PropertyConsumerApp_C::RunWidgetSetValueRequestTest()
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

void PropertyConsumerApp_C::RunAddWidgetRequestTest()
{
    std::cout << "Requesting Add Widget\n";
    auto addWidgetReqStatus = _guiClient->SendAddWidgetRequest(_propertyWidgetDataList);
    if (GuiProtocol::GuiClientStatus_E::Success != addWidgetReqStatus)
    {
        std::cout << "Failed to send add widget request, error: " << static_cast<uint8_t>(addWidgetReqStatus) << "\n";
    }
    else
    {
        std::cout << "Add Widget request sent successfully\n";
    }
}



/* Property Consumer Test Functions */
void PropertyConsumerApp_C::RunPropertyListRequestTest()
{
    std::cout << "Requesting Property List\n";
    auto propListReqStatus = _propertyConsumer->SendPropertyListRequest();
    if (PropertyGatherer::PropertyConsumerStatus_E::Success != propListReqStatus)
    {
        std::cout << "Failed to request property list, error: " << static_cast<uint8_t>(propListReqStatus) << "\n";
    }
}

void PropertyConsumerApp_C::RunPropertyGetValueRequestTest()
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

void PropertyConsumerApp_C::RunPropertySetValueRequestTest()
{
    std::cout << "Requesting Property Set Value\n";
        
    // Get the property ID from the widget value
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

    // Get the value from the Input Text Widget
    auto inputIt = _widgetNameToIdMap.find(PROPERTY_SET_VALUE_INPUT_WIDGET_NAME);
    if (inputIt == _widgetNameToIdMap.end())
    {
        std::cout << "Widget ID not found for Set Property Input\n";
        return;
    }
    std::string widgetValString = std::get<std::string>(_gui.GetWidgetValue(inputIt->second));
    PropertyGatherer::PropertyStorageVariant val;
    switch (propertyDesc.propertyType)
    {
        case PropertyGatherer::PropertyStorageVariantType_E::String:
        {
            val = widgetValString;
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Unsigned8BitInt:
        {
            unsigned long parsed = std::stoul(widgetValString);
            if (parsed > UINT8_MAX) throw std::out_of_range("Value exceeds uint8_t range");
            val = static_cast<uint8_t>(parsed);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Unsigned16BitInt:
        {
            unsigned long parsed = std::stoul(widgetValString);
            if (parsed > UINT16_MAX) throw std::out_of_range("Value exceeds uint16_t range");
            val = static_cast<uint16_t>(parsed);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Unsigned32BitInt:
        {
            val = static_cast<uint32_t>(std::stoul(widgetValString));
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Unsigned64BitInt:
        {
            val = std::stoull(widgetValString);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Signed8BitInt:
        {
            long parsed = std::stol(widgetValString);
            if (parsed < INT8_MIN || parsed > INT8_MAX) throw std::out_of_range("Value exceeds int8_t range");
            val = static_cast<int8_t>(parsed);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Signed16BitInt:
        {
            long parsed = std::stol(widgetValString);
            if (parsed < INT16_MIN || parsed > INT16_MAX) throw std::out_of_range("Value exceeds int16_t range");
            val = static_cast<int16_t>(parsed);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Signed32BitInt:
        {
            val = static_cast<int32_t>(std::stol(widgetValString));
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Signed64BitInt:
        {
            val = std::stoll(widgetValString);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Float:
        {
            val = std::stof(widgetValString);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Double:
        {
            val = std::stod(widgetValString);
            break;
        }
        case PropertyGatherer::PropertyStorageVariantType_E::Boolean:
        {
            auto lowerStr = widgetValString;
            std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
            val = (lowerStr == "true" || lowerStr == "1");
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
