#ifndef GUI_PROTOCOL_MESSAGES_H
#define GUI_PROTOCOL_MESSAGES_H

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

/* Project include */
#include "custom_types.h"

namespace GuiProtocol
{    
    using WidgetValueVariant_T = std::variant<std::string>;

    /* Enum classes */
    enum class MessageID_E
    {
        WIDGET_LIST_REQ,
        WIDGET_LIST_REPLY,
    };
    enum class WidgetDataTypes_E
    {
        STRING, 
        INT,
        FLOAT,
    };
    enum class WidgetReplyStatus_E
    {
        SUCCESS,
    };

    /* Structs */

    /**
     * @brief Generic Header for all message types
     */
    struct Header_T
    {
        uint8_t protocolIdHigh = GUI_PROTOCOL_ID_HIGH;
        uint8_t protocolIdLow = GUI_PROTOCOL_ID_LOW;
        uint16_t messageId;
    };

    struct WidgetDescriptor_T
    {
        uint32_t widgetId;
        bool isInteractable : 1;
        bool isStatic : 1;
        uint8_t reserved : 6;
        uint8_t widgetType;
        uint8_t dataType; 
        std::string widgetName;
    };

    struct WidgetListRequest_T
    {
        Header_T header;
    };

    struct WidgetListReply_T
    {
        Header_T header;
        uint16_t numWidgets;
        std::vector<WidgetDescriptor_T> widgetDescriptorList;
        uint16_t status;
    };

    struct Widget_T
    {
        WidgetDescriptor_T desc;
        WidgetValueVariant_T val;
    };

    WidgetListRequest_T GetWidgetListRequest();
    WidgetListReply_T GetWidgetListReply(std::vector<WidgetDescriptor_T>& descList);

    class GuiProtocolMessageSerializer
    {
        public:
            GuiProtocolMessageSerializer();
            ~GuiProtocolMessageSerializer();

            /* Serialization functions */
            uint16_t Serialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& outBuff);

            /* Deserialization functions */
            bool Deserialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& msgBuf);

        private:
            void SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff);
            void DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf);
            uint16_t SerializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& outBuff);
            void DeserializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& msgBuf, uint16_t& offset);
    };
}

#endif // GUI_PROTOCOL_MESSAGES_H