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
        _msgQueue.AddMessageToQueue(std::move(rxMsg));

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

    PropertyConsumerReqStatus_E PropertyConsumer_C::SendGetValueRequest()
    {

    }
}