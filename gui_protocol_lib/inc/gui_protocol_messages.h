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

/* Shared include */
#include "custom_types.h"

namespace GuiProtocol
{
    /* Enum classes */
    enum class MessageID_E
    {
        WIDGET_LIST_REQ,
        WIDGET_LIST_REPLY,
        WIDGET_SET_VALUE_REQ,
        WIDGET_SET_VALUE_REPLY,
        WIDGET_EVENT_NOTIFICATION,
        WIDGET_EVENT_NOTIFICATION_ACK,
    };
    enum class WidgetDataTypes_E
    {
        WIDGET_DATA_TYPE_STRING, 
        WIDGET_DATA_TYPE_INT,
        WIDGET_DATA_TYPE_FLOAT,
        WIDGET_DATA_TYPE_BOOL,
        WIDGET_DATA_TYPE_UNKNOWN,
    };
    enum class WidgetReplyStatus_E
    {
        SET_VAL_SUCCESS,
        SET_VAL_PARTIAL_SUCCESS,
        SET_VAL_FAILED_TO_SET,
        SET_VAL_UNKNOWN_WIDGET,
        SET_VAL_ERROR,
    };

    /* Type defs */
    typedef std::vector<std::pair<uint32_t, WidgetValueVariant_T>> WidgetSetValueIdentifier_T; 

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

    /**
     * @brief Contains widget information such as ID, type, and interactability status
     */
    struct WidgetDescriptor_T
    {
        uint32_t widgetId;
        bool isInteractable : 1;
        bool isStatic : 1;
        bool isReadable : 1;
        bool isWritable : 1;
        uint8_t reserved : 4; // Reserved for future use
        uint8_t widgetType;
        uint8_t dataType; 
        std::string widgetName;
    };

    /**
     * @brief Message structure for requesting list of available widgets
     */
    struct WidgetListRequest_T
    {
        Header_T header;
    };
    /**
     * @brief Message structure containing reply to widget list request
     */
    struct WidgetListReply_T
    {
        Header_T header;
        uint16_t numWidgets;
        std::vector<WidgetDescriptor_T> widgetDescriptorList;
        uint16_t status;
    };

    /* 
        Set Value Request Structs 
    */

    /**
     * @brief Container for widget value update request data
     */
    struct WidgetSetValueRequestContainer_T
    {
        uint32_t widgetId;
        WidgetValueVariant_T value;
    };    
    /**
    * @brief Message structure for requesting widget value updates
    */
    struct WidgetSetValueRequest_T
    {
        Header_T header;
        uint16_t numWidgetSetValues;
        std::vector<WidgetSetValueRequestContainer_T> setValuesList;
    };

    /*
        Set Value Reply Structs 
    */

    /**
     * @brief Container for widget value update reply data
     */
    struct WidgetSetValueReplyContainer_T
    {
        uint32_t widgetId;
        WidgetValueVariant_T value;
        uint16_t status;
    };
    
    /**
     * @brief Message structure containing reply to widget value update request
     */
    struct WidgetSetValueReply_T
    {
        Header_T header;
        uint16_t numWidgetSetValues;
        std::vector<WidgetSetValueReplyContainer_T> setValuesList;
        uint16_t status;
    };
    /**
     * @brief Structure containing widget value update response information
     */
    struct WidgetSetValueResponseReturn_T
    {
        uint16_t windowId;
        uint16_t widgetId;
        uint8_t widgetType;
        uint8_t dataType; 
        WidgetValueVariant_T val;
        uint16_t status;
    };

    /**
     * @brief Structure for storing widget descriptor and its current value
     */
    struct WidgetValueStorage_T
    {
        WidgetDescriptor_T desc;
        WidgetValueVariant_T val;
    };

    /* Widget Event Notification */
    struct WidgetEventNotification_T
    {
        Header_T header;
        uint32_t widgetId;
        WidgetValueVariant_T updatedValue;
    };

    /* Widget Event Notification Ack */
    struct WidgetEventNotificationAck_T
    {
        Header_T header;
        uint32_t widgetId;
        uint16_t status;
    };

    /* Widget Descriptor Creator functions*/
    WidgetDescriptor_T GetTextWidgetDescriptor(uint16_t windowId, uint16_t widgetId, bool isReadable, bool isWritable, std::string& widgetName);
    WidgetDescriptor_T GetButtonWidgetDescriptor(uint16_t windowId, uint16_t widgetId, std::string& widgetName);
    WidgetDescriptor_T GetSliderWidgetDescriptor(uint16_t windowId, uint16_t widgetId, std::string& widgetName);

    /* Request Reply Creator functions */
    WidgetListRequest_T GetWidgetListRequest();
    WidgetListReply_T GetWidgetListReply(std::vector<WidgetDescriptor_T>& descList, WidgetReplyStatus_E status);
    WidgetSetValueRequest_T GetWidgetSetValueRequest(std::vector<WidgetValueStorage_T>& widgetSetVals);
    WidgetSetValueReply_T GetWidgetSetValueReply(std::vector<WidgetSetValueResponseReturn_T>& widgetSetVals, WidgetReplyStatus_E status);
    WidgetEventNotification_T GetWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T updatedValue);
    WidgetEventNotificationAck_T GetWidgetEventNotificationAck(uint32_t widgetId, uint16_t status);

    class GuiProtocolMessageSerializer
    {
        public:
            GuiProtocolMessageSerializer();
            ~GuiProtocolMessageSerializer();

            /* Serialization functions */
            uint16_t Serialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetSetValueRequest_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetSetValueReply_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetEventNotification_T& pMessage, std::vector<uint8_t>& outBuff);
            uint16_t Serialize(WidgetEventNotificationAck_T& pMessage, std::vector<uint8_t>& outBuff);

            /* Deserialization functions */
            bool Deserialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetSetValueRequest_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetSetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetEventNotification_T& pMessage, std::vector<uint8_t>& msgBuf);
            bool Deserialize(WidgetEventNotificationAck_T& pMessage, std::vector<uint8_t>& msgBuf);

        private:
            void SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff);
            void DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf);

            uint16_t SerializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& outBuff);
            void DeserializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& msgBuf, uint16_t& offset);

            uint16_t SerializeWidgetSetValueRequestContainer(WidgetSetValueRequestContainer_T& pCont, std::vector<uint8_t>& outBuff);
            void DeserializeWidgetSetValueRequestContainer(WidgetSetValueRequestContainer_T& pCont, std::vector<uint8_t>& msgBuf, uint16_t& offset);
            
            uint16_t SerializeWidgetSetValueReplyContainer(WidgetSetValueReplyContainer_T& pCont, std::vector<uint8_t>& outBuff);
            void DeserializeWidgetSetValueReplyContainer(WidgetSetValueReplyContainer_T& pCont, std::vector<uint8_t>& msgBuf, uint16_t& offset);

            uint16_t SerializeVariant(const WidgetValueVariant_T& variant, std::vector<uint8_t>& outBuf);
            WidgetValueVariant_T DeserializeVariant(std::vector<uint8_t>& inBuff, uint16_t& index);
    };
}

#endif // GUI_PROTOCOL_MESSAGES_H