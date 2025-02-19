#include "gui_client_api.h"

/* System includes */
#include <chrono>
namespace GuiProtocol
{
    GuiClient_C::GuiClient_C()
    {

    }

    GuiClient_C::~GuiClient_C()
    {

    }

    void GuiClient_C::GuiClient_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        Message_T rxMsg = {std::move(msg), size};
        _msgQueue.AddMessageToQueue(std::move(rxMsg));
    }

    void GuiClient_C::GuiClient_ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
            // Process msg based on msg id
        }
        else if (false == _updatedWidgets.empty())
        {

        }
        else
        {
            ProcessStateMachine();
        }
    }

    GuiClientReqStatus_E GuiClient_C::GuiClient_SendWidgetListRequest()
    {
        GuiClientReqStatus_E retVal = GuiClientReqStatus_E::ERROR;
        if (GuiClientState_E::INITIALIZED == _state)
        {
            std::cout << "Sending Widget List request\n";
            std::vector<uint8_t> buffer;
            _msgSerializer.Serialize(GetWidgetListRequest(), buffer);
            if (0 < GuiClient_SendMessage(buffer))
            {
                _widgetListRequested = true;
                retVal = GuiClientReqStatus_E::SUCCESS;
                std::cout << "Sent Widget List request\n";
            }
            else
            {
                retVal = GuiClientReqStatus_E::FAILED_TO_SEND_MSG;
            }
        }
        return retVal;
    }

    bool GuiClient_C::GuiClient_SetValue(std::vector<std::pair<std::string, WidgetValueVariant_T>>& widgetKeyValPairs)
    {
        bool retVal = true;

        for (auto& [key, value] : widgetKeyValPairs)
        {
            auto it = _widgetList.find(key);
            if (it != _widgetList.end())
            {
                it->second.val = value;

                /* Keep a list of widgets that were updated */
                _updatedWidgets.push_back(key);
            }
            else
            {
                std::cout << "Failed to find " << key << " in the widget list\n";
                retVal = false;
            }
        }
        return retVal;
    }

    uint64_t GuiClient_C::GetCurrentTimeMs()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        );
    }

    void GuiClient_C::ProcessStateMachine()
    {
        switch (_state)
        {
            case GuiClientState_E::INITIALIZED:
                if (true == _widgetListRequested)
                {
                    _state = GuiClientState_E::WIDGET_LIST_REQUESTED;
                }
                break;

            case GuiClientState_E::WIDGET_LIST_REQUESTED:
                // Check for a timeout
                if (true == _widgetListReceived)
                {
                    _state = GuiClientState_E::WIDGET_LIST_RECEIVED;
                }
                break;

            case GuiClientState_E::WIDGET_LIST_RECEIVED:
                break;

            default:
                break;
        }
    }

    void GuiClient_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.GetMessageFromQueue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[2]) << 8) | static_cast<uint8_t>(msg.data[3]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::WIDGET_LIST_REPLY:
                ProcessReceivedWidgetListReply(msg);
                break;

            default:
                break;
        }
    }

    void GuiClient_C::ProcessReceivedWidgetListReply(Message_T& msg)
    {
        WidgetListReply_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (WidgetReplyStatus_E::SUCCESS == static_cast<WidgetReplyStatus_E>(reply.status))
        {
            _widgetListReceived = true;

            /* Populate widget list from provided descriptors */
            for (auto& desc : reply.widgetDescriptorList)
            {
                Widget_T widget;
                widget.desc = desc;
                _widgetList[desc.widgetName] = widget;
            }
        }
        /* Call user callback */
        GuiClient_OnWidgetListReplyReceived(static_cast<WidgetReplyStatus_E>(reply.status));
    }

    void GuiClient_C::ProcessUpdatedWidgets()
    {
        for (auto widget : _updatedWidgets)
        {
            auto it = _widgetList.find(widget);
            if (it != _widgetList.end())
            {
                // Send a Widget Set Value
            }
        }
    }
}