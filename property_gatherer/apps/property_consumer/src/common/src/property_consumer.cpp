#include "stdafx.h"
#include "property_consumer.h"

PropertyConsumer_C::PropertyConsumer_C(PropertyConsumerInitParams_C initParams) : _producerInfo(initParams.producerInfo), _guiAppInfo(initParams.guiAppInfo)
{
    _transport = UdpTransportFactory::CreateTransport();
    _transport->InitializeSocket(initParams.myIp, initParams.myPort);
    _rxBufferSize = 2048;
    _guiAppDevKey = _guiAppInfo.destIp + ":" + std::to_string(_guiAppInfo.destPort);
    _producerAppDevKey = _producerInfo.destIp + ":" + std::to_string(_producerInfo.destPort);
}

PropertyConsumer_C::~PropertyConsumer_C()
{

}

void PropertyConsumer_C::RunTest()
{
    auto guiClientStatus = GuiClient_SendWidgetListRequest();
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != guiClientStatus)
    {
        std::cout << "Failed to request widget list, error: " << static_cast<uint8_t>(guiClientStatus) << "\n";
    }
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
        GuiClient_ProcessTimedActivities();
        _isQuit = IsUserQuit();
    }
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
        GuiClient_ProcessReceivedMessage(msgBuf, msgSize);
    }
    else
    {
        std::cout << "Unknown sender, known senders: " << _guiAppDevKey << ", " << _producerAppDevKey << "\n";
    }
}

void PropertyConsumer_C::RunSetValueTest()
{
    std::cout << "Running Set Value test\n";
    std::vector<std::pair<std::string, GuiProtocol::WidgetValueVariant_T>> setWidgetList;
    for (const auto& [widgetName, widgetDesc] : GuiClient_WidgetList())
    {
        std::string newWidgetValue = "Consumer Set Widget " + widgetDesc.desc.widgetName;
        setWidgetList.push_back({widgetName, newWidgetValue});
        std::cout << newWidgetValue << "\n";
    }

    auto setValReturn = GuiClient_SendSetValueRequest(setWidgetList);
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != setValReturn)
    {
        std::cout << "Set Value Request failed with " << static_cast<uint8_t>(setValReturn) << "\n";
    }

    _runSetValTest = false;
}

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