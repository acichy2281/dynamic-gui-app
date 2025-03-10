#ifndef GUI_PROTOCOL_MESSAGES_TEST_H
#define GUI_PROTOCOL_MESSAGES_TEST_H

/* System includes */
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <mutex>
#include <queue>
#include <memory>
#include <fstream>
#include <thread>
#include <unordered_map>
#include <regex>
#include <limits>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <future>
#include <map>
#include <inttypes.h>

/* Project includes */
#include "gui_protocol_messages.h"

namespace GuiProtocolTest
{
    class GuiProtocolMessageSerializationTest_C 
    {
        public:
            GuiProtocolMessageSerializationTest_C();
            ~GuiProtocolMessageSerializationTest_C();
            bool RunTest();

        private:
            bool TestSerializeWidgetListRequest();
            bool TestSerializeWidgetListReply();
            bool TestSerializeWidgetSetValueRequest();
            bool TestSerializeWidgetSetValueReply();
            bool TestDeserializeWidgetListRequest();
            bool TestDeserializeWidgetListReply();
            bool TestDeserializeWidgetSetValueRequest();
            bool TestDeserializeWidgetSetValueReply();
            GuiProtocol::GuiProtocolMessageSerializer _msgSerializer;

    };
}
#endif // GUI_PROTOCOL_MESSAGES_TEST_H