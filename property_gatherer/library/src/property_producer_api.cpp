#include "property_producer_api.h"

namespace PropertyGatherer
{
    PropertyProducer_C::PropertyProducer_C()
    {

    }
    
    PropertyProducer_C::~PropertyProducer_C()
    {
        
    }

    PropertyProducerStatus_E PropertyProducer_C::PropertyProducer_Initialize(PropertyProducerInitParams_T& initParams)
    {
        auto retVal = PropertyProducerStatus_E::Error;
        if (false == SetCallbacks(initParams.callbacks))
        {
            std::cout << "Error! Failed to set callbacks\n";
            _errorOccured = true;
        }
        else if (false == SetPropertyList(initParams.propertyList))
        {
            std::cout << "Error! Failed to set property list\n";
            _errorOccured = true;
        }
        else
        {
            retVal = PropertyProducerStatus_E::Success;
            _initialized = true;
        }
        return retVal;
    }

    bool PropertyProducer_C::SetCallbacks(const PropertyProducerCallbacks_T& callbacks)
    {
        bool retVal = false;
        if (nullptr == callbacks.sendMessage)
        {
            std::cout << "Error! SendMessage callback is null\n";
        }
        else if (nullptr == callbacks.onPropertyListRequestReceived)
        {
            std::cout << "Error! onPropertyListRequestReceived callback is null\n";
        }
        else if (nullptr == callbacks.onPropertyGetValueRequestRecieved)
        {
            std::cout << "Error! onPropertyGetValueRequestRecieved callback is null\n";
        }
        else if (nullptr == callbacks.onPropertySetValueRequestRecieved)
        {
            std::cout << "Error! onPropertySetValueRequestRecieved callback is null\n";
        }
        else 
        {
            SendMessage = callbacks.sendMessage;
            OnPropertyListRequestReceived = callbacks.onPropertyListRequestReceived;
            OnPropertyGetValueRequestRecieved = callbacks.onPropertyGetValueRequestRecieved;
            OnPropertySetValueRequestRecieved = callbacks.onPropertySetValueRequestRecieved;
            retVal = true;
        }
        return retVal;
    }

    bool PropertyProducer_C::SetPropertyList(const std::vector<PropertyDescriptor_T>& descList)
    {
        bool retVal = true;
        for (int i = 0; i < descList.size(); i++)
        {
            auto desc = descList[i];
            auto it = _propertyMap.find(desc.propertyId);
            if (it == _propertyMap.end())
            {
                _propertyMap[desc.propertyId] = desc;
            }
            else
            {
                std::cout << "Cannot add duplicate property " << desc.propertyId << ":" << desc.propertyName << "\n";
                retVal = false;
            }
        }
        return retVal;
    }
    
    PropertyProducerStatus_E PropertyProducer_C::PropertyProducer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto retVal = PropertyProducerStatus_E::Error;
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
                retVal = PropertyProducerStatus_E::Success;
                std::cout << "Message added to queue\n";
            }
        }
        return retVal; 
    }

    void PropertyProducer_C::PropertyProducer_ProcessTimedActivities()
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

    uint64_t PropertyProducer_C::GetCurrentTimeMs()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
                ).count()
            );
    }

    void PropertyProducer_C::ProcessStateMachine()
    {
        if (true == _errorOccured)
        {
            _state = PropertyProducerState_E::Error;
        }
        switch (_state)
        {
            case PropertyProducerState_E::Uninitialized:
                if (true == _initialized)
                {
                    _state = PropertyProducerState_E::Initialized;
                }
                break;
            case PropertyProducerState_E::Initialized:
                if (true == _propertyListReplySent)
                {
                    _state = PropertyProducerState_E::Ready;
                    std::cout << "Setting state to Property List Populated\n";
                }
                break;
            case PropertyProducerState_E::Ready:
                break;

            case PropertyProducerState_E::Error:
                break;

            default:
                break;
        }
    }

    void PropertyProducer_C::ProcessReceivedMessageQueue()
    {
        if (PropertyProducerState_E::Uninitialized != _state && 
            PropertyProducerState_E::Error != _state)
        {
            auto msg = _msgQueue.Dequeue();
            uint16_t msgId = (static_cast<uint8_t>(msg.data[3] << 8) | static_cast<uint8_t>(msg.data[2]));

            switch (static_cast<MessageId_E>(msgId))
            {
                case MessageId_E::PropertyListReq:
                    std::cout << "Received Property List Request\n";
                    ProcessReceivedPropertyListRequest();
                    break;
                case MessageId_E::GetValueRequest:
                    std::cout << "Received Property Get Value Request\n";
                    ProcessReceivedPropertyGetValueRequest(msg);
                    break;
                case MessageId_E::SetValueRequest:
                    std::cout << "Received Property Set Value Request\n";
                    ProcessReceivedPropertySetValueRequest(msg);
                    break;
                default:
                    std::cout << "Unknown message ID: " << msgId << "\n";
                    break;
            }
        }
    }

    void PropertyProducer_C::ProcessReceivedPropertyListRequest()
    {
        if (PropertyProducerState_E::Initialized == _state)
        {
            std::vector<uint8_t> buffer;

            // Construct a vector of property descriptors from the map
            std::vector<PropertyDescriptor_T> propertyDescriptors;
            for (auto& [propertyId, propertDesc] : _propertyMap)
            {
                propertyDescriptors.push_back(propertDesc);
            }

            auto status = PropertyGathererReplyStatus_E::Success;
            auto propertyListReply = GetPropertyListReply(propertyDescriptors, status);
            _msgSerializer.Serialize(propertyListReply, buffer);

            std::cout << "Sending Property List reply\n";
            OnPropertyListRequestReceived(propertyDescriptors);

            if (0 < SendMessage(buffer))
            {
                _propertyListReplySent = true;
                std::cout << "Sent property list reply\n";
            }
        }
        else
        {
            std::cout << "Error! Property List already sent!\n";
        }
    }

    void PropertyProducer_C::ProcessReceivedPropertyGetValueRequest(Message_T& msg)
    {
        auto status = PropertyGathererReplyStatus_E::Error;
        std::vector<PropertyStorageVariant> propertyGetValueResponseList;
        if (PropertyProducerState_E::Ready == _state)
        {
            status = PropertyGathererReplyStatus_E::Success;
            GetValueReq_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Populate a list of Property Storage Variants */
            std::vector<PropertyValueContainer_T> propertyContainerList;
            for (auto& propertyId : reqMsg.propIds)
            {
                auto it = _propertyMap.find(propertyId);
                if (it != _propertyMap.end() && true == it->second.isReadable)
                {
                    PropertyValueContainer_T valueContainer;
                    valueContainer.propertyId = propertyId;
                    propertyContainerList.push_back(valueContainer);
                }
                else if (it != _propertyMap.end() && false == it->second.isReadable)
                {
                    std::cout << "Property ID " << propertyId << " is not readable\n";
                    status = PropertyGathererReplyStatus_E::AccessError;
                }
                else
                {
                    std::cout << "Property ID " << propertyId << " is not invalid\n";
                    status = PropertyGathererReplyStatus_E::InvalidPropertyId;
                }
            }
            
            /* If no errors were caught populating the list pass the Get Value Req list to the Application */
            if (PropertyGathererReplyStatus_E::Success == status)
            {
                status = OnPropertyGetValueRequestRecieved(propertyContainerList);
            }

            /* If errors occured during Get Value Req list construction or after providing to the client Send back an empty Get Value List */
            if (PropertyGathererReplyStatus_E::Success != status)
            {
                std::cout << "Error! Property Set Value Request failed\n";
            }
            else
            {
                for (auto& propertyContainer : propertyContainerList)
                {
                    propertyGetValueResponseList.push_back(propertyContainer.value);
                }
            }
        }
        else
        {
            status = PropertyGathererReplyStatus_E::Busy;
        }

        std::vector<uint8_t> buffer;
        auto propertyGetValRep = GetPropertyGetValueReply(propertyGetValueResponseList, status);
        _msgSerializer.Serialize(propertyGetValRep, buffer);
        if (0 < SendMessage(buffer))
        {
            _propertyGetValueReplySent = true;
        }
    }

    void PropertyProducer_C::ProcessReceivedPropertySetValueRequest(Message_T& msg)
    {
        PropertyStorageVariant propertyValue;
        auto status = PropertyGathererReplyStatus_E::Error;
        if (PropertyProducerState_E::Ready == _state)
        {
            status = PropertyGathererReplyStatus_E::Success;
            SetValueReq_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            PropertyValueContainer_T propertyValueContainer;
            propertyValueContainer.propertyId = reqMsg.propId;
            propertyValueContainer.value = reqMsg.value;

            /* Check to see that Property Id requested to set is in the Property List and is Writeable */
            auto it = _propertyMap.find(reqMsg.propId);
            if (it != _propertyMap.end() && true == it->second.isWriteable)
            {
                status = OnPropertySetValueRequestRecieved(propertyValueContainer);
            }
            else if (it != _propertyMap.end() && false == it->second.isWriteable)
            {
                std::cout << "Property ID " << reqMsg.propId << " is not writeable\n";
                status = PropertyGathererReplyStatus_E::AccessError;
            }
            else
            {
                std::cout << "Property ID " << reqMsg.propId << " is not invalid\n";
                status = PropertyGathererReplyStatus_E::InvalidPropertyId;
            }

            if (PropertyGathererReplyStatus_E::Success != status)
            {
                std::cout << "Error! Property Set Value Request failed\n";
            }
            
            /* Value set by user is the value to send back to the Consumer */
            propertyValue = propertyValueContainer.value;
        }
        else
        {
            status = PropertyGathererReplyStatus_E::Busy;
        }

        std::vector<uint8_t> buffer;
        auto propertySetValRep = GetPropertySetValueReply(propertyValue, status);
        _msgSerializer.Serialize(propertySetValRep, buffer);
        if (0 < SendMessage(buffer))
        {
            _propertySetValueReplySent = true;
        }
    }
}