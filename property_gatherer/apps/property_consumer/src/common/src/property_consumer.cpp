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
    if (GuiProtocol::GuiClientReqStatus_E::SUCCESS != GuiClient_SendWidgetListRequest())
    {
        std::cout << "Failed to request widget list\n";
    }
    while (false == _isQuit)
    {
        if (true == _transport->PollReceiveSocket())
        {
            HandleMessage();
        }
        GuiClient_ProcessTimedActivities();
    }
}

void PropertyConsumer_C::HandleMessage()
{
    std::string senderIp;
    uint16_t senderPort;
    auto msgBuf = std::make_unique<char []>(_rxBufferSize);
    auto msgSize = _transport->ReceiveMessage(msgBuf, _rxBufferSize, senderIp, senderPort);
    std::string devKey = senderIp + ":" + std::to_string(senderPort);

    if (_guiAppDevKey == devKey)
    {
        GuiClient_ProcessReceivedMessage(msgBuf, msgSize);
    }
}

void PropertyConsumer_C::UpdateGuiWidget()
{
    // std::vector<uint8_t> msg;
    // _transport->TransportSendMessage(_guiAppInfo.destIp, _guiAppInfo.destPort, msg);
}

int32_t PropertyConsumer_C::GuiClient_SendMessage(const std::vector<uint8_t>& message)
{
    return _transport->TransportSendMessage(_guiAppInfo.destIp, _guiAppInfo.destPort, message);
}

void PropertyConsumer_C::GuiClient_OnWidgetListReplyReceived(GuiProtocol::WidgetReplyStatus_E status)
{
    if (GuiProtocol::WidgetReplyStatus_E::SUCCESS == status)
    {
        std::vector<std::pair<std::string, GuiProtocol::WidgetValueVariant_T>> setWidgetList;
        std::cout << "Widget List reply received with status success!\n";
        _widgetListReceived = true;
        for (const auto& [widgetName, widgetDesc] : GuiClient_WidgetList())
        {
            std::string newWidgetValue = "Consumer Set Widget " + std::to_string(widgetDesc.desc.widgetId) + widgetDesc.desc.widgetName;
            // setWidgetList.push_back({widgetName, newWidgetValue});
            std::cout << newWidgetValue << "\n";
        }
    }
    else
    {
        std::cout << "Widget List reply status was not success!\n";
    }
}