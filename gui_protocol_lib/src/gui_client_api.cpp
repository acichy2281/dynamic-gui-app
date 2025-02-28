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
        auto queueSizeBeforeAdd = _msgQueue.Size();
        Message_T rxMsg = {std::move(msg), size};
        _msgQueue.AddMessageToQueue(std::move(rxMsg));

        if (queueSizeBeforeAdd == _msgQueue.Size())
        {
            std::cout << "Failed to add message to queue\n";
        }
    }

    void GuiClient_C::GuiClient_ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
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
            auto widgetListReq = GetWidgetListRequest();
            _msgSerializer.Serialize(widgetListReq, buffer);
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

    GuiClientReqStatus_E GuiClient_C::GuiClient_SendSetValueRequest(WidgetSetValueIdentifier_T& widgetKeyValPairs)
    {
        GuiClientReqStatus_E retVal = GuiClientReqStatus_E::ERROR;
        if (GuiClientState_E::WIDGET_LIST_RECEIVED == _state)
        {
            auto widgetValList = GenerateWidgetValueList(widgetKeyValPairs);

            std::cout << "Sending Widget Set Value request\n";
            std::vector<uint8_t> buffer;
            auto widgetSetValReq = GetWidgetSetValueRequest(widgetValList);
            _msgSerializer.Serialize(widgetSetValReq, buffer);
            if (0 < GuiClient_SendMessage(buffer))
            {
                retVal = GuiClientReqStatus_E::SUCCESS;
                std::cout << "Sent Widget Set Value request\n";
            }
            else
            {
                retVal = GuiClientReqStatus_E::FAILED_TO_SEND_MSG;
                std::cout << "Failed to send Widget Set Value request\n";
            }
        }
        else
        {
            std::cout << "Error! Widget list has not been received yet\n";
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
                if (true == _widgetListReceived)
                {
                    _state = GuiClientState_E::WIDGET_LIST_RECEIVED;
                }
                // Check for a timeout
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
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::WIDGET_LIST_REPLY:
                std::cout << "Received a Widget List Reply\n";
                ProcessReceivedWidgetListReply(msg);
                break;
            
            case MessageID_E::WIDGET_SET_VALUE_REPLY:
                std::cout << "Received a Widget Set Value Reply\n";
                ProcessReceivedWidgetSetValueReply(msg);
                break;

            default:
                std::cout << "Error! Unknown message received\n";
                break;
        }
    }

    void GuiClient_C::ProcessReceivedWidgetListReply(Message_T& msg)
    {
        WidgetListReply_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (WidgetReplyStatus_E::SET_VAL_SUCCESS == static_cast<WidgetReplyStatus_E>(reply.status))
        {
            _widgetListReceived = true;

            /* Populate widget list from provided descriptors */
            for (auto& desc : reply.widgetDescriptorList)
            {
                WidgetValueStorage_T widget;
                widget.desc = desc;
                _widgetList[desc.widgetName] = widget;
            }

            ProcessStateMachine();
        }
        /* Call user callback */
        GuiClient_OnWidgetListReplyReceived(static_cast<WidgetReplyStatus_E>(reply.status));
    }

    void GuiClient_C::ProcessReceivedWidgetSetValueReply(Message_T& msg)
    {
        WidgetSetValueReply_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (WidgetReplyStatus_E::SET_VAL_SUCCESS == static_cast<WidgetReplyStatus_E>(reply.status))
        {
            // Do something
        }
        /* Call user callback */
        GuiClient_OnWidgetSetValueReplyReceived(static_cast<WidgetReplyStatus_E>(reply.status), reply.setValuesList);
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

    std::vector<WidgetValueStorage_T> GuiClient_C::GenerateWidgetValueList(WidgetSetValueIdentifier_T& widgetKeyValPairs)
    {
        std::vector<WidgetValueStorage_T> retVal;
        for (auto& [key, value] : widgetKeyValPairs)
        {
            auto it = _widgetList.find(key);
            if (it != _widgetList.end())
            {
                it->second.val = value;
                retVal.push_back(it->second);
            }
            else
            {
                std::cout << "Failed to find " << key << " in the widget list\n";
            }
        }
        return retVal;
    }
}