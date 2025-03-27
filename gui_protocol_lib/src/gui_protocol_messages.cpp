#include "gui_protocol_messages.h"

namespace GuiProtocol
{
    WidgetDescriptor_T GetWidgetDescriptor(uint16_t windowId, 
                                           uint16_t widgetId,
                                           bool isReadable,
                                           bool isWritable,
                                           bool isInteractable,
                                           bool isStatic,
                                           WidgetTypes_E widgetType,
                                           WidgetDataTypes_E dataType,
                                           std::string& widgetName)
    {
        WidgetDescriptor_T retVal;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.isInteractable = isInteractable;
        retVal.isStatic = isStatic;
        retVal.isReadable = isReadable;
        retVal.isWritable = isWritable;
        retVal.reserved = 0;
        retVal.widgetType = static_cast<uint8_t>(widgetType);
        retVal.dataType = static_cast<uint8_t>(dataType);
        retVal.widgetName = widgetName;

        return retVal;
    }

    WidgetDescriptor_T GetButtonWidgetDescriptor(uint16_t windowId, uint16_t widgetId, std::string& widgetName)
    {
        bool isInteractable = true;
        bool isStatic = false;
        bool isReadable = true;
        bool isWritable = false;
        uint8_t reserved = 0;
        uint8_t widgetType = static_cast<uint8_t>(WidgetTypes_E::BUTTON);
        uint8_t dataType = static_cast<uint8_t>(WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL); // Assuming button state is boolean

        WidgetDescriptor_T retVal;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.isInteractable = isInteractable;
        retVal.isStatic = isStatic;
        retVal.isReadable = isReadable;
        retVal.isWritable = isWritable;
        retVal.reserved = reserved;
        retVal.widgetType = widgetType;
        retVal.dataType = dataType;
        retVal.widgetName = widgetName;

        return retVal;
    }

    WidgetDescriptor_T GetSliderWidgetDescriptor(uint16_t windowId, uint16_t widgetId, std::string& widgetName)
    {
        bool isInteractable = true;
        bool isStatic = false;
        bool isReadable = true;
        bool isWritable = true; // Assuming slider can be written to
        uint8_t reserved = 0;
        uint8_t widgetType = static_cast<uint8_t>(WidgetTypes_E::SLIDER);
        uint8_t dataType = static_cast<uint8_t>(WidgetDataTypes_E::WIDGET_DATA_TYPE_FLOAT); // Assuming slider value is float

        WidgetDescriptor_T retVal;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.isInteractable = isInteractable;
        retVal.isStatic = isStatic;
        retVal.isReadable = isReadable;
        retVal.isWritable = isWritable;
        retVal.reserved = reserved;
        retVal.widgetType = widgetType;
        retVal.dataType = dataType;
        retVal.widgetName = widgetName;

        return retVal;
    }

    WidgetListRequest_T GetWidgetListRequest()
    {
        WidgetListRequest_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_LIST_REQ);
        return retVal;
    }

    WidgetListReply_T GetWidgetListReply(std::vector<WidgetDescriptor_T>& descList, WidgetReplyStatus_E status)
    {
        WidgetListReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_LIST_REPLY);
        retVal.numWidgets = descList.size();
        retVal.widgetDescriptorList = descList;
        retVal.status = static_cast<uint16_t>(status);
        return retVal;
    }

    WidgetSetValueRequest_T GetWidgetSetValueRequest(std::vector<WidgetValueStorage_T>& widgetSetVals)
    {
        WidgetSetValueRequest_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_SET_VALUE_REQ);
        retVal.numWidgetSetValues = static_cast<uint16_t>(widgetSetVals.size());

        for (auto& widgetSetVal : widgetSetVals)
        {
            WidgetSetValueRequestContainer_T setValCont;
            setValCont.widgetId = widgetSetVal.desc.widgetId;
            setValCont.value = widgetSetVal.val;
            retVal.setValuesList.push_back(setValCont);
        }
        return retVal;
    }
    
    WidgetSetValueReply_T GetWidgetSetValueReply(std::vector<WidgetSetValueResponseReturn_T>& widgetSetVals, WidgetReplyStatus_E status)
    {
        WidgetSetValueReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_SET_VALUE_REPLY);
        retVal.numWidgetSetValues = static_cast<uint16_t>(widgetSetVals.size());

        for (auto& widgetSetVal : widgetSetVals)
        {
            WidgetSetValueReplyContainer_T setValCont;
            setValCont.widgetId = static_cast<uint32_t>(widgetSetVal.windowId << 16) | static_cast<uint32_t>(widgetSetVal.widgetId);
            setValCont.value = widgetSetVal.val;
            setValCont.status = widgetSetVal.status;
            retVal.setValuesList.push_back(setValCont);
        }
        retVal.status = static_cast<uint16_t>(status);
        return retVal;
    }

    WidgetEventNotification_T GetWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T updatedValue)
    {
        WidgetEventNotification_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_EVENT_NOTIFICATION);
        retVal.widgetId = widgetId;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.updatedValue = updatedValue;
        return retVal;
    }

    WidgetEventNotificationAck_T GetWidgetEventNotificationAck(uint32_t widgetId, uint16_t status)
    {
        WidgetEventNotificationAck_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_EVENT_NOTIFICATION_ACK);
        retVal.widgetId = widgetId;
        retVal.status = status;
        return retVal;
    }

    GuiProtocolMessageSerializer::GuiProtocolMessageSerializer()
    {

    }

    GuiProtocolMessageSerializer::~GuiProtocolMessageSerializer()
    {

    }

    void GuiProtocolMessageSerializer::SerializeHeader(Header_T& header, std::vector<uint8_t>& outBuff)
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

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();
        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Number of Widgets */
        outBuff.resize(bufSize + sizeof(pMessage.numWidgets));
        std::memcpy(outBuff.data() + bufSize, &pMessage.numWidgets, sizeof(pMessage.numWidgets));
        bufSize = outBuff.size();

        /* Serialize List of Widget Descriptors */
        for (auto& widgetDesc : pMessage.widgetDescriptorList)
        {
            bufSize += SerializeWidgetDescriptor(widgetDesc, outBuff);
        }

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::SerializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& outBuff)
    {
        uint16_t bufSize = outBuff.size();

        /* Serialize Widget Id */
        outBuff.resize(bufSize + sizeof(pDesc.widgetId));
        std::memcpy(outBuff.data() + bufSize, &pDesc.widgetId, sizeof(pDesc.widgetId));
        bufSize = outBuff.size();

        /* Serialize Flags + Widget Type (2 bytes total) */
        uint8_t packedFlags =
        (static_cast<uint16_t>(pDesc.isInteractable) << 7) |
        (static_cast<uint16_t>(pDesc.isStatic) << 6) |
        (static_cast<uint16_t>(pDesc.isReadable) << 5) |
        (static_cast<uint16_t>(pDesc.isWritable) << 4) |
        (pDesc.reserved & 0xF); // Keep only the lower 4 bits of reserved
        outBuff.push_back(packedFlags);

        /* Serialize Widget Type */
        outBuff.push_back(pDesc.widgetType);

        /* Serialize Data Type */
        outBuff.push_back(pDesc.dataType);
        bufSize = outBuff.size();

        /* Serialize Widget Name */
        outBuff.insert(outBuff.end(), pDesc.widgetName.begin(), pDesc.widgetName.end());
        outBuff.push_back('\0');  // Ensure null termination
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetSetValueRequest_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Number of Widgets */
        outBuff.resize(bufSize + sizeof(pMessage.numWidgetSetValues));
        std::memcpy(outBuff.data() + bufSize, &pMessage.numWidgetSetValues, sizeof(pMessage.numWidgetSetValues));
        bufSize = outBuff.size();

        for (auto& setRequst : pMessage.setValuesList)
        {
            bufSize += SerializeWidgetSetValueRequestContainer(setRequst, outBuff);
        }
        
        return bufSize;
    }
    
    uint16_t GuiProtocolMessageSerializer::SerializeWidgetSetValueRequestContainer(WidgetSetValueRequestContainer_T& pCont, std::vector<uint8_t>& outBuff)
    {
        uint16_t bufSize = outBuff.size();

        /* Serialize Widget Id */
        outBuff.resize(bufSize + sizeof(pCont.widgetId));
        std::memcpy(outBuff.data() + bufSize, &pCont.widgetId, sizeof(pCont.widgetId));
        bufSize = outBuff.size();

        SerializeVariant(pCont.value, outBuff);
        
        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetSetValueReply_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Number of Widgets */
        outBuff.resize(bufSize + sizeof(pMessage.numWidgetSetValues));
        std::memcpy(outBuff.data() + bufSize, &pMessage.numWidgetSetValues, sizeof(pMessage.numWidgetSetValues));
        bufSize = outBuff.size();

        for (auto& setReply : pMessage.setValuesList)
        {
            bufSize += SerializeWidgetSetValueReplyContainer(setReply, outBuff);
        }

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size();

        return bufSize;
    }
    
    uint16_t GuiProtocolMessageSerializer::SerializeWidgetSetValueReplyContainer(WidgetSetValueReplyContainer_T& pCont, std::vector<uint8_t>& outBuff)
    {
        uint16_t bufSize = outBuff.size();

        /* Serialize Widget Id */
        outBuff.resize(bufSize + sizeof(pCont.widgetId));
        std::memcpy(outBuff.data() + bufSize, &pCont.widgetId, sizeof(pCont.widgetId));
        bufSize = outBuff.size();

        SerializeVariant(pCont.value, outBuff);

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pCont.status));
        std::memcpy(outBuff.data() + bufSize, &pCont.status, sizeof(pCont.status));
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetEventNotification_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Widget Id */
        outBuff.resize(bufSize + sizeof(pMessage.widgetId));
        std::memcpy(outBuff.data() + bufSize, &pMessage.widgetId, sizeof(pMessage.widgetId));
        bufSize = outBuff.size();

        SerializeVariant(pMessage.updatedValue, outBuff);

        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::Serialize(WidgetEventNotificationAck_T& pMessage, std::vector<uint8_t>& outBuff)
    {
        SerializeHeader(pMessage.header, outBuff);
        uint16_t bufSize = outBuff.size();

        /* Serialize Widget Id */
        outBuff.resize(bufSize + sizeof(pMessage.widgetId));
        std::memcpy(outBuff.data() + bufSize, &pMessage.widgetId, sizeof(pMessage.widgetId));
        bufSize = outBuff.size();

        /* Serialize status */
        bufSize = outBuff.size();
        outBuff.resize(bufSize + sizeof(pMessage.status));
        std::memcpy(outBuff.data() + bufSize, &pMessage.status, sizeof(pMessage.status));
        bufSize = outBuff.size();

        return bufSize;
    }

    uint16_t GuiProtocolMessageSerializer::SerializeVariant(const WidgetValueVariant_T& variant, std::vector<uint8_t>& outBuf)
    {
        // Serialize the type index (or type identifier)
        uint8_t typeIndex = static_cast<uint8_t>(variant.index());
        outBuf.push_back(typeIndex);  // Store the type index at the beginning of the serialized data

        return std::visit([&outBuf](const auto& value) -> uint16_t
        {
            using T = std::decay_t<decltype(value)>;

            if constexpr (std::is_same_v<T, std::string>)
            {
                size_t oldSize = outBuf.size();
                size_t strSize = value.size() + 1; // +1 for null terminator

                outBuf.resize(oldSize + strSize);
                std::memcpy(outBuf.data() + oldSize, value.c_str(), strSize); // Include null terminator

                return static_cast<uint16_t>(strSize);
            }
            else
            {
                auto bufSize = outBuf.size();
                outBuf.resize(bufSize + sizeof(T));
                std::memcpy(outBuf.data() + bufSize, &value, sizeof(T));
                return sizeof(T);
            }
        }, variant);
    }

    void GuiProtocolMessageSerializer::DeserializeHeader(Header_T& header, std::vector<uint8_t>& msgBuf)
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

    bool GuiProtocolMessageSerializer::Deserialize(WidgetListRequest_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        return true;
    }

    bool GuiProtocolMessageSerializer::Deserialize(WidgetListReply_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize number of widgets */
        std::memcpy(&pMessage.numWidgets, msgBuf.data() + bufIndex, sizeof(pMessage.numWidgets));
        bufIndex += sizeof(pMessage.numWidgets);

        /* Deserialize widget descriptors */
        for (int i = 0; i < pMessage.numWidgets; i++)
        {
            WidgetDescriptor_T widget;
            DeserializeWidgetDescriptor(widget, msgBuf, bufIndex);
            pMessage.widgetDescriptorList.push_back(widget);
        }

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }

    bool GuiProtocolMessageSerializer::Deserialize(WidgetSetValueRequest_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize number of widgets */
        std::memcpy(&pMessage.numWidgetSetValues, msgBuf.data() + bufIndex, sizeof(pMessage.numWidgetSetValues));
        bufIndex += sizeof(pMessage.numWidgetSetValues);

        for (int i = 0; i < pMessage.numWidgetSetValues; i++)
        {
            WidgetSetValueRequestContainer_T pCont;
            DeserializeWidgetSetValueRequestContainer(pCont, msgBuf, bufIndex);
            pMessage.setValuesList.push_back(pCont);
        }

        return true;
    }

    bool GuiProtocolMessageSerializer::Deserialize(WidgetSetValueReply_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize number of widgets */
        std::memcpy(&pMessage.numWidgetSetValues, msgBuf.data() + bufIndex, sizeof(pMessage.numWidgetSetValues));
        bufIndex += sizeof(pMessage.numWidgetSetValues);

        for (int i = 0; i < pMessage.numWidgetSetValues; i++)
        {
            WidgetSetValueReplyContainer_T pCont;
            DeserializeWidgetSetValueReplyContainer(pCont, msgBuf, bufIndex);
            pMessage.setValuesList.push_back(pCont);
        }

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }

    void GuiProtocolMessageSerializer::DeserializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& msgBuf, uint16_t& offset)
    {
        /* Deserialize Widget ID (4 bytes) */
        std::memcpy(&pDesc.widgetId, msgBuf.data() + offset, sizeof(pDesc.widgetId));
        offset += sizeof(pDesc.widgetId);

        /* Deserialize Flags + Widget Type (2 bytes total) */
        uint8_t packedFlags = msgBuf[offset];
        pDesc.isInteractable = (packedFlags >> 7) & 0x01;
        pDesc.isStatic = (packedFlags >> 6) & 0x01;
        pDesc.isReadable = (packedFlags >> 5) & 0x01;
        pDesc.isWritable = (packedFlags >> 4) & 0x01;
        pDesc.reserved = packedFlags & 0xF; // Extract reserved (4 bits)
        pDesc.widgetType = msgBuf[offset + 1];
        offset += 2;

        /* Deserialize Data Type (1 byte) */
        pDesc.dataType = msgBuf[offset];
        offset += 1;

        /* Deserialize Widget Name */
        size_t startIdx = offset;
        while (offset < msgBuf.size() && msgBuf[offset] != '\0')
        {
            ++offset;
        }

        if (offset < msgBuf.size()) // If null terminator found
        {
            pDesc.widgetName = std::string(reinterpret_cast<const char*>(&msgBuf[startIdx]), offset - startIdx);
            ++offset; // Move past the null terminator
        }
        else
        {
            pDesc.widgetName.clear(); // Invalid string (no null terminator found)
        }
    }

    void GuiProtocolMessageSerializer::DeserializeWidgetSetValueRequestContainer(WidgetSetValueRequestContainer_T& pCont, std::vector<uint8_t>& msgBuf, uint16_t& offset)
    {
        /* Deserialize widget ID */
        std::memcpy(&pCont.widgetId, msgBuf.data() + offset, sizeof(pCont.widgetId));
        offset += sizeof(pCont.widgetId);

        pCont.value = DeserializeVariant(msgBuf, offset);
    }
    
    void GuiProtocolMessageSerializer::DeserializeWidgetSetValueReplyContainer(WidgetSetValueReplyContainer_T& pCont, std::vector<uint8_t>& msgBuf, uint16_t& offset)
    {
        /* Deserialize widget ID */
        std::memcpy(&pCont.widgetId, msgBuf.data() + offset, sizeof(pCont.widgetId));
        offset += sizeof(pCont.widgetId);

        pCont.value = DeserializeVariant(msgBuf, offset);

        /* Deserialize status */
        std::memcpy(&pCont.status, msgBuf.data() + offset, sizeof(pCont.status));
        offset += sizeof(pCont.status);
    }

    bool GuiProtocolMessageSerializer::Deserialize(WidgetEventNotification_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize Widget ID */
        std::memcpy(&pMessage.widgetId, msgBuf.data() + bufIndex, sizeof(pMessage.widgetId));
        bufIndex += sizeof(pMessage.widgetId);

        pMessage.updatedValue = DeserializeVariant(msgBuf, bufIndex);

        return true;
    }

    bool GuiProtocolMessageSerializer::Deserialize(WidgetEventNotificationAck_T& pMessage, std::vector<uint8_t>& msgBuf)
    {
        DeserializeHeader(pMessage.header, msgBuf);
        uint16_t bufIndex = sizeof(pMessage.header);

        /* Deserialize Widget ID */
        std::memcpy(&pMessage.widgetId, msgBuf.data() + bufIndex, sizeof(pMessage.widgetId));
        bufIndex += sizeof(pMessage.widgetId);

        /* Deserialize status */
        std::memcpy(&pMessage.status, msgBuf.data() + bufIndex, sizeof(pMessage.status));
        bufIndex += sizeof(pMessage.status);

        return true;
    }
    
    WidgetValueVariant_T GuiProtocolMessageSerializer::DeserializeVariant(std::vector<uint8_t>& inBuff, uint16_t& index)
    {
        // Read the type index (which type was serialized)
        uint8_t typeIndex = inBuff[index++];
        WidgetValueVariant_T result;

        // Based on the type index, read the appropriate data from the buffer
        switch (static_cast<WidgetValueVariantType_E>(typeIndex)) {
            // case 0: { // int8_t
            //     int8_t val = static_cast<int8_t>(inBuff[index++]);
            //     result = val;
            //     break;
            // }
            // case 1: { // int16_t
            //     int16_t val = static_cast<int16_t>(inBuff[index++] | (inBuff[index++] << 8));
            //     result = val;
            //     break;
            // }
            // case 2: { // int32_t
            //     int32_t val = static_cast<int32_t>(inBuff[index++] | (inBuff[index++] << 8) |
            //                                     (inBuff[index++] << 16) | (inBuff[index++] << 24));
            //     result = val;
            //     break;
            // }
            // case 3: { // int64_t
            //     int64_t val = static_cast<int64_t>(inBuff[index++] | (inBuff[index++] << 8) |
            //                                     (inBuff[index++] << 16) | (inBuff[index++] << 24) |
            //                                     (inBuff[index++] << 32) | (inBuff[index++] << 40) |
            //                                     (inBuff[index++] << 48) | (inBuff[index++] << 56));
            //     result = val;
            //     break;
            // }
            // case UNSIGNED_8_BIT_INT: { // uint8_t
            //     uint8_t val = inBuff[index++];
            //     result = val;
            //     break;
            // }
            // case 5: { // uint16_t
            //     uint16_t val = static_cast<uint16_t>(inBuff[index++] | (inBuff[index++] << 8));
            //     result = val;
            //     break;
            // }
            // case 6: { // uint32_t
            //     uint32_t val = static_cast<uint32_t>(inBuff[index++] | (inBuff[index++] << 8) |
            //                                         (inBuff[index++] << 16) | (inBuff[index++] << 24));
            //     result = val;
            //     break;
            // }
            // case 7: { // uint64_t
            //     uint64_t val = static_cast<uint64_t>(inBuff[index++] | (inBuff[index++] << 8) |
            //                                         (inBuff[index++] << 16) | (inBuff[index++] << 24) |
            //                                         (inBuff[index++] << 32) | (inBuff[index++] << 40) |
            //                                         (inBuff[index++] << 48) | (inBuff[index++] << 56));
            //     result = val;
            //     break;
            // }

            case WIDGET_VARIANT_TYPE_STRING: { // std::string
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
            case WIDGET_VARIANT_TYPE_INT: { // int32_t
                int32_t val;
                std::memcpy(&val, &inBuff[index], sizeof(val));
                index += sizeof(val);
                result = val;
                break;
            }
            case WIDGET_VARIANT_TYPE_BOOL: { // bool
                bool val = (inBuff[index++] != 0);
                result = val;
                break;
            }
            case WIDGET_VARIANT_TYPE_FLOAT: { // float
                float val;
                std::memcpy(&val, &inBuff[index], sizeof(val));
                index += sizeof(val);
                result = val;
                break;
            }
            default:
                std::cout << "Unknown type index during deserialization.\n";
                throw std::runtime_error("Unknown type index during deserialization.");
        }

        return result;
    }
}