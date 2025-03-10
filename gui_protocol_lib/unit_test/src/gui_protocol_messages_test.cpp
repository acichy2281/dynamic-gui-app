#include "gui_protocol_messages_test.h"

namespace GuiProtocolTest
{
    GuiProtocolMessageSerializationTest_C::GuiProtocolMessageSerializationTest_C()
    {

    }

    GuiProtocolMessageSerializationTest_C::~GuiProtocolMessageSerializationTest_C()
    {

    }
    
    bool GuiProtocolMessageSerializationTest_C::RunTest()
    {
        bool retVal = true;
        if (false == TestSerializeWidgetListRequest())
        {
            retVal = false;
        }
        if (false == TestSerializeWidgetListReply())
        {
            retVal = false;
        }
        if (false == TestSerializeWidgetSetValueRequest())
        {
            retVal = false;
        }
        if (false == TestSerializeWidgetSetValueReply())
        {
            retVal = false;
        }
        if (false == TestDeserializeWidgetListRequest())
        {
            retVal = false;
        }
        if (false == TestDeserializeWidgetListReply())
        {
            retVal = false;
        }
        if (false == TestDeserializeWidgetSetValueRequest())
        {
            retVal = false;
        }
        if (false == TestDeserializeWidgetSetValueReply())
        {
            retVal = false;
        }
        return retVal;
    }

    bool GuiProtocolMessageSerializationTest_C::TestSerializeWidgetListRequest()
    {
        GuiProtocol::WidgetListRequest_T req = {{0x01, 0x02, 0x1234}};
        std::vector<uint8_t> outBuff;
        
        uint16_t size = _msgSerializer.Serialize(req, outBuff);

        std::vector<uint8_t> expected = {0x01, 0x02, 0x34, 0x12};
        bool match = true;
        if (outBuff.size() != expected.size()) 
        {
            std::cout << "SerializeWidgetListRequest failed: Size mismatch. Expected " 
                      << expected.size() << " bytes but got " << outBuff.size() << " bytes.\n";
            match = false;
        } 
        else 
        {
            for (size_t i = 0; i < expected.size(); i++) 
            {
                if (outBuff[i] != expected[i]) 
                {
                    std::cout << "SerializeWidgetListRequest failed at byte " << i 
                            << ": Expected 0x" << std::hex << (int)expected[i] 
                            << " but got 0x" << (int)outBuff[i] << std::dec << "\n";
                    match = false;
                }
            }
        }
        if (match) 
        {
            std::cout << "SerializeWidgetListRequest passed.\n";
        }
        return match;
    }

    bool GuiProtocolMessageSerializationTest_C::TestSerializeWidgetListReply()
    {
        bool isInteractable = true;
        bool isStatic = false;
        uint8_t reserved = 0;
        uint8_t widgetType = 0x01;
        uint8_t dataType = 0x02;
        std::string widgetName = "TestWidget";
        GuiProtocol::WidgetDescriptor_T widgetDesc = {0x12345678, isInteractable, isStatic, reserved, widgetType, dataType, widgetName};
        std::vector<GuiProtocol::WidgetDescriptor_T> widgetList = {widgetDesc};
        GuiProtocol::WidgetListReply_T reply = {{0x01, 0x02, 0x5678}, static_cast<uint16_t>(widgetList.size()), widgetList, 0x0001};
        std::vector<uint8_t> outBuff;

        uint16_t size = _msgSerializer.Serialize(reply, outBuff);

        uint8_t widgetFlags = (isInteractable ? 0x80 : 0x00) | (isStatic ? 0x40 : 0x00) | (reserved & 0x3F);
        std::vector<uint8_t> expected = {0x01, 0x02, 0x78, 0x56, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, widgetFlags, 0x01, 0x02, 'T', 'e', 's', 't', 'W', 'i', 'd', 'g', 'e', 't', '\0', 0x01, 0x00};
        bool match = true;
        if (outBuff.size() != expected.size()) 
        {
            std::cout << "SerializeWidgetListReply failed: Size mismatch. Expected " 
                      << expected.size() << " bytes but got " << outBuff.size() << " bytes.\n";
            match = false;
        } 
        else 
        {
            for (size_t i = 0; i < expected.size(); i++) 
            {
                if (outBuff[i] != expected[i]) 
                {
                    std::cout << "SerializeWidgetListReply failed at byte " << i 
                            << ": Expected 0x" << std::hex << (int)expected[i] 
                            << " but got 0x" << (int)outBuff[i] << std::dec << "\n";
                    match = false;
                }
            }
        }
        if (match) 
        {
            std::cout << "SerializeWidgetListReply passed.\n";
        }
        return match;
    }

    bool GuiProtocolMessageSerializationTest_C::TestSerializeWidgetSetValueRequest()
    {
        GuiProtocol::WidgetValueVariant_T value = std::string("TestValue");
        GuiProtocol::WidgetSetValueRequestContainer_T setValueContainer = {0x12345678, value};
        std::vector<GuiProtocol::WidgetSetValueRequestContainer_T> setValueList = {setValueContainer};
        GuiProtocol::WidgetSetValueRequest_T request = {{0x01, 0x02, 0x9ABC}, static_cast<uint16_t>(setValueList.size()), setValueList};
        std::vector<uint8_t> outBuff;

        uint16_t size = _msgSerializer.Serialize(request, outBuff);
        
        uint8_t widgetVariantIdentifier = static_cast<uint8_t>(GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING);
        std::vector<uint8_t> expected = {0x01, 0x02, 0xBC, 0x9A, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, widgetVariantIdentifier, 'T', 'e', 's', 't', 'V', 'a', 'l', 'u', 'e', '\0'};
        bool match = true;
        if (outBuff.size() != expected.size()) 
        {
            std::cout << "SerializeWidgetSetValueRequest failed: Size mismatch. Expected " 
                      << expected.size() << " bytes but got " << outBuff.size() << " bytes.\n";
            match = false;
        } 
        else 
        {
            for (size_t i = 0; i < expected.size(); i++) 
            {
                if (outBuff[i] != expected[i]) 
                {
                    std::cout << "SerializeWidgetSetValueRequest failed at byte " << i 
                            << ": Expected 0x" << std::hex << (int)expected[i] 
                            << " but got 0x" << (int)outBuff[i] << std::dec << "\n";
                    match = false;
                }
            }
        }
        if (match) 
        {
            std::cout << "SerializeWidgetSetValueRequest passed.\n";
        }
        return match;
    }

    bool GuiProtocolMessageSerializationTest_C::TestSerializeWidgetSetValueReply()
    {
        GuiProtocol::WidgetValueVariant_T value = std::string("TestValue");
        uint16_t status = 0x0001;
        GuiProtocol::WidgetSetValueReplyContainer_T setValueContainer = {0x12345678, value, status};
        std::vector<GuiProtocol::WidgetSetValueReplyContainer_T> setValueList = {setValueContainer};
        GuiProtocol::WidgetSetValueReply_T reply = {{0x01, 0x02, 0x9ABC}, static_cast<uint16_t>(setValueList.size()), setValueList, status};
        std::vector<uint8_t> outBuff;

        uint16_t size = _msgSerializer.Serialize(reply, outBuff);

        uint8_t widgetVariantIdentifier = static_cast<uint8_t>(GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING);
        std::vector<uint8_t> expected = {0x01, 0x02, 0xBC, 0x9A, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, widgetVariantIdentifier, 'T', 'e', 's', 't', 'V', 'a', 'l', 'u', 'e', '\0', 0x01, 0x00, 0x01, 0x00};
        bool match = true;
        if (outBuff.size() != expected.size()) 
        {
            std::cout << "SerializeWidgetSetValueReply failed: Size mismatch. Expected " 
                      << expected.size() << " bytes but got " << outBuff.size() << " bytes.\n";
            match = false;
        } 
        else 
        {
            for (size_t i = 0; i < expected.size(); i++) 
            {
                if (outBuff[i] != expected[i]) 
                {
                    std::cout << "SerializeWidgetSetValueReply failed at byte " << i 
                            << ": Expected 0x" << std::hex << (int)expected[i] 
                            << " but got 0x" << (int)outBuff[i] << std::dec << "\n";
                    match = false;
                }
            }
        }
        if (match) 
        {
            std::cout << "SerializeWidgetSetValueReply passed.\n";
        }
        return match;
    }
    
    bool GuiProtocolMessageSerializationTest_C::TestDeserializeWidgetListRequest()
    {
        std::vector<uint8_t> inBuff = {0x01, 0x02, 0x34, 0x12};
        GuiProtocol::WidgetListRequest_T message;
        _msgSerializer.Deserialize(message, inBuff);

        bool success = true;
        if (message.header.protocolIdHigh != 0x01)
        {
            std::cout << "DeserializeWidgetListRequest failed: protocolIdHigh mismatch. Expected 0x01, got 0x" 
                      << std::hex << (int)message.header.protocolIdHigh << std::dec << "\n";
            success = false;
        }
        if (message.header.protocolIdLow != 0x02)
        {
            std::cout << "DeserializeWidgetListRequest failed: protocolIdLow mismatch. Expected 0x02, got 0x" 
                      << std::hex << (int)message.header.protocolIdLow << std::dec << "\n";
            success = false;
        }
        if (message.header.messageId != 0x1234)
        {
            std::cout << "DeserializeWidgetListRequest failed: messageId mismatch. Expected 0x1234, got 0x" 
                      << std::hex << message.header.messageId << std::dec << "\n";
            success = false;
        }

        if (success)
        {
            std::cout << "DeserializeWidgetListRequest passed.\n";
        }
        return success;
    }

    bool GuiProtocolMessageSerializationTest_C::TestDeserializeWidgetListReply()
    {
        std::vector<uint8_t> inBuff = {0x01, 0x02, 0x34, 0x12, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, 0x81, 0x01, 0x02, 'T', 'e', 's', 't', 'W', 'i', 'd', 'g', 'e', 't', '\0', 0x01, 0x00};
        GuiProtocol::WidgetListReply_T message;
        _msgSerializer.Deserialize(message, inBuff);

        bool success = true;
        if (message.header.protocolIdHigh != 0x01)
        {
            std::cout << "DeserializeWidgetListReply failed: protocolIdHigh mismatch. Expected 0x01, got 0x" 
                  << std::hex << (int)message.header.protocolIdHigh << std::dec << "\n";
            success = false;
        }
        if (message.header.protocolIdLow != 0x02)
        {
            std::cout << "DeserializeWidgetListReply failed: protocolIdLow mismatch. Expected 0x02, got 0x"
                  << std::hex << (int)message.header.protocolIdLow << std::dec << "\n";
            success = false;
        }
        if (message.header.messageId != 0x1234)
        {
            std::cout << "DeserializeWidgetListReply failed: messageId mismatch. Expected 0x1234, got 0x"
                  << std::hex << message.header.messageId << std::dec << "\n";
            success = false;
        }
        if (message.numWidgets != 1)
        {
            std::cout << "DeserializeWidgetListReply failed: numWidgets mismatch. Expected 1, got "
                  << message.numWidgets << "\n";
            success = false;
        }
        if (message.widgetDescriptorList.size() != 1)
        {
            std::cout << "DeserializeWidgetListReply failed: widgetDescriptorList size mismatch. Expected 1, got "
                  << message.widgetDescriptorList.size() << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].isInteractable != true)
        {
            std::cout << "DeserializeWidgetListReply failed: isInteractable mismatch. Expected true, got "
              << std::boolalpha << message.widgetDescriptorList[0].isInteractable << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].isStatic != false) 
        {
            std::cout << "DeserializeWidgetListReply failed: isStatic mismatch. Expected false, got "
              << std::boolalpha << message.widgetDescriptorList[0].isStatic << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].reserved != 0x01)
        {
            std::cout << "DeserializeWidgetListReply failed: reserved mismatch. Expected 0x01, got 0x"
              << std::hex << (int)message.widgetDescriptorList[0].reserved << std::dec << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].widgetType != 0x01)
        {
            std::cout << "DeserializeWidgetListReply failed: widgetType mismatch. Expected 0x01, got 0x"
              << std::hex << (int)message.widgetDescriptorList[0].widgetType << std::dec << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].dataType != 0x02)
        {
            std::cout << "DeserializeWidgetListReply failed: dataType mismatch. Expected 0x02, got 0x"
              << std::hex << (int)message.widgetDescriptorList[0].dataType << std::dec << "\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].widgetName != "TestWidget")
        {
            std::cout << "DeserializeWidgetListReply failed: widgetName mismatch. Expected 'TestWidget', got '"
              << message.widgetDescriptorList[0].widgetName << "'\n";
            success = false;
        }
        if (message.widgetDescriptorList[0].widgetId != 0x12345678)
        {
            std::cout << "DeserializeWidgetListReply failed: widgetId mismatch. Expected 0x12345678, got 0x"
                  << std::hex << message.widgetDescriptorList[0].widgetId << std::dec << "\n";
            success = false;
        }
        if (message.status != 0x0001)
        {
            std::cout << "DeserializeWidgetListReply failed: status mismatch. Expected 0x0001, got 0x"
                  << std::hex << message.status << std::dec << "\n";
            success = false;
        }

        if (success)
        {
            std::cout << "DeserializeWidgetListReply passed.\n";
            return true;
        } 
        else 
        {
            std::cout << "DeserializeWidgetListReply failed.\n";
            return false;
        }
    }

    bool GuiProtocolMessageSerializationTest_C::TestDeserializeWidgetSetValueRequest()
    {
        uint8_t widgetVariantIdentifier = static_cast<uint8_t>(GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING);
        std::vector<uint8_t> inBuff = {0x01, 0x02, 0xBC, 0x9A, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, widgetVariantIdentifier, 'T', 'e', 's', 't', 'V', 'a', 'l', 'u', 'e', '\0'};
        GuiProtocol::WidgetSetValueRequest_T message;
        _msgSerializer.Deserialize(message, inBuff);

        bool success = true;
        if (message.header.protocolIdHigh != 0x01)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: protocolIdHigh mismatch. Expected 0x01, got 0x" 
                      << std::hex << (int)message.header.protocolIdHigh << std::dec << "\n";
            success = false;
        }
        if (message.header.protocolIdLow != 0x02)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: protocolIdLow mismatch. Expected 0x02, got 0x"
                      << std::hex << (int)message.header.protocolIdLow << std::dec << "\n";
            success = false;
        }
        if (message.header.messageId != 0x9ABC)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: messageId mismatch. Expected 0x9ABC, got 0x"
                      << std::hex << message.header.messageId << std::dec << "\n";
            success = false;
        }
        if (message.numWidgetSetValues != 0x0001)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: numWidgetSetValues mismatch. Expected 1, got "
                      << message.numWidgetSetValues << "\n";
            success = false;
        }
        if (message.setValuesList.size() != 1)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: setValuesList size mismatch. Expected 1, got "
                      << message.setValuesList.size() << "\n";
            success = false;
        }
        if (message.setValuesList[0].widgetId != 0x12345678)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: widgetId mismatch. Expected 0x12345678, got 0x"
                      << std::hex << message.setValuesList[0].widgetId << std::dec << "\n";
            success = false;
        }
        if (message.setValuesList[0].value.index() != GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING)
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: value type mismatch. Expected string type ("
                      << GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING << "), got "
                      << message.setValuesList[0].value.index() << "\n";
            success = false;
        }
        if (std::get<std::string>(message.setValuesList[0].value) != "TestValue")
        {
            std::cout << "DeserializeWidgetSetValueRequest failed: value mismatch. Expected 'TestValue', got '"
                      << std::get<std::string>(message.setValuesList[0].value) << "'\n";
            success = false;
        }

        if (success)
        {
            std::cout << "DeserializeWidgetSetValueRequest passed.\n";
            return true;
        }
        else 
        {
            std::cout << "DeserializeWidgetListReply failed.\n";
            return false;
        }
    }

    bool GuiProtocolMessageSerializationTest_C::TestDeserializeWidgetSetValueReply()
    {
        uint8_t widgetVariantIdentifier = static_cast<uint8_t>(GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING);
        std::vector<uint8_t> inBuff = {0x01, 0x02, 0xBC, 0x9A, 0x01, 0x00, 0x78, 0x56, 0x34, 0x12, widgetVariantIdentifier, 'T', 'e', 's', 't', 'V', 'a', 'l', 'u', 'e', '\0', 0x01, 0x00, 0x01, 0x00};
        GuiProtocol::WidgetSetValueReply_T message;
        _msgSerializer.Deserialize(message, inBuff);

        bool success = true;

        if (message.header.protocolIdHigh != 0x01)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: protocolIdHigh mismatch. Expected 0x01, got 0x" 
                      << std::hex << (int)message.header.protocolIdHigh << std::dec << "\n";
            success = false;
        }
        if (message.header.protocolIdLow != 0x02)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: protocolIdLow mismatch. Expected 0x02, got 0x"
                      << std::hex << (int)message.header.protocolIdLow << std::dec << "\n";
            success = false;
        }
        if (message.header.messageId != 0x9ABC)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: messageId mismatch. Expected 0x9ABC, got 0x"
                      << std::hex << message.header.messageId << std::dec << "\n";
            success = false;
        }
        if (message.numWidgetSetValues != 0x0001)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: numWidgetSetValues mismatch. Expected 1, got "
                      << message.numWidgetSetValues << "\n";
            success = false;
        }
        if (message.setValuesList.size() != 1)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: setValuesList size mismatch. Expected 1, got "
                      << message.setValuesList.size() << "\n";
            success = false;
        }
        if (message.setValuesList[0].widgetId != 0x12345678)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: widgetId mismatch. Expected 0x12345678, got 0x"
                      << std::hex << message.setValuesList[0].widgetId << std::dec << "\n";
            success = false;
        }
        if (message.setValuesList[0].value.index() != GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: value type mismatch. Expected string type ("
                      << GuiProtocol::WidgetValueVariantType_E::WIDGET_VARIANT_TYPE_STRING << "), got "
                      << message.setValuesList[0].value.index() << "\n";
            success = false;
        }
        if (std::get<std::string>(message.setValuesList[0].value) != "TestValue")
        {
            std::cout << "DeserializeWidgetSetValueReply failed: value mismatch. Expected 'TestValue', got '"
                      << std::get<std::string>(message.setValuesList[0].value) << "'\n";
            success = false;
        }
        if (message.setValuesList[0].status != 0x0001)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: container status mismatch. Expected 0x0001, got 0x"
                      << std::hex << message.setValuesList[0].status << std::dec << "\n";
            success = false;
        }
        if (message.status != 0x0001)
        {
            std::cout << "DeserializeWidgetSetValueReply failed: reply status mismatch. Expected 0x0001, got 0x"
                      << std::hex << message.status << std::dec << "\n";
            success = false;
        }

        if (success)
        {
            std::cout << "DeserializeWidgetSetValueReply passed.\n";
            return true;
        }
        else 
        {
            return false;
        }
    }

}