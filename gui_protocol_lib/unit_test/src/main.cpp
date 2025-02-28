#include "gui_protocol_messages_test.h"

int main(int argc, char** argv)
{
    int retVal = 0;
    GuiProtocolTest::GuiProtocolMessageSerializationTest_C unitTester;

    if (false == unitTester.RunTest())
    {
        std::cout << "Gui Protocol Message Serialization Test failed\n";
        retVal = -1;
    }
    else 
    {
        std::cout << "Gui Protocol Message Serialization Test passed\n";
        retVal = 0;
    }
    return retVal;
}