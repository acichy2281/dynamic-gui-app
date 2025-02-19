#include "gui_protocol_messages.h"

namespace GuiProtocol
{
    WidgetListRequest_T GetWidgetListRequest()
    {
        WidgetListRequest_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_LIST_REQ);
        return retVal;
    }

    WidgetListReply_T GetWidgetListReply(std::vector<WidgetDescriptor_T>& descList)
    {
        WidgetListReply_T retVal;
        retVal.header.messageId = static_cast<uint16_t>(MessageID_E::WIDGET_LIST_REPLY);
        retVal.numWidgets = descList.size();
        retVal.widgetDescriptorList = descList;
        retVal.status = static_cast<uint16_t>(WidgetReplyStatus_E::SUCCESS);
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
        (pDesc.reserved & 0x3F);
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

        /* Deserialize variants */
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

    void GuiProtocolMessageSerializer::DeserializeWidgetDescriptor(WidgetDescriptor_T& pDesc, std::vector<uint8_t>& msgBuf, uint16_t& offset)
    {
        /* Deserialize Widget ID (4 bytes) */
        std::memcpy(&pDesc.widgetId, msgBuf.data() + offset, sizeof(pDesc.widgetId));
        offset += sizeof(pDesc.widgetId);

        /* Deserialize Flags + Widget Type (2 bytes total) */
        uint8_t packedFlags = msgBuf[offset];
        pDesc.isInteractable = (packedFlags >> 7) & 0x01;
        pDesc.isStatic = (packedFlags >> 6) & 0x01;
        pDesc.reserved = packedFlags & 0x3F; // Extract reserved (6 bits)
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
}