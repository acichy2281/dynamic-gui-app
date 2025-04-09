#include "property_gatherer_messages.h"

namespace PropertyGatherer
{

    PropertyListRequest_T GetPropertyListRequest()
    {
        PropertyListRequest_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::PropertyListReq);
        return retVal;
    }

    PropertyListReply_T GetPropertyListReply(std::vector<PropertyDescriptor_T>& descList, PropertyGathererReplyStatus_E status)
    {
        PropertyListReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::PropertyListReply);
        retVal.numProperties = descList.size();
        retVal.propertyDescriptorList = descList;
        retVal.status = static_cast<uint16_t>(status);
        return retVal;
    }

    GetValueReq_T GetPropertyGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds)
    {
        GetValueReq_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::GetValueRequest);
        retVal.maxRespLen = maxResponseLength;
        retVal.numReqProp = static_cast<uint16_t>(propertyIds.size());
        retVal.propIds = propertyIds;
        return retVal;
    }

    GetValueReply_T GetPropertyGetValueReply(std::vector<PropertyStorageVariant> values, PropertyGathererReplyStatus_E status)
    {
        GetValueReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::GetValueReply);
        retVal.propValues = values;
        retVal.status = static_cast<uint16_t>(status);
        return retVal;
    }

    SetValueReq_T GetPropertySetValueRequest(uint16_t propertyId, PropertyStorageVariant& value)
    {
        SetValueReq_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::SetValueRequest);
        retVal.propId = propertyId;
        retVal.value = value;
        return retVal;
    }

    SetValueReply_T GetPropertySetValueReply(PropertyStorageVariant& value, PropertyGathererReplyStatus_E status)
    {
        SetValueReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageId_E::SetValueReply);
        retVal.value = value;
        retVal.status = static_cast<uint16_t>(status);
        return retVal;
    }

    PropertyGathererMessageSerializer::PropertyGathererMessageSerializer()
    {

    }

    PropertyGathererMessageSerializer::~PropertyGathererMessageSerializer()
    {

    }

    void PropertyGathererMessageSerializer::SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff)
    {
        uint8_t bufSize = outBuff.size();

        /* Serialize Protocol ID High */
        outBuff.resize(bufSize + sizeof(header.protocolIdHigh));
        std::memcpy(outBuff.data() + bufSize, &header.protocolIdHigh, sizeof(header.protocolIdHigh));
        bufSize = outBuff.size(); 

        /* Serialize Protocol ID Low */
        outBuff.resize(bufSize + sizeof(header.protocolIdLow));
        std::memcpy(outBuff.data() + bufSize, &header.protocolIdLow, sizeof(header.protocolIdLow));
        bufSize = outBuff.size(); 

        /* Serialize Message ID */
        outBuff.resize(bufSize + sizeof(header.messageId));
        std::memcpy(outBuff.data() + bufSize, &header.messageId, sizeof(header.messageId));
        bufSize = outBuff.size(); 
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(PropertyListRequest_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();
        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(PropertyListReply_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Number of properties */
        outBuff.resize(bufSize + sizeof(pMessage.numProperties));
        std::memcpy(outBuff.data() + bufSize, &pMessage.numProperties, sizeof(pMessage.numProperties));
        bufSize = outBuff.size();

        /* Serialize property descriptors */
        for (auto& propDescriptor : pMessage.propertyDescriptorList)
        {
            bufSize += SerializePropertyDescriptor(propDescriptor, outBuff);
        }

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::SerializePropertyDescriptor(PropertyDescriptor_T& pDesc, std::vector<uint8_t>& outBuff)
    {
        uint16_t bufSize = outBuff.size();

        /* Serialize property descriptor length */
        outBuff.resize(bufSize + sizeof(pDesc.propertyDescriptorLength));
        std::memcpy(outBuff.data() + bufSize, &pDesc.propertyDescriptorLength, sizeof(pDesc.propertyDescriptorLength));
        bufSize = outBuff.size();
        
        /* Serialize property id */
        outBuff.resize(bufSize + sizeof(pDesc.propertyId));
        std::memcpy(outBuff.data() + bufSize, &pDesc.propertyId, sizeof(pDesc.propertyId));
        bufSize = outBuff.size();

        /* Serialize property flags */
        uint16_t packedFlags = 
        (static_cast<uint16_t>(pDesc.isWriteable) << 15) |
        (static_cast<uint16_t>(pDesc.isReadable) << 14) |
        (static_cast<uint16_t>(pDesc.isSubscribable) << 13) |
        (static_cast<uint16_t>(pDesc.isStatic) << 12) |
        (pDesc.reservedForFlags & 0xFFF);

        outBuff.resize(bufSize + sizeof(packedFlags));
        std::memcpy(outBuff.data() + bufSize, &packedFlags, sizeof(packedFlags));
        bufSize = outBuff.size();

        /* Serialize property type */
        outBuff.resize(bufSize + sizeof(pDesc.propertyType));
        std::memcpy(outBuff.data() + bufSize, &pDesc.propertyType, sizeof(pDesc.propertyType));
        bufSize = outBuff.size();

        /* Serialize reserved bits (1 byte) */
        outBuff.resize(bufSize + sizeof(pDesc.reserved));
        std::memcpy(outBuff.data() + bufSize, &pDesc.reserved, sizeof(pDesc.reserved));
        bufSize = outBuff.size();

        /* Serialize property length */
        outBuff.resize(bufSize + sizeof(pDesc.propertyLength));
        std::memcpy(outBuff.data() + bufSize, &pDesc.propertyLength, sizeof(pDesc.propertyLength));
        bufSize = outBuff.size();

        /* Serialize property name */
        outBuff.insert(outBuff.end(), pDesc.propertyName.begin(), pDesc.propertyName.end());
        outBuff.push_back('\0');
        bufSize = outBuff.size();

        /* Serialize property units */
        outBuff.resize(bufSize + sizeof(pDesc.propertyUnits));
        std::memcpy(outBuff.data() + bufSize, &pDesc.propertyUnits, sizeof(pDesc.propertyUnits));
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(GetValueReq_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Max Response Length */
        outBuff.resize(bufSize + sizeof(pMessage.maxRespLen));
        std::memcpy(outBuff.data() + bufSize, &pMessage.maxRespLen, sizeof(pMessage.maxRespLen));
        bufSize = outBuff.size(); 

        /* Serialize Number of Requested Properties */
        outBuff.resize(bufSize + sizeof(pMessage.numReqProp));
        std::memcpy(outBuff.data() + bufSize, &pMessage.numReqProp, sizeof(pMessage.numReqProp));
        bufSize = outBuff.size(); 

        /* Serialize List of Requested Property Ids */
        for (auto& propertyId : pMessage.propIds)
        {
            outBuff.resize(bufSize + sizeof(propertyId));
            std::memcpy(outBuff.data() + bufSize, &propertyId, sizeof(propertyId));
            bufSize = outBuff.size(); 
        }

        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(GetValueReply_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize List of Requested Property Ids */
        for (auto& properties : pMessage.propValues)
        {
            bufSize += SerializeVariant(properties, outBuff);
        }

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size(); 
        
        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(SetValueReq_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize property id */
        outBuff.resize(bufSize + sizeof(pMessage.propId));
        std::memcpy(outBuff.data() + bufSize, &pMessage.propId, sizeof(pMessage.propId));
        bufSize = outBuff.size(); 

        /* Serialize property value */
        bufSize += SerializeVariant(pMessage.value, outBuff);

        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::Serialize(SetValueReply_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize property value */
        bufSize += SerializeVariant(pMessage.value, outBuff);

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size(); 

        return bufSize;
    }

    uint16_t PropertyGathererMessageSerializer::SerializeVariant(const PropertyStorageVariant& variant, std::vector<uint8_t>& outBuff)
    {
        // Serialize the type index (or type identifier)
         uint8_t typeIndex = static_cast<uint8_t>(variant.index());
         outBuff.push_back(typeIndex);  // Store the type index at the beginning of the serialized data

        // switch (typeIndex)
        // {
        //     case UNSIGNED_8_BIT_INT:
        //         outBuff.resize(outBuff.size() + sizeof(uint8_t));
        //         std::memcpy();
                
        // }
        return std::visit([&outBuff](const auto& value) -> uint16_t
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::string>)
            {
                size_t oldSize = outBuff.size();
                size_t strSize = value.size() + 1; // +1 for null terminator

                outBuff.resize(oldSize + strSize);
                std::memcpy(outBuff.data() + oldSize, value.c_str(), strSize); // Include null terminator

                return static_cast<uint16_t>(strSize);
            }
            else
            {
                auto bufSize = outBuff.size();
                outBuff.resize(bufSize + sizeof(T));
                std::memcpy(outBuff.data() + bufSize, &value, sizeof(T));
                return sizeof(T);
            }
        }, variant);
    }

    void PropertyGathererMessageSerializer::DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf)
    {
        uint8_t bufIndex = 0;

        /* Deserialize Protocol ID High */
        std::memcpy(&header.protocolIdHigh, msgBuf.data() + bufIndex, sizeof(header.protocolIdHigh));
        bufIndex += sizeof(header.protocolIdHigh);

        /* Deserialize Protocol ID Low */
        std::memcpy(&header.protocolIdLow, msgBuf.data() + bufIndex, sizeof(header.protocolIdLow));
        bufIndex += sizeof(header.protocolIdLow);

        /* Deserialize Message ID */
        std::memcpy(&header.messageId, msgBuf.data() + bufIndex, sizeof(header.messageId));
        bufIndex += sizeof(header.messageId);
    }

    bool PropertyGathererMessageSerializer::Deserialize(PropertyListRequest_T& pMessage, std::vector<uint8_t>& msgBuf) 
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        return true;
    }

    bool PropertyGathererMessageSerializer::Deserialize(PropertyListReply_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize number of properties */
        std::memcpy(&pMessage.numProperties, msgBuf.data() + bufIndex, sizeof(pMessage.numProperties));
        bufIndex += sizeof(pMessage.numProperties);

        /* Deserialize property descriptors */
        for (int i = 0; i < pMessage.numProperties; i++) {
            PropertyDescriptor_T propDesc;
            DeserializePropertyDescriptor(propDesc, msgBuf, bufIndex);
            pMessage.propertyDescriptorList.push_back(propDesc);
        }

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }

    void PropertyGathererMessageSerializer::DeserializePropertyDescriptor(PropertyDescriptor_T& pDesc, std::vector<uint8_t>& msgBuff, uint16_t& offset)
    {
        /* Deserialize property descriptor length */
        std::memcpy(&pDesc.propertyDescriptorLength, msgBuff.data() + offset, sizeof(pDesc.propertyDescriptorLength));
        offset += sizeof(pDesc.propertyDescriptorLength);

        /* Deserialize property id */
        std::memcpy(&pDesc.propertyId, msgBuff.data() + offset, sizeof(pDesc.propertyId));
        offset += sizeof(pDesc.propertyId);

        /* Deserialize flags and reserved space */
        uint16_t packedFlags = msgBuff[offset];
        pDesc.isWriteable = (packedFlags >> 15) & 0x01;
        pDesc.isReadable = (packedFlags >> 14) & 0x01;
        pDesc.isSubscribable = (packedFlags >> 13) & 0x01;
        pDesc.isStatic = (packedFlags >> 12) & 0x01;
        pDesc.reservedForFlags = packedFlags & 0xFFF;
        offset += sizeof(packedFlags);

        /* Deserialize property type */
        pDesc.propertyType = msgBuff[offset];
        offset += 1;

        /* Deserialize reserved bits */
        pDesc.reserved = msgBuff[offset];
        offset += 1;

        /* Deserialize property length */
        std::memcpy(&pDesc.propertyLength, msgBuff.data() + offset, sizeof(pDesc.propertyLength));
        offset += sizeof(pDesc.propertyLength);

        /* Deserialize property name */
        size_t startIdx = offset;
        while (offset < msgBuff.size() && msgBuff[offset] != '\0') 
        {
            ++offset;
        }

        if (offset < msgBuff.size())
        {
            pDesc.propertyName = std::string(reinterpret_cast<const char*>(&msgBuff[startIdx]), offset - startIdx);
            ++offset;
        }
        else
        {
            pDesc.propertyName.clear();
        }

        /* Deserialize property units */
        std::memcpy(&pDesc.propertyUnits, msgBuff.data() + offset, sizeof(pDesc.propertyUnits));
        offset += sizeof(pDesc.propertyUnits);
    }

    bool PropertyGathererMessageSerializer::Deserialize(GetValueReq_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize Max Response Length */
        std::memcpy(&pMessage.maxRespLen, msgBuf.data() + bufIndex, sizeof(pMessage.maxRespLen));
        bufIndex += sizeof(pMessage.maxRespLen);

        /* Deserialize Number of Requested Properties */
        std::memcpy(&pMessage.numReqProp, msgBuf.data() + bufIndex, sizeof(pMessage.numReqProp));
        bufIndex += sizeof(pMessage.numReqProp);

        /* Deserialize List of Requested Property Ids */
        pMessage.propIds.resize(pMessage.numReqProp);
        for (auto& propertyId : pMessage.propIds)
        {
            std::memcpy(&propertyId, msgBuf.data() + bufIndex, sizeof(propertyId));
            bufIndex += sizeof(propertyId); 
        }

        return true;
    }

    bool PropertyGathererMessageSerializer::Deserialize(GetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize variants */
        while (bufIndex < (msgBuf.size() - sizeof(pMessage.status)))
        {
            pMessage.propValues.push_back(DeserializeVariant(msgBuf, bufIndex));
        }

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }

    bool PropertyGathererMessageSerializer::Deserialize(SetValueReq_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize property id */
        std::memcpy(&pMessage.propId, msgBuf.data() + bufIndex, sizeof(pMessage.propId));
        bufIndex += sizeof(pMessage.propId);

        /* Deserialize property value */
        pMessage.value = DeserializeVariant(msgBuf, bufIndex);

        return true;
    }

    bool PropertyGathererMessageSerializer::Deserialize(SetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize property value */
        pMessage.value = DeserializeVariant(msgBuf, bufIndex);

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }

    PropertyStorageVariant PropertyGathererMessageSerializer::DeserializeVariant(std::vector<uint8_t>& inBuff, uint16_t& index)
    {
        // Read the type index (which type was serialized)
        uint8_t typeIndex = inBuff[index++];
        
        PropertyStorageVariant result;

        // Based on the type index, read the appropriate data from the buffer
        switch (typeIndex) {
            case Signed8BitInt: { // int8_t            
                int8_t value = static_cast<int8_t>(inBuff[index]);
                index += sizeof(int8_t);
                return value;
            }
            case Signed16BitInt: { // int16_t
                int16_t value;
                std::memcpy(&value, &inBuff[index], sizeof(int16_t));
                index += sizeof(int16_t);
                return value;
            }
            case Signed32BitInt: { // int32_t
                int32_t value;
                std::memcpy(&value, &inBuff[index], sizeof(int32_t));
                index += sizeof(int32_t);
                return value;
            }
            case Signed64BitInt: { // int64_t
                int64_t value;
                std::memcpy(&value, &inBuff[index], sizeof(int64_t));
                index += sizeof(int64_t);
                return value;
            }
            case Unsigned8BitInt: { // uint8_t
                uint8_t value = inBuff[index];
                index += sizeof(uint8_t);
                return value;
            }
            case Unsigned16BitInt: { // uint16_t
                uint16_t value;
                std::memcpy(&value, &inBuff[index], sizeof(uint16_t));
                index += sizeof(uint16_t);
                return value;
            }
            case Unsigned32BitInt: { // uint32_t
                uint32_t value;
                std::memcpy(&value, &inBuff[index], sizeof(uint32_t));
                index += sizeof(uint32_t);
                return value;
            }
            case Unsigned64BitInt: { // uint64_t
                uint64_t value;
                std::memcpy(&value, &inBuff[index], sizeof(uint64_t));
                index += sizeof(uint64_t);
                return value;
            }
            case Float: { // float
                float value;
                std::memcpy(&value, &inBuff[index], sizeof(value));
                index += sizeof(value);
                return value;
            }
            case Double: { // double
                double value;
                std::memcpy(&value, &inBuff[index], sizeof(value));
                index += sizeof(value);
                return value;
            }
            case String: { // std::string
                uint16_t startIdx = index;
                while (index < inBuff.size() && inBuff[index] != '\0') 
                {
                    ++index;
                }
                std::string str(reinterpret_cast<const char*>(&inBuff[startIdx]), index - startIdx);
                ++index;
                result = str;
                break;
            }
            case Boolean: { // bool
                bool value = inBuff[index] != 0;
                index += sizeof(bool);
                return value;
            }
            default:
                throw std::runtime_error("Unknown type index during deserialization.");
        }

        return result;
    }
}