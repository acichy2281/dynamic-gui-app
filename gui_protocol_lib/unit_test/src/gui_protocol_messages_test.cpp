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
        return retVal;
    }

    bool GuiProtocolMessageSerializationTest_C::TestSerializeWidgetListRequest()
    {
        GuiProtocol::WidgetListRequest_T req = {{0x01, 0x02, 0x1234}};
        std::vector<uint8_t> outBuff;
        
        uint16_t size = _msgSerializer.Serialize(req, outBuff);

        std::vector<uint8_t> expected = {0x01, 0x02, 0x34, 0x12};
        if (outBuff == expected) 
        {
            std::cout << "SerializeWidgetListRequest passed.\n";
            return true;
        } 
        else 
        {
            std::cout << "SerializeWidgetListRequest failed.\n";
            return false;
        }
    }
}