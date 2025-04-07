#include "property_consumer_api.h"

namespace PropertyGatherer
{
    PropertyConsumer_C::PropertyConsumer_C()
    {
        
    }
    
    PropertyConsumer_C::~PropertyConsumer_C()
    {
        
    }

    PropertyConsumerStatus_E PropertyConsumer_C::PropertyConsumer_Initialize(PropertyConsumerInitParams_T& initParams)
    {
        auto retVal = PropertyConsumerStatus_E::Error;
        if (false == SetCallbacks(initParams.callbacks))
        {
            std::cout << "Error! Failed to set callbacks\n";
            _errorOccured = true;
        }
        else
        {
            retVal = PropertyConsumerStatus_E::Success;
            _initialized = true;
        }
        return retVal;
    }

    bool PropertyConsumer_C::SetCallbacks(const PropertyConsumerCallbacks_T& callbacks)
    {
        bool retVal = false;
        if (nullptr == callbacks.sendMessage)
        {
            std::cout << "Error! SendMessage callback is null\n";
        }
        else if (nullptr == callbacks.onPropertyListReplyReceived)
        {
            std::cout << "Error! onPropertyListReplyReceived callback is null\n";
        }
        else if (nullptr == callbacks.onPropertyGetValueReplyRecieved)
        {
            std::cout << "Error! onPropertyGetValueReplyRecieved callback is null\n";
        }
        else if (nullptr == callbacks.onPropertySetValueReplyRecieved)
        {
            std::cout << "Error! onPropertySetValueReplyRecieved callback is null\n";
        }
        else
        {
            SendMessage = callbacks.sendMessage;
            OnPropertyListReplyReceived = callbacks.onPropertyListReplyReceived;
            OnPropertyGetValueReplyRecieved = callbacks.onPropertyGetValueReplyRecieved;
            OnPropertySetValueReplyRecieved = callbacks.onPropertySetValueReplyRecieved;
            retVal = true;
        }
        return retVal;
    }

    PropertyConsumerStatus_E PropertyConsumer_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto retVal = PropertyConsumerStatus_E::Error;
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
                retVal = PropertyConsumerStatus_E::Success;
                std::cout << "Message added to queue\n";
            }
        }
        return retVal; 
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

    PropertyConsumerStatus_E PropertyConsumer_C::SendPropertyListRequest()
    {
        PropertyConsumerStatus_E retVal = PropertyConsumerStatus_E::Error;
        if (PropertyConsumerState_E::Initialized == _state)
        {
            std::cout << "Sending Property List request\n";
            std::vector<uint8_t> buffer;
            auto propertyListReq = GetPropertyListRequest();
            _msgSerializer.Serialize(propertyListReq, buffer);
            if (0 < SendMessage(buffer))
            {
                _propertyListRequested = true;
                _propertyListReceived = false;
                retVal = PropertyConsumerStatus_E::Success;
                std::cout << "Sent widget list request\n";
            }
            else
            {
                retVal = PropertyConsumerStatus_E::FailedToSendMsg;
            }
        }
        else if (PropertyConsumerState_E::PropertyListRequested == _state)
        {
            retVal = PropertyConsumerStatus_E::RequestInProgress;
            std::cout << "Error! Property list request already sent\n";
        }
        else if (PropertyConsumerState_E::Uninitialized == _state)
        {
            retVal = PropertyConsumerStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            std::cout << "Error! Property list request cannot be sent in current state\n";
        }
        return retVal;
    }

    PropertyConsumerStatus_E PropertyConsumer_C::SendGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds)
    {
        PropertyConsumerStatus_E retVal = PropertyConsumerStatus_E::Error;
        if (PropertyConsumerState_E::Ready == _state)
        {
            std::cout << "Sending Get Property request\n";
            std::vector<uint8_t> buffer;
            auto getPropertyRequest = GetPropertyGetValueRequest(maxResponseLength, propertyIds);
            _msgSerializer.Serialize(getPropertyRequest, buffer);
            if (0 < SendMessage(buffer))
            {
                retVal = PropertyConsumerStatus_E::Success;
                _propertyGetValueReqSent = true;
                _propertyGetValueReplyReceived = false;
                std::cout << "Sent Get Property Value request\n";
            }
            else
            {
                retVal = PropertyConsumerStatus_E::FailedToSendMsg;
                std::cout << "Failed to send Get Property Value request\n";
            }
        }
        else if (PropertyConsumerState_E::Initialized == _state)
        {
            retVal = PropertyConsumerStatus_E::PropertyListNotReceived;
            std::cout << "Error! Property list not received!\n";
        }
        else if (PropertyConsumerState_E::Error == _state)
        {
            retVal = PropertyConsumerStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (PropertyConsumerState_E::Uninitialized == _state)
        {
            retVal = PropertyConsumerStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = PropertyConsumerStatus_E::RequestInProgress;
            std::cout << "Error! Request in progress.\n";
        }
        return retVal;
    }

    PropertyConsumerStatus_E PropertyConsumer_C::SendSetValueRequest(uint16_t propertyId, PropertyStorageVariant& value)
    {
        PropertyConsumerStatus_E retVal = PropertyConsumerStatus_E::Error;
        if (PropertyConsumerState_E::Ready == _state)
        {
            std::cout << "Sending Set Property request\n";
            std::vector<uint8_t> buffer;
            auto setPropertyRequest = GetPropertySetValueRequest(propertyId, value);
            _msgSerializer.Serialize(setPropertyRequest, buffer);
            if (0 < SendMessage(buffer))
            {
                retVal = PropertyConsumerStatus_E::Success;
                _propertySetValueReqSent = true;
                _propertySetValueReplyReceived = false;
                std::cout << "Sent Set Property Value request\n";
            }
            else
            {
                retVal = PropertyConsumerStatus_E::FailedToSendMsg;
                std::cout << "Failed to send Set Property Value request\n";
            }
        }
        else if (PropertyConsumerState_E::Initialized == _state)
        {
            retVal = PropertyConsumerStatus_E::PropertyListNotReceived;
            std::cout << "Error! Property list not received!\n";
        }
        else if (PropertyConsumerState_E::Error == _state)
        {
            retVal = PropertyConsumerStatus_E::Error;
            std::cout << "Error! Currently in Error State!\n";
        }
        else if (PropertyConsumerState_E::Uninitialized == _state)
        {
            retVal = PropertyConsumerStatus_E::NotInitialized;
            std::cout << "Error! Initialize must be called first!\n";
        }
        else
        {
            retVal = PropertyConsumerStatus_E::RequestInProgress;
            std::cout << "Error! Request in progress.\n";
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
        if (true == _errorOccured)
        {
            _state = PropertyConsumerState_E::Error;
        }
        switch (_state)
        {
            case PropertyConsumerState_E::Uninitialized:
                if (true == _initialized)
                {
                    _state = PropertyConsumerState_E::Initialized;
                }
                break;
            case PropertyConsumerState_E::Initialized:
                if (true == _propertyListRequested)
                {
                    _state = PropertyConsumerState_E::PropertyListRequested;
                }
                break;

            case PropertyConsumerState_E::PropertyListRequested:
                if (true == _propertyListReceived)
                {
                    _state = PropertyConsumerState_E::Ready;
                }
                break;
            
            case PropertyConsumerState_E::Ready:
                if (true == _propertyGetValueReqSent)
                {
                    _state = PropertyConsumerState_E::PropertyGetValReqSent;
                }
                else if (true == _propertySetValueReqSent)
                {
                    _state = PropertyConsumerState_E::PropertySetValReqSent;
                }
                break;

            case PropertyConsumerState_E::PropertyGetValReqSent:
                if (true == _propertyGetValueReplyReceived)
                {
                    _propertyGetValueReqSent = false;
                    _propertyGetValueReplyReceived = false;
                    _state = PropertyConsumerState_E::Ready;
                }
                break;

            case PropertyConsumerState_E::PropertySetValReqSent:
                if (true == _propertySetValueReplyReceived)
                {
                    _propertySetValueReqSent = false;
                    _propertySetValueReplyReceived = false;
                    _state = PropertyConsumerState_E::Ready;
                }
                break;

            case PropertyConsumerState_E::Error:
                break;

            default:
                break;
        }
    }

    void PropertyConsumer_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.Dequeue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
        switch (static_cast<MessageId_E>(msgId))
        {
            case MessageId_E::PropertyListReply:
                std::cout << "Received a Property List Reply\n";
                ProcessReceivedPropertyListReply(msg);
                break;

            case MessageId_E::GetValueReply:
                std::cout << "Received a Property Get Value Reply\n";
                ProcessReceivedPropertyGetValueReply(msg);
                break;

            case MessageId_E::SetValueReply:
                std::cout << "Received a Property Set Value Reply\n";
                ProcessReceivedPropertySetValueReply(msg);
                break;

            default:
                std::cout << "Error! Unknown message received\n";
                break;
        }
    }

    void PropertyConsumer_C::ProcessReceivedPropertyListReply(Message_T& msg)
    {
        if (PropertyConsumerState_E::PropertyListRequested != _state)
        {
            std::cout << "Error! Property List Reply received, but no request was sent\n";
        }
        else 
        {
            PropertyListReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (PropertyGathererReplyStatus_E::Success == static_cast<PropertyGathererReplyStatus_E>(reply.status))
            {
                _propertyListReceived = true;
                _propertyDescriptorList = reply.propertyDescriptorList;
                ProcessStateMachine();
            }
            OnPropertyListReplyReceived(static_cast<PropertyGathererReplyStatus_E>(reply.status), reply.propertyDescriptorList);
        }
    }

    void PropertyConsumer_C::ProcessReceivedPropertyGetValueReply(Message_T& msg)
    {
        if (PropertyConsumerState_E::PropertyGetValReqSent != _state)
        {
            std::cout << "Error! Property Get Value Reply received, but no request was sent\n";
        }
        else 
        {
            GetValueReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (PropertyGathererReplyStatus_E::Success != static_cast<PropertyGathererReplyStatus_E>(reply.status))
            {
                std::cout << "Error! Property Get Value Reply status was not success!\n";
            }
            OnPropertyGetValueReplyRecieved(static_cast<PropertyGathererReplyStatus_E>(reply.status), reply.propValues);
            _propertyGetValueReplyReceived = true;
        }
    }

    void PropertyConsumer_C::ProcessReceivedPropertySetValueReply(Message_T& msg)
    {
        if (PropertyConsumerState_E::PropertySetValReqSent != _state)
        {
            std::cout << "Error! Property Set Value Reply received, but no request was sent\n";
        }
        else 
        {
            SetValueReply_T reply;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reply, msgBuf);

            if (PropertyGathererReplyStatus_E::Success != static_cast<PropertyGathererReplyStatus_E>(reply.status))
            {
                std::cout << "Error! Property Set Value Reply status was not success!\n";
            }
            OnPropertySetValueReplyRecieved(static_cast<PropertyGathererReplyStatus_E>(reply.status), reply.value);
            _propertySetValueReplyReceived = true;
        }
    }
}