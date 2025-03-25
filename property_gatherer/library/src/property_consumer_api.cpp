#include "property_consumer_api.h"

namespace PropertyGatherer
{
    PropertyConsumer_C::PropertyConsumer_C()
    {
        
    }
    
    PropertyConsumer_C::~PropertyConsumer_C()
    {
        
    }

    void PropertyConsumer_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto queueSizeBeforeAdd = _msgQueue.Size();
        Message_T rxMsg = { std::move(msg), size };
        _msgQueue.Enqueue(std::move(rxMsg));

        if (queueSizeBeforeAdd == _msgQueue.Size())
        {
            std::cout << "Failed to add message to queue\n";
        }
    }

    void PropertyConsumer_C::ProcessTimedActivities()
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

    PropertyConsumerReqStatus_E PropertyConsumer_C::SendPropertyListRequest()
    {
        PropertyConsumerReqStatus_E retVal = PropertyConsumerReqStatus_E::ERROR;
        if (PropertyConsumerState_E::INITIALIZED == _state)
        {
            std::cout << "Sending Property List request\n";
            std::vector<uint8_t> buffer;
            auto propertyListReq = GetPropertyListRequest();
            _msgSerializer.Serialize(propertyListReq, buffer);
            if (0 < SendMessage(buffer))
            {
                _propertyListRequested = true;
                retVal = PropertyConsumerReqStatus_E::SUCCESS;
                std::cout << "Sent widget list request\n";
            }
            else
            {
                retVal = PropertyConsumerReqStatus_E::FAILED_TO_SEND_MSG;
            }
        }
        return retVal;
    }

    PropertyConsumerReqStatus_E PropertyConsumer_C::SendGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds)
    {
        PropertyConsumerReqStatus_E retVal = PropertyConsumerReqStatus_E::ERROR;
        if (PropertyConsumerState_E::PROPERTY_LIST_RECEIVED == _state)
        {
            std::cout << "Sending Get Property request\n";
            std::vector<uint8_t> buffer;
            auto getPropertyRequest = GetPropertyGetValueRequest(maxResponseLength, propertyIds);
            _msgSerializer.Serialize(getPropertyRequest, buffer);
            if (0 < SendMessage(buffer))
            {
                retVal = PropertyConsumerReqStatus_E::SUCCESS;
                std::cout << "Sent Get Property Value request\n";
            }
            else
            {
                retVal = PropertyConsumerReqStatus_E::FAILED_TO_SEND_MSG;
                std::cout << "Failed to send Get Property Value request\n";
            }
        }
        else
        {
            std::cout << "Error! Property list has not been received yet, but trying to get value\n";
        }
        return retVal;
    }

    uint64_t PropertyConsumer_C::GetCurrentTimeMs()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
                ).count()
            );
    }

    void PropertyConsumer_C::ProcessStateMachine()
    {
        switch (_state)
        {
            case PropertyConsumerState_E::INITIALIZED:
                if (true == _propertyListRequested)
                {
                    _state = PropertyConsumerState_E::PROPERTY_LIST_REQUESTED;
                }
                break;

            case PropertyConsumerState_E::PROPERTY_LIST_REQUESTED:
                if (true == _propertyListReceived)
                {
                    _state = PropertyConsumerState_E::PROPERTY_VALUE_RECEIVED;
                }
                // Check for a timeout
                break;

            case PropertyConsumerState_E::PROPERTY_VALUE_RECEIVED:
                break;

            default:
                break;
        }
    }

    void PropertyConsumer_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.Dequeue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::PROPERTY_LIST_REPLY:
                std::cout << "Received a Property List Reply\n";
                ProcessReceivedPropertyListReply(msg);
                break;

            case MessageID_E::GET_VALUE_REPLY:
                std::cout << "Received a Property Get Value Reply\n";
                ProcessReceivedPropertyGetValueReply(msg);
                break;

            default:
                std::cout << "Error! Unknown message received\n";
                break;
        }
    }

    void PropertyConsumer_C::ProcessReceivedPropertyListReply(Message_T& msg)
    {
        PropertyListReply_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (PropertyReplyStatus_E::SET_VAL_SUCCESS == static_cast<PropertyReplyStatus_E>(reply.status))
        {
            _propertyListReceived = true;

            /* Probably maintain some internal storage of available properties */

            ProcessStateMachine();
        }

        OnPropertyListReplyReceived(static_cast<PropertyReplyStatus_E>(reply.status), reply.propertyDescriptorList);
    }

    void PropertyConsumer_C::ProcessReceivedPropertyGetValueReply(Message_T& msg)
    {
        GetValueReply_T reply;
        std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
        _msgSerializer.Deserialize(reply, msgBuf);

        if (PropertyReplyStatus_E::SET_VAL_SUCCESS == static_cast<PropertyReplyStatus_E>(reply.status))
        {
            // Do something
        }
        OnPropertyGetValueReplyRecieved(static_cast<PropertyReplyStatus_E>(reply.status), reply.propValues);
    }
}