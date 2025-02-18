#ifndef GUI_CLIENT_API_H
#define GUI_CLIENT_API_H

/* System Includes */
#include <memory>
#include <inttypes.h>
#include <unordered_map>

/* Shared includes */
#include "thread_safe_queue.h"
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
            void GuiClient_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void GuiClient_ProcessTimedActivities();
            GuiClientReqStatus_E GuiClient_SendWidgetListRequest();
            bool GuiClient_SetValue(std::vector<std::pair<std::string, WidgetValueVariant_T>>& widgetKeyValPairs);

        protected:
            const std::unordered_map<std::string, Widget_T>& GuiClient_WidgetList() const
            {
                return _widgetList;
            }

        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListReply(Message_T& msg);
            void ProcessUpdatedWidgets();

            /* Callbacks */
            virtual int32_t GuiClient_SendMessage(const std::vector<uint8_t>& message) = 0;
            virtual void GuiClient_OnWidgetListReplyReceived(WidgetReplyStatus_E status) = 0;

            /* Member Variables */
            GuiClientState_E _state = GuiClientState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            bool _widgetListRequested = false;
            bool _widgetListReceived = false;
            std::unordered_map<std::string, Widget_T> _widgetList;
            std::vector<std::string> _updatedWidgets;
    };
}

#endif // GUI_CLIENT_API_H