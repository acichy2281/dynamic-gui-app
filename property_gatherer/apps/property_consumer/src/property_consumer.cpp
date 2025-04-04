#include "stdafx.h"
#include "property_consumer.h"

PropertyConsumerApp_C::PropertyConsumerApp_C(PropertyConsumerInitParams_C initParams) : 
    _producerInfo(initParams.producerInfo), _guiAppInfo(initParams.guiAppInfo),     
    _guiClient(std::make_shared<GuiProtocol::GuiClient_C>(
        std::bind(&PropertyConsumerApp_C::GuiClient_SendMessage, this, std::placeholders::_1),
        std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetListReplyReceived, this, std::placeholders::_1),
        std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetSetValueReplyReceived, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&PropertyConsumerApp_C::GuiClient_OnWidgetEventNotificationReceived, this, std::placeholders::_1, std::placeholders::_2)
    )),
    _propertyConsumer(std::make_shared<PropertyGatherer::PropertyConsumer_C>())
{
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myInfo.destIp, initParams.myInfo.destPort);
    _rxBufferSize = 2048;
    _guiAppDevKey = _guiAppInfo.destIp + ":" + std::to_string(_guiAppInfo.destPort);
    _producerAppDevKey = _producerInfo.destIp + ":" + std::to_string(_producerInfo.destPort);
    _propertyConsumer->SetCallbacks({ std::bind(&PropertyConsumerApp_C::PropertyConsumer_SendMessage, this, std::placeholders::_1),
                                      std::bind(&PropertyConsumerApp_C::PropertyConsumer_OnPropertyListReplyReceived, this, std::placeholders::_1, std::placeholders::_2),
                                      std::bind(&PropertyConsumerApp_C::PropertyConsumer_OnPropertyGetValueReplyRecieved, this, std::placeholders::_1, std::placeholders::_2) });
    if (false == initParams.configFile.empty()) _gui.SetConfigFile(initParams.configFile);
}

PropertyConsumerApp_C::~PropertyConsumerApp_C()
{

}

void PropertyConsumerApp_C::Gui_OnWidgetEvent(WidgetDescriptor_T& widgetDesc, WidgetValueVariant_T val)
{
    std::cout << "Widget event callback: Widget ID = " << widgetDesc.widgetId << ", " << widgetDesc.widgetName << ", Value = ";
    std::visit([](auto&& arg) { std::cout << arg; }, val);
    if (widgetDesc.widgetName == "Get Property List")
    {
        std::cout << "Requesting Property List\n";
        auto propListReqStatus = _propertyConsumer->SendPropertyListRequest();
        if (PropertyGatherer::PropertyConsumerReqStatus_E::PROPERTY_CONSUMER_STATUS_SUCCESS != propListReqStatus)
        {
            std::cout << "Failed to request property list, error: " << static_cast<uint8_t>(propListReqStatus) << "\n";
        }
    }
    else if (widgetDesc.widgetName == "Get Value")
    {
        std::cout << "Requesting Property Value\n";
        auto propValReqStatus = _propertyConsumer->SendGetValueRequest(255, {0});
        if (PropertyGatherer::PropertyConsumerReqStatus_E::PROPERTY_CONSUMER_STATUS_SUCCESS != propValReqStatus)
        {
            std::cout << "Failed to request property value, error: " << static_cast<uint8_t>(propValReqStatus) << "\n";
        }
    }
    else if (widgetDesc.widgetName == "Set Value")
    {
        RunSetValueTest();
    }
    else if (widgetDesc.widgetName == "Start Gui Client Thread")
    {
        std::cout << "Starting Gui Client Thread\n";
        std::thread guiClientThread(&PropertyConsumerApp_C::RunGuiClientTest, this);  
        guiClientThread.detach();
    }
    else if (widgetDesc.widgetName == "Start Property Consumer Thread")
    {
        std::cout << "Starting Property Consumer Thread\n";
        std::thread propertyConsumerThread(&PropertyConsumerApp_C::RunPropertyConsumerTest, this);  
        propertyConsumerThread.detach();
    }
    std::cout << "\n";
}

void PropertyConsumerApp_C::Gui_OnGuiWindowClosed()
{
    std::cout << "GUI window closed\n";
    _isQuit = true;
}

void PropertyConsumerApp_C::RunTest()
{
    if (false == _gui.InitializeGui())
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
    }
    _gui.SetCallbacks({ std::bind(&PropertyConsumerApp_C::Gui_OnWidgetEvent, this, std::placeholders::_1, std::placeholders::_2), 
                        std::bind(&PropertyConsumerApp_C::Gui_OnGuiWindowClosed, this) });
    // std::thread guiClientThread(&PropertyConsumerApp_C::RunGuiClientTest, this);  
    // std::thread propertyConsumerThread(&PropertyConsumerApp_C::RunPropertyConsumerTest, this);
    std::thread messageHandlerThread(&PropertyConsumerApp_C::MessageHandlerThread, this);  
    _gui.RunGui();
    messageHandlerThread.join();
    // guiClientThread.join();
    // propertyConsumerThread.join();
}

void PropertyConsumerApp_C::RunGuiClientTest()
{
    auto guiClientStatus = _guiClient->SendWidgetListRequest();
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != guiClientStatus)
    {
        std::cout << "Failed to request widget list, error: " << static_cast<uint8_t>(guiClientStatus) << "\n";
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

void PropertyConsumerApp_C::RunSetValueTest()
{
    std::cout << "Running Set Value test\n";
    std::vector<std::pair<uint32_t, WidgetValueVariant_T>> setWidgetList;
    for (const auto& [widgetId, widgetStorage] : _guiClient->WidgetList())
    {
        if (0 == (widgetStorage.desc.flags & Writeable))
        {
            std::cout << "Widget " << widgetId << " is not writable, skipping...\n";
        }
        else if (widgetStorage.desc.widgetType == static_cast<uint8_t>(WidgetTypes_E::Text))
        {
            std::string newWidgetValue = "Consumer Set Widget " + widgetStorage.desc.widgetName;
            setWidgetList.push_back({widgetId, newWidgetValue});
            std::cout << newWidgetValue << "\n";
        }
    }

    auto setValReturn = _guiClient->SendSetValueRequest(setWidgetList);
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != setValReturn)
    {
        std::cout << "Set Value Request failed with " << static_cast<uint8_t>(setValReturn) << "\n";
    }

    _runSetValTest = false;
}

/* GUI Client callback implementations */
int32_t PropertyConsumerApp_C::GuiClient_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_guiAppInfo.destIp, _guiAppInfo.destPort, message);
}

void PropertyConsumerApp_C::GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status)
{
    if (GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS == status)
    {
        std::cout << "Widget List reply received with status success!\n";
        _widgetListReceived = true;
        _runSetValTest = true;
    }
    else
    {
        std::cout << "Widget List reply status was not success!\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList)
{
    if (GuiProtocol::WidgetReplyStatus_E::SET_VAL_SUCCESS == status)
    {
        std::cout << "Widget Set Value reply received with status success!\n";
    }
    else
    {
        std::cout << "Widget Set Value reply status was not success!\n";
    }
}

void PropertyConsumerApp_C::GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue)
{
    std::cout << "Widget Event Notification received for widgetId: " << widgetId << "\n";
    if (std::holds_alternative<std::string>(updatedValue))
    {
        std::cout << "Updated value: " << std::get<std::string>(updatedValue) << "\n";
    }
    else if (std::holds_alternative<int>(updatedValue))
    {
        std::cout << "Updated value: " << std::get<int>(updatedValue) << "\n";
    }
    else if (std::holds_alternative<float>(updatedValue))
    {
        std::cout << "Updated value: " << std::get<float>(updatedValue) << "\n";
    }
    else if (std::holds_alternative<bool>(updatedValue))
    {
        std::cout << "Updated value: " << (std::get<bool>(updatedValue) ? "true" : "false") << "\n";
    }
    else
    {
        std::cout << "Unknown updated value type!\n";
    }
}

int32_t PropertyConsumerApp_C::PropertyConsumer_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_producerInfo.destIp, _producerInfo.destPort, message);
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertyListReplyReceived(PropertyGatherer::PropertyReplyStatus_E status, std::vector<PropertyGatherer::PropertyDescriptor_T>& descList)
{
    std::cout << "Property List Reply received\n";

    for (const auto& desc : descList)
    {
        std::cout << "Property ID: " << desc.propertyId << ", Name: " << desc.propertyName << "\n";
    }
}

void PropertyConsumerApp_C::PropertyConsumer_OnPropertyGetValueReplyRecieved(PropertyGatherer::PropertyReplyStatus_E status, std::vector<PropertyGatherer::PropertyStorageVariant>& values)
{
    std::cout << "Property Get Value Reply received\n";

    for (const auto& value : values)
    {
        std::cout << "Property Value: ";
        std::visit([](auto&& arg) { std::cout << arg; }, value);
        std::cout << "\n";
    }
    // Handle the reply here
}