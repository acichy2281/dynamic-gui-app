#include "gui_server_api.h"

/* System includes */
#include <chrono>
namespace GuiProtocol
{
    GuiServer_C::GuiServer_C()
    {

    }

    GuiServer_C::~GuiServer_C()
    {

    }

    GuiServerStatus_E GuiServer_C::Initialize(GuiServerInitParams_T& initParams)
    {
        GuiServerStatus_E retVal = GuiServerStatus_E::Error;
        if (false == SetCallbacks(initParams.callbacks))
        {
            std::cout << "Error! Failed to set callbacks\n";
            _errorOccured = true;
        }
        else
        {
            retVal = GuiServerStatus_E::Success;
            _initialized = true;
        }
        return retVal;
    }

    bool GuiServer_C::SetCallbacks(const GuiServerCallbacks_T& callbacks)
    {
        bool retVal = false;
        if (nullptr == callbacks.sendMessage)
        {
            std::cout << "Error! SendMessage callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetListRequestReceived)
        {
            std::cout << "Error! onWidgetListRequestReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetSetValueRequestReceived)
        {
            std::cout << "Error! onWidgetSetValueRequestReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetGetValueRequestReceived)
        {
            std::cout << "Error! onWidgetGetValueRequestReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetEventNotificationAckReceived)
        {
            std::cout << "Error! onWidgetEventNotificationAckReceived callback is null\n";
        }
        else if (nullptr == callbacks.onAddWidgetRequestReceived)
        {
            std::cout << "Error! onAddWidgetRequestReceived callback is null\n";
        }
        else
        {
            SendMessage = callbacks.sendMessage;
            OnWidgetListRequestReceived = callbacks.onWidgetListRequestReceived;
            OnWidgetSetValueRequestReceived = callbacks.onWidgetSetValueRequestReceived;
            OnWidgetGetValueRequestReceived = callbacks.onWidgetGetValueRequestReceived;
            OnWidgetEventNotificationAckReceived = callbacks.onWidgetEventNotificationAckReceived;
            OnAddWidgetRequestReceived = callbacks.onAddWidgetRequestReceived;
            retVal = true;
        }
        return retVal;
    }

    GuiServerStatus_E GuiServer_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto retVal = GuiServerStatus_E::Error;
        if (nullptr == msg)
        {
            std::cout << "Error! Received null message\n";
        }
        else if (0 == size)
        {
            std::cout << "Error! Received empty message\n";
        }
        else
        {
            auto queueSizeBeforeAdd = _msgQueue.Size();
            Message_T rxMsg = { std::move(msg), size };
            _msgQueue.Enqueue(std::move(rxMsg));

            if (queueSizeBeforeAdd == _msgQueue.Size())
            {
                _errorOccured = true;
                std::cout << "Failed to add message to queue\n";
            }
            else
            {
                retVal = GuiServerStatus_E::Success;
                std::cout << "Message added to queue\n";
            }
        }
        return retVal; 
    }

    void GuiServer_C::ProcessTimedActivities()
    {
        ProcessStateMachine();
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
        }
    }
    
    GuiServerStatus_E GuiServer_C::SendWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T val)
    {
        GuiServerStatus_E retVal = GuiServerStatus_E::ReqStatusError;
        if (GuiServerState_E::Ready == _state)
        {
            std::vector<uint8_t> buffer;
            WidgetEventNotification_T widgetEventNotification = GetWidgetEventNotification(windowId, widgetId, val);
            _msgSerializer.Serialize(widgetEventNotification, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetEventNotificationSent = true;
                retVal = GuiServerStatus_E::Success;
            }
            else
            {
                retVal = GuiServerStatus_E::FailedToSendMsg;
            }
        }
        else if (GuiServerState_E::Initialized == _state)
        {
            retVal = GuiServerStatus_E::WidgetListNotReceived;
            std::cout << "Error! Widget List not received!\n";
        }
        else if (GuiServerState_E::Error == _state)
        {
            retVal = GuiServerStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (GuiServerState_E::Uninitialized == _state)
        {
            retVal = GuiServerStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = GuiServerStatus_E::RequestInProgress;
            std::cout << "Error! Request in progress.\n";
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
        if (_errorOccured)
        {
            _state = GuiServerState_E::Error;
        }
        switch (_state)
        {
            case GuiServerState_E::Uninitialized:
                if (true == _initialized)
                {
                    _state = GuiServerState_E::Initialized;
                }
                break;
            case GuiServerState_E::Initialized:
                if (true == _widgetListReplySent)
                {
                    _state = GuiServerState_E::Ready;
                    std::cout << "Setting State to Ready\n";
                }
                break;

            case GuiServerState_E::Ready:
                if (true == _widgetEventNotificationSent)
                {
                    _state = GuiServerState_E::WidgetEventNotificationSent;
                    std::cout << "Setting State to Event Notification Sent\n";
                    _widgetEventNotificationSent = false; // Reset for next event notification
                }
                break;
            
            case GuiServerState_E::WidgetEventNotificationSent:
                if (true == _widgetEventNotificationAckReceived)
                {
                    _state = GuiServerState_E::Ready;
                    std::cout << "Setting State back to Ready\n";
                    _widgetEventNotificationAckReceived = false; // Reset for next event notification
                }
                break;

            case GuiServerState_E::Error:
                break;

            default:
                break;
        }
    }

    void GuiServer_C::ProcessReceivedMessageQueue()
    {
        if (GuiServerState_E::Uninitialized != _state && 
            GuiServerState_E::Error != _state)
        {
            auto msg = _msgQueue.Dequeue();
            uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
            switch (static_cast<MessageId_E>(msgId))
            {
                case MessageId_E::WidgetListReq:
                    std::cout << "Received Widget List Request\n";
                    ProcessReceivedWidgetListRequest();
                    break;

                case MessageId_E::WidgetSetValueReq:
                    std::cout << "Received Set Value Request\n";
                    ProcessReceivedWidgetSetValueRequest(msg);
                    break;

                case MessageId_E::WidgetGetValueReq:
                    std::cout << "Received Get Value Request\n";
                    ProcessReceivedWidgetGetValueRequest(msg);
                    break;

                case MessageId_E::WidgetEventNotificationAck:
                    std::cout << "Received Widget Event Notification Ack\n";
                    ProcessReceivedWidgetEventNotificationAck(msg);
                    break;

                case MessageId_E::AddWidgetReq:
                    std::cout << "Received Add Widget Request\n";
                    ProcessReceivedAddWidgetRequest(msg);
                    break;

                default:
                    std::cout << "Unknown message received with Message ID " << msgId << "\n";
                    break;
            }
        }
    }

    void GuiServer_C::ProcessReceivedWidgetListRequest()
    {
        if (GuiServerState_E::Initialized == _state)
        {
            std::vector<uint8_t> buffer;
            
            /* Construct a list of widget descriptors */
            std::vector<WidgetDescriptor_T> descList;

            /* Call user callback for them to populate descList */
            OnWidgetListRequestReceived(descList);

            /* Generate a WidgetListReply serialized message */
            auto status = WidgetReplyStatus_E::Success;
            auto widgetListReply = GetWidgetListReply(descList, status);
            _msgSerializer.Serialize(widgetListReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetListReplySent = true;
            }
        }
        else
        {
            std::cout << "Error! Not able to process widget list request!\n";
        }
    }

    void GuiServer_C::ProcessReceivedWidgetSetValueRequest(Message_T& msg)
    {
        /* Only process the widget Set Value Request if the widget list is populated and has been sent to the Client */
        if (GuiServerState_E::Ready == _state)
        {
            WidgetSetValueRequest_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Generate a list of Widget Set Value requests for widgets */
            std::vector<WidgetSetValueReplyContainer_T> widgetSetValueReplyList;
            for (auto& widgetSetVal : reqMsg.setValuesList)
            {
                WidgetSetValueReplyContainer_T widgetSetValueReply;
                widgetSetValueReply.widgetId = static_cast<uint32_t>(widgetSetVal.widgetId);
                widgetSetValueReply.value = widgetSetVal.value;
                widgetSetValueReply.status = static_cast<uint16_t>(WidgetReplyStatus_E::Error);
                widgetSetValueReplyList.push_back(widgetSetValueReply);
            }

            /* Call user callback for them to Set Widget values */
            auto status = OnWidgetSetValueRequestReceived(widgetSetValueReplyList);

            /* Generate a WidgetSetValueReply serialized message */
            std::vector<uint8_t> buffer;
            auto widgetSetValReply = GetWidgetSetValueReply(widgetSetValueReplyList, status);
            _msgSerializer.Serialize(widgetSetValReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetSetValueReplySent = true;
            }
        }
        else 
        {
            std::cout << "Error! Not able to process request\n";
        }
    }

    void GuiServer_C::ProcessReceivedWidgetGetValueRequest(Message_T& msg)
    {
        if (GuiServerState_E::Ready == _state)
        {
            WidgetGetValueRequest_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Call user callback */
            WidgetValueVariant_T val;
            auto status = OnWidgetGetValueRequestReceived(reqMsg.widgetId, val);
            std::vector<uint8_t> buffer;
            auto widgetGetValReply = GetWidgetGetValueReply(reqMsg.widgetId, val, status);
            _msgSerializer.Serialize(widgetGetValReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetGetValueReplySent = true;
            }
        }
        else
        {
            std::cout << "Error! Not able to process request\n";
        }
    }

    void GuiServer_C::ProcessReceivedWidgetEventNotificationAck(Message_T& msg)
    {
        if (GuiServerState_E::WidgetEventNotificationSent != _state)
        {
            std::cout << "Error! Widget Event Notification Ack received, but no request was sent\n";
            return;
        }
        else
        {
            WidgetEventNotificationAck_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (WidgetReplyStatus_E::Success == static_cast<WidgetReplyStatus_E>(reply.status))
            {
                _widgetEventNotificationAckReceived = true;
            }

            /* Call user callback */
            auto windowId = static_cast<uint16_t>(reply.widgetId >> 16);
            auto widgetId = static_cast<uint16_t>(reply.widgetId & 0xFFFF);
            OnWidgetEventNotificationAckReceived(static_cast<WidgetReplyStatus_E>(reply.status), windowId, widgetId);
        }
    }

    void GuiServer_C::ProcessReceivedAddWidgetRequest(Message_T& msg)
    {
        if (GuiServerState_E::Ready == _state)
        {
            AddWidgetRequest_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Call user callback */
            std::vector<WidgetDescriptor_T> widgetDescList;
            widgetDescList.reserve(reqMsg.numWidgets);
            auto status = OnAddWidgetRequestReceived(reqMsg.widgetDataList, widgetDescList);

            std::vector<uint8_t> buffer;
            auto addWidgetReply = GetAddWidgetReply(widgetDescList, status);
            _msgSerializer.Serialize(addWidgetReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _addWidgetReplySent = true;
            }
        }
        else
        {
            std::cout << "Error! Not able to process request\n";
        }
    }
}