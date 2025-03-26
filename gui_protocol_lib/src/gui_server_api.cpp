#include "gui_server_api.h"

/* System includes */
#include <chrono>
namespace GuiProtocol
{
    GuiServer_C::GuiServer_C(
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage,
        std::function<void()> onWidgetListRequestReceived,
        std::function<WidgetReplyStatus_E(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>&)> onWidgetSetValueRequestReceived,
        std::function<void(WidgetReplyStatus_E, uint16_t, uint16_t)> onWidgetEventNotificationAckReceived
    ) : SendMessage(sendMessage),
        OnWidgetListRequestReceived(onWidgetListRequestReceived),
        OnWidgetSetValueRequestReceived(onWidgetSetValueRequestReceived),
        OnWidgetEventNotificationAckReceived(onWidgetEventNotificationAckReceived)
    {

    }

    GuiServer_C::~GuiServer_C()
    {

    }

    void GuiServer_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto queueSizeBeforeAdd = _msgQueue.Size();
        Message_T rxMsg = {std::move(msg), size};
        _msgQueue.Enqueue(std::move(rxMsg));

        if (queueSizeBeforeAdd == _msgQueue.Size())
        {
            std::cout << "Failed to add message to received msg queue\n";
        }
        else
        {
            std::cout << "Added message to received msg queue\n";
        }
    }

    void GuiServer_C::ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
        }
        ProcessStateMachine();
    }

    bool GuiServer_C::SetWidgetList(std::vector<WidgetDescriptor_T>& descList)
    {
        bool retVal = false;

        if (false == _widgetListPopulated)
        {
            for (auto& desc : descList)
            {
                auto it = _widgetMap.find(desc.widgetId);
                if (it == _widgetMap.end())
                {
                    _widgetMap[desc.widgetId] = desc;
                }
                else
                {
                    std::cout << "Cannot add duplicate widget " << desc.widgetId << ":" << desc.widgetName << "\n";
                }
            }
            _widgetListPopulated = true;
        }
        return true;
    }
    
    GuiServerReqStatus_E GuiServer_C::SendWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T val)
    {
        GuiServerReqStatus_E retVal = GuiServerReqStatus_E::REQ_STATUS_ERROR;
        if (GuiServerState_E::READY == _state)
        {
            std::vector<uint8_t> buffer;
            WidgetEventNotification_T widgetEventNotification = GetWidgetEventNotification(windowId, widgetId, val);
            _msgSerializer.Serialize(widgetEventNotification, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetEventNotificationSent = true;
                retVal = GuiServerReqStatus_E::SUCCESS;
            }
            else
            {
                retVal = GuiServerReqStatus_E::FAILED_TO_SEND_MSG;
            }
        }

        return retVal;
    }

    uint64_t GuiServer_C::GetCurrentTimeMs()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        );
    }

    void GuiServer_C::ProcessStateMachine()
    {
        switch (_state)
        {
            case GuiServerState_E::INITIALIZED:
                if (true == _widgetListPopulated)
                {
                    _state = GuiServerState_E::WIDGET_LIST_POPULATED;
                    std::cout << "Setting State to Widget List Populated\n";
                }
                break;

            case GuiServerState_E::WIDGET_LIST_POPULATED:
                if (true == _widgetListReplySent)
                {
                    _state = GuiServerState_E::READY;
                    std::cout << "Setting State to Ready\n";
                }
                break;

            case GuiServerState_E::READY:
                if (true == _widgetEventNotificationSent)
                {
                    _state = GuiServerState_E::WIDGET_EVENT_NOTIFICATION_SENT;
                    std::cout << "Setting State to Event Notification Sent\n";
                    _widgetEventNotificationSent = false; // Reset for next event notification
                }
                break;
            
            case GuiServerState_E::WIDGET_EVENT_NOTIFICATION_SENT:
                if (true == _widgetEventNotificationAckReceived)
                {
                    _state = GuiServerState_E::READY;
                    std::cout << "Setting State back to Ready\n";
                    _widgetEventNotificationAckReceived = false; // Reset for next event notification
                }
                break;

            default:
                break;
        }
    }

    void GuiServer_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.Dequeue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::WIDGET_LIST_REQ:
                std::cout << "Received Widget List Request\n";
                ProcessReceivedWidgetListRequest();
                break;

            case MessageID_E::WIDGET_SET_VALUE_REQ:
                std::cout << "Received Set Value Request\n";
                ProcessReceivedWidgetSetValueRequest(msg);
                break;

            case MessageID_E::WIDGET_EVENT_NOTIFICATION_ACK:
                std::cout << "Received Widget Event Notification Ack\n";
                ProcessReceivedWidgetEventNotificationAck(msg);
                break;

            default:
                std::cout << "Unknown message received with Message ID " << msgId << "\n";
                break;
        }
    }

    void GuiServer_C::ProcessReceivedWidgetListRequest()
    {
        if (GuiServerState_E::WIDGET_LIST_POPULATED == _state)
        {
            std::vector<uint8_t> buffer;
            
            /* Construct a list of widget descriptors */
            std::vector<WidgetDescriptor_T> descList;
            for (auto& [widgetId, widgetDesc] : _widgetMap)
            {
                descList.push_back(widgetDesc);
            }

            /* Generate a WidgetListReply serialized message */
            auto status = WidgetReplyStatus_E::SET_VAL_SUCCESS;
            auto widgetListReply = GetWidgetListReply(descList, status);
            _msgSerializer.Serialize(widgetListReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetListReplySent = true;
            }
        }
        else if (GuiServerState_E::INITIALIZED == _state)
        {
            std::cout << "Error! Widget List not populated yet!\n";
        }
        else
        {
            std::cout << "Error! Widget List already received!\n";
        }
        /* Call user callback */
        OnWidgetListRequestReceived();
    }

    void GuiServer_C::ProcessReceivedWidgetSetValueRequest(Message_T& msg)
    {
        /* Only process the widget Set Value Request if the widget list is populated and has been sent to the Client */
        if (true == _widgetListPopulated && true == _widgetListReplySent)
        {
            WidgetSetValueRequest_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Generate a list of Widget Set Value requests for widgets */
            std::vector<GuiProtocol::WidgetSetValueResponseReturn_T> widgetSetValueResponseList;
            for (auto& widgetToSet : reqMsg.setValuesList)
            {
                auto it = _widgetMap.find(widgetToSet.widgetId);
                if (it != _widgetMap.end())
                {
                    /* Populate a struct to pass too the GUI app for it to use to Set value */
                    WidgetSetValueResponseReturn_T widgetSetValueResponse;
                    widgetSetValueResponse.windowId = static_cast<uint16_t>(widgetToSet.widgetId >> 16);
                    widgetSetValueResponse.widgetId = static_cast<uint16_t>(widgetToSet.widgetId & 0xFFFF);
                    widgetSetValueResponse.widgetType = it->second.widgetType;
                    widgetSetValueResponse.dataType = it->second.dataType;
                    widgetSetValueResponse.val = widgetToSet.value;
                    widgetSetValueResponse.status = static_cast<uint16_t>(WidgetReplyStatus_E::SET_VAL_ERROR);
                    widgetSetValueResponseList.push_back(widgetSetValueResponse);
                }
            }
            auto status = OnWidgetSetValueRequestReceived(widgetSetValueResponseList);

            std::vector<uint8_t> buffer;
            auto widgetSetValReply = GetWidgetSetValueReply(widgetSetValueResponseList, status);
            _msgSerializer.Serialize(widgetSetValReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetSetValueReplySent = true;
            }
        }
        else 
        {
            std::cout << "Error! Widget List not received\n";
        }
    }

    void GuiServer_C::ProcessReceivedWidgetEventNotificationAck(Message_T& msg)
    {
        WidgetEventNotificationAck_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (WidgetReplyStatus_E::SET_VAL_SUCCESS == static_cast<WidgetReplyStatus_E>(reply.status))
        {
            _widgetEventNotificationAckReceived = true;
        }

        /* Call user callback */
       auto windowId = static_cast<uint16_t>(reply.widgetId >> 16);
       auto widgetId = static_cast<uint16_t>(reply.widgetId & 0xFFFF);
       OnWidgetEventNotificationAckReceived(static_cast<WidgetReplyStatus_E>(reply.status), windowId, widgetId);
    }
}