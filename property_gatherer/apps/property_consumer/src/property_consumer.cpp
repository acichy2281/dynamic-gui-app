#include "stdafx.h"
#include "property_consumer.h"

PropertyConsumer_C::PropertyConsumer_C(PropertyConsumerInitParams_C initParams) : 
    _producerInfo(initParams.producerInfo), _guiAppInfo(initParams.guiAppInfo),     
    _guiClient(std::make_shared<GuiProtocol::GuiClient_C>(
        std::bind(&PropertyConsumer_C::GuiClient_SendMessage, this, std::placeholders::_1),
        std::bind(&PropertyConsumer_C::GuiClient_OnWidgetListReplyReceived, this, std::placeholders::_1),
        std::bind(&PropertyConsumer_C::GuiClient_OnWidgetSetValueReplyReceived, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&PropertyConsumer_C::GuiClient_OnWidgetEventNotificationReceived, this, std::placeholders::_1, std::placeholders::_2)
    ))
{
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myIp, initParams.myPort);
    _rxBufferSize = 2048;
    _guiAppDevKey = _guiAppInfo.destIp + ":" + std::to_string(_guiAppInfo.destPort);
    _producerAppDevKey = _producerInfo.destIp + ":" + std::to_string(_producerInfo.destPort);    
    
    if (false == _gui.InitializeGui())
    {
        std::cerr << "Error: Failed to initialize GUI app\n";
    }
}

PropertyConsumer_C::~PropertyConsumer_C()
{

}

void PropertyConsumer_C::RunTest()
{
    auto guiClientStatus = _guiClient->SendWidgetListRequest();
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != guiClientStatus)
    {
        std::cout << "Failed to request widget list, error: " << static_cast<uint8_t>(guiClientStatus) << "\n";
    }
    
    std::thread guiServerThread(&DynamicGui_C::RunGui, std::ref(_gui));
    while (false == _isQuit)
    {
        if (true == _transport->PollReceiveSocket())
        {
            HandleMessage();
        }
        if (true == _runSetValTest)
        {
            RunSetValueTest();
        }
        _guiClient->ProcessTimedActivities();
        _isQuit = IsUserQuit();
    }
    guiServerThread.join();
}

void PropertyConsumer_C::HandleMessage()
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
    else
    {
        std::cout << "Unknown sender, known senders: " << _guiAppDevKey << ", " << _producerAppDevKey << "\n";
    }
}

void PropertyConsumer_C::RunSetValueTest()
{
    std::cout << "Running Set Value test\n";
    std::vector<std::pair<uint32_t, WidgetValueVariant_T>> setWidgetList;
    for (const auto& [widgetId, widgetStorage] : _guiClient->WidgetList())
    {
        if (false == widgetStorage.desc.isWritable)
        {
            std::cout << "Widget " << widgetId << " is not writable, skipping...\n";
        }
        else
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
int32_t PropertyConsumer_C::GuiClient_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_guiAppInfo.destIp, _guiAppInfo.destPort, message);
}

void PropertyConsumer_C::GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status)
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

void PropertyConsumer_C::GuiClient_OnWidgetSetValueReplyReceived(GuiProtocol::WidgetReplyStatus_E status, std::vector<GuiProtocol::WidgetSetValueReplyContainer_T>& setValuesList)
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

void PropertyConsumer_C::GuiClient_OnWidgetEventNotificationReceived(uint32_t widgetId, WidgetValueVariant_T updatedValue)
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