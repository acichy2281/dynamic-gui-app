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
}