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

namespace PropertyGatherer
{
    enum PropertyStorageVariantType_E 
    {
        UNSIGNED_8_BIT_INT,
        STRING,
    };

    using PropertyStorageVariant = std::variant</*int8_t,
                                                int16_t,
                                                int32_t,
                                                int64_t,*/
                                                uint8_t,
                                                /*uint16_t,
                                                uint32_t,
                                                uint64_t,
                                                float,*/
                                                std::string>;

    enum class MessageID_E
    {
        PROTOCOL_VERSION_REQ,
        PROTOCOL_VERSION_REPLY,
        PROPERTY_LIST_REQ,
        PROPERTY_LIST_REPLY,
        GET_VALUE_REQUEST,
        GET_VALUE_REPLY,
        SET_VALUE_REQUEST,
        SET_VALUE_REPLY,
    };

    /* Structs */

    /**
     * @brief Generic Header for all message types
     */
    struct Header_T
    {
        uint8_t protocolIdHigh;
        uint8_t protocolIdLow;
        uint16_t messageId;
    };

    struct PropertyDescriptor_T
    {
        // Property Descriptor Length
        uint16_t propertyId;
        // Property Flags
        PropertyStorageVariantType_E propertyType;
        // PropertyLength
        std::string propertyName;
        // Property Units
    };

    struct PropertyListRequest_T
    {
        Header_T header;
    };

    struct PropertyListReply_T
    {
        Header_T header;
        uint16_t numProperties;
        std::vector<PropertyDescriptor_T>
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

    PropertyListRequest_T GetPropertyListRequest();
    PropertyListReply_T GetPropertyListReply();
    GetValueReq_T GetValueRequest();
    GetValueReply_T GetValueReply();

    class PropertyGathererMessageSerializer
    {
        public:
            PropertyGathererMessageSerializer();
            ~PropertyGathererMessageSerializer();

            /* Serialization functions */
            uint16_t Serialize(PropertyListRequest_T pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(PropertyListReply_T pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(GetValueReq_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(GetValueReply_T& pMessage, std::vector<uint8_t>& outBuff);

            /* Deserialization functions */
            bool Deserialize(PropertyListRequest_T& pMessage, std::vector<uint8_t>& msgBuff);
            bool Deserialize(PropertyListReply_T& pMessage, std::vector<uint8_t>& msgBuff);
            bool Deserialize(GetValueReq_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(GetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf);

        private:
            void SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff);
            void DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf);

            uint16_t SerializeVariant(const PropertyStorageVariant& variant, std::vector<uint8_t>& outBuf);
            PropertyStorageVariant DeserializeVariant(std::vector<uint8_t>& inBuff, uint16_t& index);
    };
}

#endif // PROPERTY_GATHERER_MESSAGES_H