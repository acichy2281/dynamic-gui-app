#ifndef PROPERTY_GATHERER_MESSAGES_H
#define PROPERTY_GATHERER_MESSAGES_H

/* System Includes */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include <string>
#include <variant>
#include <iostream>
#include <memory>
#include <cstring>

/* Shared includes */
#include "custom_types.h"

namespace PropertyGatherer
{
    enum PropertyStorageVariantType_E 
    {
        Signed8BitInt,
        Signed16BitInt,
        Signed32BitInt,
        Signed64BitInt,
        Unsigned8BitInt,
        Unsigned16BitInt,
        Unsigned32BitInt,
        Unsigned64BitInt,
        Float,
        Double,
        String,
        Boolean,
    };

    using PropertyStorageVariant = std::variant<int8_t,
                                                int16_t,
                                                int32_t,
                                                int64_t,
                                                uint8_t,
                                                uint16_t,
                                                uint32_t,
                                                uint64_t,
                                                float,
                                                double,
                                                std::string,
                                                bool>;


    /* Type defs */
    typedef uint16_t PropertyId_T;

    enum class MessageId_E
    {
        ProtocolVersionReq,
        ProtocolVersionReply,
        PropertyListReq,
        PropertyListReply,
        GetValueRequest,
        GetValueReply,
        SetValueRequest,
        SetValueReply,
    };

    enum class PropertyGathererReplyStatus_E
    {
        Success,
        Busy,
        UnknownRequest,
        Error,
        InvalidParameter,
        InvalidPropertyId,
        InvalidPropertyValue,
        AccessError,
        MaxSubscriptionsExceeded,
        MaxResponseLenExceeded,
        ResourceError,
        Pending,
        PartialSuccess,
    };

    /* Structs */

    /**
     * @brief Generic Header for all message types
     */
    struct Header_T
    {
        uint8_t protocolIdHigh = PROPERTY_GATHERER_PROTOCOL_ID_HIGH;
        uint8_t protocolIdLow = PROPERTY_GATHERER_PROTOCOL_ID_LOW;
        uint16_t messageId;
    };

    struct PropertyDescriptor_T
    {
        uint16_t propertyDescriptorLength;
        uint16_t propertyId;
        bool isWriteable : 1;
        bool isReadable : 1;
        bool isSubscribable : 1;
        bool isStatic : 1;
        uint16_t reservedForFlags : 12;
        uint8_t propertyType;
        uint8_t reserved;
        uint16_t propertyLength;
        std::string propertyName;
        uint16_t propertyUnits;
    };

    inline PropertyDescriptor_T CreatePropertyDescriptor(
        uint16_t id,
        bool writeable,
        bool readable,
        bool subscribable,
        bool isStatic,
        PropertyStorageVariantType_E type,
        uint16_t length,
        const std::string& name,
        uint16_t units = 0)
    {
        PropertyDescriptor_T descriptor;
        descriptor.propertyId = id;
        descriptor.isWriteable = writeable;
        descriptor.isReadable = readable;
        descriptor.isSubscribable = subscribable;
        descriptor.isStatic = isStatic;
        descriptor.reservedForFlags = 0;
        descriptor.propertyType = static_cast<uint8_t>(type);
        descriptor.reserved = 0;
        descriptor.propertyLength = length;
        descriptor.propertyName = name;
        descriptor.propertyUnits = units;
        descriptor.propertyDescriptorLength = sizeof(PropertyDescriptor_T) - sizeof(std::string) + name.length();
        return descriptor;
    }


    struct PropertyListRequest_T
    {
        Header_T header;
    };

    struct PropertyListReply_T
    {
        Header_T header;
        uint16_t numProperties;
        std::vector<PropertyDescriptor_T> propertyDescriptorList;
        uint16_t status;
    };

    struct GetValueReq_T
    {
        struct Header_T header;
        uint16_t maxRespLen;
        uint16_t numReqProp;
        std::vector<uint16_t> propIds;
    };

    struct GetValueReply_T
    {
        struct Header_T header;
        std::vector<PropertyStorageVariant> propValues;
        uint16_t status;
    };

    struct SetValueReq_T
    {
        struct Header_T header;
        uint16_t propId;
        PropertyStorageVariant value;
    };

    struct SetValueReply_T
    {
        struct Header_T header;
        PropertyStorageVariant value;
        uint16_t status;
    };

    struct PropertyValueContainer_T
    {
        PropertyId_T propertyId;
        PropertyStorageVariant value;
    };

    PropertyListRequest_T GetPropertyListRequest();
    PropertyListReply_T GetPropertyListReply(std::vector<PropertyDescriptor_T>& descList, PropertyGathererReplyStatus_E status);
    GetValueReq_T GetPropertyGetValueRequest(uint16_t maxResponseLength, std::vector<uint16_t> propertyIds);
    GetValueReply_T GetPropertyGetValueReply(std::vector<PropertyStorageVariant> values, PropertyGathererReplyStatus_E status);
    SetValueReq_T GetPropertySetValueRequest(uint16_t propertyId, PropertyStorageVariant& value);
    SetValueReply_T GetPropertySetValueReply(PropertyStorageVariant& value, PropertyGathererReplyStatus_E status);

    class PropertyGathererMessageSerializer
    {
        public:
            PropertyGathererMessageSerializer();
            ~PropertyGathererMessageSerializer();

            /* Serialization functions */
            uint16_t Serialize(PropertyListRequest_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(PropertyListReply_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(GetValueReq_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(GetValueReply_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(SetValueReq_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(SetValueReply_T& pMessage, std::vector<uint8_t>& outBuff);

            /* Deserialization functions */
            bool Deserialize(PropertyListRequest_T& pMessage, std::vector<uint8_t>& msgBuff);
            bool Deserialize(PropertyListReply_T& pMessage, std::vector<uint8_t>& msgBuff);
            bool Deserialize(GetValueReq_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(GetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(SetValueReq_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(SetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf);

        private:
            void SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff);
            void DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf);

            uint16_t SerializePropertyDescriptor(PropertyDescriptor_T& pDesc, std::vector<uint8_t>& outBuff);
            void DeserializePropertyDescriptor(PropertyDescriptor_T& pDesc, std::vector<uint8_t>& msgBuff, uint16_t& offset);

            uint16_t SerializeVariant(const PropertyStorageVariant& variant, std::vector<uint8_t>& outBuf);
            PropertyStorageVariant DeserializeVariant(std::vector<uint8_t>& inBuff, uint16_t& index);
    };
}

#endif // PROPERTY_GATHERER_MESSAGES_H