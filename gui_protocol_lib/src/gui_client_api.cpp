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

    GuiClientStatus_E GuiClient_C::Initialize(GuiClientInitParams_T& initParams)
    {
        GuiClientStatus_E retVal = GuiClientStatus_E::Error;
        if (false == SetCallbacks(initParams.callbacks))
        {
            std::cout << "Error! Failed to set callbacks\n";
            _errorOccured = true;
        }
        else
        {
            retVal = GuiClientStatus_E::Success;
            _initialized = true;
        }
        return retVal;
    }

    bool GuiClient_C::SetCallbacks(const GuiClientCallbacks_T& callbacks)
    {
        bool retVal = false;
        if (nullptr == callbacks.sendMessage)
        {
            std::cout << "Error! SendMessage callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetListReplyReceived)
        {
            std::cout << "Error! onWidgetListReplyReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetSetValueReplyReceived)
        {
            std::cout << "Error! onWidgetSetValueReplyReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetGetValueReplyReceived)
        {
            std::cout << "Error! onWidgetGetValueReplyReceived callback is null\n";
        }
        else if (nullptr == callbacks.onWidgetEventNotificationReceived)
        {
            std::cout << "Error! onWidgetEventNotificationReceived callback is null\n";
        }
        else
        {
            SendMessage = callbacks.sendMessage;
            OnWidgetListReplyReceived = callbacks.onWidgetListReplyReceived;
            OnWidgetSetValueReplyReceived = callbacks.onWidgetSetValueReplyReceived;
            OnWidgetGetValueReplyReceived = callbacks.onWidgetGetValueReplyReceived;
            OnWidgetEventNotificationReceived = callbacks.onWidgetEventNotificationReceived;
            retVal = true;
        }
        return retVal;
    }
    
    GuiClientStatus_E GuiClient_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {        
        auto retVal = GuiClientStatus_E::Error;
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
                retVal = GuiClientStatus_E::Success;
                std::cout << "Message added to queue\n";
            }
        }
        return retVal; 
    }

    void GuiClient_C::ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
        }
        else
        {
            ProcessStateMachine();
        }
    }

    GuiClientStatus_E GuiClient_C::SendWidgetListRequest()
    {
        GuiClientStatus_E retVal = GuiClientStatus_E::Error;
        if (GuiClientState_E::Initialized == _state)
        {
            std::cout << "Sending Widget List request\n";
            std::vector<uint8_t> buffer;
            auto widgetListReq = GetWidgetListRequest();
            _msgSerializer.Serialize(widgetListReq, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetListRequested = true;
                retVal = GuiClientStatus_E::Success;
                std::cout << "Sent Widget List request\n";
            }
            else
            {
                retVal = GuiClientStatus_E::FailedToSendMsg;
            }
        }
        else if (GuiClientState_E::WidgetListRequested == _state)
        {
            retVal = GuiClientStatus_E::RequestInProgress;
            std::cout << "Error! Widget list already requested!\n";
        }
        else if (GuiClientState_E::Error == _state)
        {
            retVal = GuiClientStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (GuiClientState_E::Uninitialized == _state)
        {
            retVal = GuiClientStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = GuiClientStatus_E::RequestInProgress;
            std::cout << "Error! Other Request in progress\n";
        }
        
        return retVal;
    }

    GuiClientStatus_E GuiClient_C::SendSetValueRequest(WidgetSetValueIdentifier_T& widgetKeyValPairs)
    {
        GuiClientStatus_E retVal = GuiClientStatus_E::Error;
        if (GuiClientState_E::Ready == _state)
        {
            std::cout << "Sending Widget Set Value request\n";
            std::vector<uint8_t> buffer;
            auto widgetSetValReq = GetWidgetSetValueRequest(widgetKeyValPairs);
            _msgSerializer.Serialize(widgetSetValReq, buffer);
            if (0 < SendMessage(buffer))
            {
                retVal = GuiClientStatus_E::Success;
                _widgetSetValueReqSent = true;
                _widgetSetValueReplyReceived = false;
                std::cout << "Sent Widget Set Value request\n";
            }
            else
            {
                retVal = GuiClientStatus_E::FailedToSendMsg;
                std::cout << "Failed to send Widget Set Value request\n";
            }
        }
        else if (GuiClientState_E::Initialized == _state)
        {
            retVal = GuiClientStatus_E::WidgetListNotReceived;
            std::cout << "Error! Widget list not received!\n";
        }
        else if (GuiClientState_E::Error == _state)
        {
            retVal = GuiClientStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (GuiClientState_E::Uninitialized == _state)
        {
            retVal = GuiClientStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = GuiClientStatus_E::RequestInProgress;
            std::cout << "Error! Request in progress\n";
        }
        return retVal;
    }

    GuiClientStatus_E GuiClient_C::SendGetValueRequest(uint32_t widgetId)
    {
        GuiClientStatus_E retVal = GuiClientStatus_E::Error;
        if (GuiClientState_E::Ready == _state)
        {
            std::cout << "Sending Widget Get Value request\n";
            std::vector<uint8_t> buffer;
            auto widgetGetValReq = GetWidgetGetValueRequest(widgetId);
            _msgSerializer.Serialize(widgetGetValReq, buffer);
            if (0 < SendMessage(buffer))
            {
                retVal = GuiClientStatus_E::Success;
                _widgetGetValueReqSent = true;
                _widgetGetValueReplyReceived = false;
                std::cout << "Sent Widget Get Value request\n";
            }
            else
            {
                retVal = GuiClientStatus_E::FailedToSendMsg;
                std::cout << "Failed to send Widget Get Value request\n";
            }
        }
        else if (GuiClientState_E::Initialized == _state)
        {
            retVal = GuiClientStatus_E::WidgetListNotReceived;
            std::cout << "Error! Widget list not received!\n";
        }
        else if (GuiClientState_E::Error == _state)
        {
            retVal = GuiClientStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (GuiClientState_E::Uninitialized == _state)
        {
            retVal = GuiClientStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = GuiClientStatus_E::RequestInProgress;
            std::cout << "Error! Request in progress\n";
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
        if (_errorOccured)
        {
            _state = GuiClientState_E::Error;
        }
        switch (_state)
        {
            case GuiClientState_E::Uninitialized:
                if (true == _initialized)
                {
                    _state = GuiClientState_E::Initialized;
                }
                break;

            case GuiClientState_E::Initialized:
                if (true == _widgetListRequested)
                {
                    _state = GuiClientState_E::WidgetListRequested;
                }
                break;

            case GuiClientState_E::WidgetListRequested:
                if (true == _widgetListReceived)
                {
                    _state = GuiClientState_E::Ready;
                }
                // Check for a timeout
                break;

            case GuiClientState_E::Ready:
                if (true == _widgetSetValueReqSent)
                {
                    _state = GuiClientState_E::WidgetSetValueRequestSent;
                }
                else if (true == _widgetGetValueReqSent)
                {
                    _state = GuiClientState_E::WidgetGetValueRequestSent;
                }
                break;

            case GuiClientState_E::WidgetSetValueRequestSent:
                if (true == _widgetSetValueReplyReceived)
                {
                    _widgetSetValueReqSent = false;
                    _widgetSetValueReplyReceived = false;
                    _state = GuiClientState_E::Ready;
                }
                break;

            case GuiClientState_E::WidgetGetValueRequestSent:
                if (true == _widgetGetValueReplyReceived)
                {
                    _widgetGetValueReqSent = false;
                    _widgetGetValueReplyReceived = false;
                    _state = GuiClientState_E::Ready;
                }
                break;

            case GuiClientState_E::Error:
                break;

            default:
                break;
        }
    }

    void GuiClient_C::ProcessReceivedMessageQueue()
    {
        if (GuiClientState_E::Uninitialized != _state && 
            GuiClientState_E::Error != _state)
        {
            auto msg = _msgQueue.Dequeue();
            uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
            switch (static_cast<MessageId_E>(msgId))
            {
                case MessageId_E::WidgetListReply:
                    std::cout << "Received a Widget List Reply\n";
                    ProcessReceivedWidgetListReply(msg);
                    break;
                
                case MessageId_E::WidgetSetValueReply:
                    std::cout << "Received a Widget Set Value Reply\n";
                    ProcessReceivedWidgetSetValueReply(msg);
                    break;

                case MessageId_E::WidgetGetValueReply:
                    std::cout << "Received a Widget Get Value Reply\n";
                    ProcessReceivedWidgetGetValueReply(msg);
                    break;

                case MessageId_E::WidgetEventNotification:
                    std::cout << "Received a Widget Event Notification\n";
                    ProcessReceivedWidgetEventNotification(msg);
                    break;

                default:
                    std::cout << "Error! Unknown message received\n";
                    break;
            }
        }
    }

    void GuiClient_C::ProcessReceivedWidgetListReply(Message_T& msg)
    {
        if (GuiClientState_E::WidgetListRequested != _state)
        {
            std::cout << "Error! Widget List Reply received, but no request was sent\n";
            return;
        }
        else
        {
            WidgetListReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (WidgetReplyStatus_E::Success == static_cast<WidgetReplyStatus_E>(reply.status))
            {
                _widgetListReceived = true;

                /* Populate widget list from provided descriptors */
                for (auto& desc : reply.widgetDescriptorList)
                {
                    _widgetList[desc.widgetId] = desc;

                    std::cout << "Added Widget ID: " << desc.widgetId << ", Name: " << desc.widgetName << " to the Widget List\n";
                }

                ProcessStateMachine();
            }
            /* Call user callback */
            OnWidgetListReplyReceived(static_cast<WidgetReplyStatus_E>(reply.status));
        }
    }

    void GuiClient_C::ProcessReceivedWidgetSetValueReply(Message_T& msg)
    {
        if (GuiClientState_E::WidgetSetValueRequestSent != _state)
        {
            std::cout << "Error! Widget Set Value Reply received, but no request was sent\n";
            return;
        }
        else
        {
            WidgetSetValueReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (WidgetReplyStatus_E::Success == static_cast<WidgetReplyStatus_E>(reply.status))
            {
            }
            /* Call user callback */
            OnWidgetSetValueReplyReceived(static_cast<WidgetReplyStatus_E>(reply.status), reply.setValuesList);
            _widgetSetValueReplyReceived = true;
        }
    }

    void GuiClient_C::ProcessReceivedWidgetGetValueReply(Message_T& msg)
    {
        if (GuiClientState_E::WidgetGetValueRequestSent != _state)
        {
            std::cout << "Error! Widget Get Value Reply received, but no request was sent\n";
            return;
        }
        else
        {
            WidgetGetValueReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (WidgetReplyStatus_E::Success == static_cast<WidgetReplyStatus_E>(reply.status))
            {
            }
            /* Call user callback */
            OnWidgetGetValueReplyReceived(reply.widgetId, reply.value, static_cast<WidgetReplyStatus_E>(reply.status));
            _widgetGetValueReplyReceived = true;
        }
    }

    void GuiClient_C::ProcessReceivedWidgetEventNotification(Message_T& msg)
    {
        if (GuiClientState_E::Error != _state)
        {
            std::cout << "Error! cannot process Widget Event notification\n";
            return;
        }
        else
        {
            WidgetEventNotification_T notification;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(notification, msgBuf);

            /* Call user callback */
            OnWidgetEventNotificationReceived(notification.widgetId, notification.updatedValue);

            /* Send the Ack */
            WidgetEventNotificationAck_T ack = GetWidgetEventNotificationAck(notification.widgetId, static_cast<uint16_t>(WidgetReplyStatus_E::Success));
            std::cout << "Sending Widget Event Notification Ack\n";
            std::vector<uint8_t> buffer;
            _msgSerializer.Serialize(ack, buffer);
            if (0 < SendMessage(buffer))
            {
                std::cout << "Sent Widget Event Notification Ack\n";
            }
            else
            {
                std::cout << "Failed to send Widget Event Notification Ack\n";
            }
        }
    }

    // std::vector<WidgetValueStorage_T> GuiClient_C::GenerateWidgetValueList(WidgetSetValueIdentifier_T& widgetKeyValPairs)
    // {
    //     std::vector<WidgetValueStorage_T> retVal;
    //     for (auto& [key, value] : widgetKeyValPairs)
    //     {
    //         auto it = _widgetList.find(key);
    //         if (it != _widgetList.end())
    //         {
    //             it->second.val = value;
    //             if (it->second.desc.flags & WidgetFlags_E::Writeable)
    //             {
    //                 retVal.push_back(it->second);
    //             }
    //             else
    //             {
    //                 std::cout << "Widget " << it->first << " is not writeable\n";
    //             }
    //         }
    //         else
    //         {
    //             std::cout << "Failed to find " << key << " in the widget list\n";
    //         }
    //     }
    //     return retVal;
    // }
}