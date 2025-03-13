#ifndef GUI_CLIENT_API_H
#define GUI_CLIENT_API_H

/* System Includes */
#include <memory>
#include <inttypes.h>
#include <unordered_map>
#include <functional>

/* Shared includes */
#include "thread_safe_queue.h"

/* Project Includes */
#include "gui_protocol_messages.h"

namespace GuiProtocol
{    
    enum class GuiClientState_E
    {
        INITIALIZED,
        WIDGET_LIST_REQUESTED,
        WIDGET_LIST_RECEIVED,
    };

    enum class GuiClientReqStatus_E
    {
        SUCCESS,
        FAILED_TO_SEND_MSG,
        ERROR,
    };
    class GuiClient_C
    {
        public:
            GuiClient_C();
            ~GuiClient_C();
            void ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();
            GuiClientReqStatus_E SendWidgetListRequest();
            GuiClientReqStatus_E SendSetValueRequest(WidgetSetValueIdentifier_T& widgetKeyValPairs);

            const std::unordered_map<std::string, WidgetValueStorage_T>& WidgetList() const
            {
                return _widgetList;
            }

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void(WidgetReplyStatus_E)> OnWidgetListReplyReceived;
            std::function<void(WidgetReplyStatus_E, std::vector<WidgetSetValueReplyContainer_T>&)> OnWidgetSetValueReplyReceived;

        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListReply(Message_T& msg);
            void ProcessReceivedWidgetSetValueReply(Message_T& msg);
            void ProcessUpdatedWidgets();
            std::vector<WidgetValueStorage_T> GenerateWidgetValueList(WidgetSetValueIdentifier_T& widgetKeyValPairs);

            /* Member Variables */
            GuiClientState_E _state = GuiClientState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            bool _widgetListRequested = false;
            bool _widgetListReceived = false;
            std::unordered_map<std::string, WidgetValueStorage_T> _widgetList;
            std::vector<std::string> _updatedWidgets;
    };
}

#endif // GUI_CLIENT_API_H