#include "property_producer_api.h"

namespace PropertyGatherer
{
    PropertyProducer_C::PropertyProducer_C()
    {

    }
    
    PropertyProducer_C::~PropertyProducer_C()
    {
        
    }

    void PropertyProducer_C::SetCallbacks(const PropertyProducerCallbacks_T& callbacks)
    {
        SendMessage = callbacks.sendMessage;
        OnPropertyListRequestReceived = callbacks.onPropertyListRequestReceived;
        OnPropertyGetValueRequestRecieved = callbacks.onPropertyGetValueRequestRecieved;
    }

    void PropertyProducer_C::PropertyProducer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto queueSizeBeforeAdd = _msgQueue.Size();
        Message_T rxMsg = { std::move(msg), size };
        _msgQueue.Enqueue(std::move(rxMsg));

        if (queueSizeBeforeAdd == _msgQueue.Size())
        {
            std::cout << "Failed to add message to queue\n";
        }
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

    bool PropertyProducer_C::SetPropertyList(std::vector<PropertyDescriptor_T>& descList)
    {
        bool retVal = false;

        if (false == _propertListPopulated)
        {
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
                }
            }
            _propertListPopulated = true;
        }
        return true;
    }

    void PropertyProducer_C::SetPropertyValue(std::vector<std::pair<uint16_t, PropertyStorageVariant>>& propertyValues)
    {
        for (auto& [propertyId, propertyValue] : propertyValues)
        {
            auto it = _propertyMap.find(propertyId);
            if (it != _propertyMap.end())
            {
                _propertyValues[propertyId] = propertyValue;
            }
            else
            {
                std::cout << "Property ID " << propertyId << " not found in property map\n";
            }
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
        switch (_state)
        {
            case PropertyProducerState_E::INITIALIZED:
                if (true == _propertListPopulated)
                {
                    _state = PropertyProducerState_E::PROPERTY_LIST_POPULATED;
                    std::cout << "Setting state to Property List Populated\n";
                }
                break;
            case PropertyProducerState_E::PROPERTY_LIST_POPULATED:
                if (true == _propertyListReplySent)
                {
                    _state = PropertyProducerState_E::PROPERTY_LIST_REPLY_SENT;
                }
                break;
        }
    }

    void PropertyProducer_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.Dequeue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3] << 8) | static_cast<uint8_t>(msg.data[2]));

        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::PROPERTY_LIST_REQ:
                std::cout << "Received Property List Request\n";
                ProcessReceivedPropertyListRequest();
                break;
            case MessageID_E::GET_VALUE_REQUEST:
                std::cout << "Received Property Get Value Request\n";
                ProcessReceivedPropertyGetValueRequest(msg);
                break;
        }
    }

    void PropertyProducer_C::ProcessReceivedPropertyListRequest()
    {
        if (PropertyProducerState_E::PROPERTY_LIST_POPULATED == _state)
        {
            std::vector<uint8_t> buffer;

            // Construct a vector of property descriptors from the map
            std::vector<PropertyDescriptor_T> propertyDescriptors;
            // propertyDescriptors.reserve(_propertyMap.size());

            for (auto& [propertyId, propertDesc] : _propertyMap)
            {
                propertyDescriptors.push_back(propertDesc);
            }

            auto status = PropertyReplyStatus_E::SET_VAL_SUCCESS;
            auto propertyListReply = GetPropertyListReply(propertyDescriptors, status);
            _msgSerializer.Serialize(propertyListReply, buffer);

            std::cout << "Sending Property List reply\n";

            if (0 < SendMessage(buffer))
            {
                _propertyListReplySent = true;
                std::cout << "Sent property list reply\n";
            }
        }
        else if (PropertyProducerState_E::INITIALIZED == _state)
        {
            std::cout << "Error! Property List not populated yet!\n";
        }
        else
        {
            std::cout << "Error! Property List already received!\n";
        }
    }

    void PropertyProducer_C::ProcessReceivedPropertyGetValueRequest(Message_T& msg)
    {
        if (true == _propertListPopulated && true == _propertyListReplySent)
        {
            GetValueReq_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Generate a list of Property Storage Variants */
            std::vector<PropertyStorageVariant> propertyGetValueResponseList;

            for (auto& propertyToGet : reqMsg.propIds)
            {
                auto val = _propertyValues.find(propertyToGet);
                auto desc = _propertyMap.find(propertyToGet);
                if (val != _propertyValues.end() && 
                    desc != _propertyMap.end() &&
                    desc->second.isReadable) 
                {
                    propertyGetValueResponseList.push_back(_propertyValues[propertyToGet]);
                }
                else if (desc != _propertyMap.end() && false == desc->second.isReadable)
                {
                    std::cout << "Property ID " << propertyToGet << " is not readable\n";
                }
                else 
                {
                    std::cout << "Property ID " << propertyToGet << " not found in property values\n";
                }
            }

            auto status = OnPropertyGetValueRequestRecieved(propertyGetValueResponseList);

            std::vector<uint8_t> buffer;
            auto propertyGetValRep = GetPropertyGetValueReply(propertyGetValueResponseList, status);
            _msgSerializer.Serialize(propertyGetValRep, buffer);
            if (0 < SendMessage(buffer))
            {
                _propertyGetValueReplySent = true;
            }
        }
        else
        {
            std::cout << "Error! Property List not received\n";
        }
    }
}