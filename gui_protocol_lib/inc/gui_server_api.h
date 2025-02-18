#ifndef GUI_SERVER_API_H
#define GUI_SERVER_API_H

/* System Includes */
#include <memory>
#include <inttypes.h>

/* Shared includes */
#include "thread_safe_queue.h"
#include "custom_types.h"

/* Project includes */
#include "gui_protocol_messages.h"

namespace GuiProtocol
{
    enum class GuiServerState_E
    {
        INITIALIZED,
        WIDGET_LIST_POPULATED,
        WIDGET_LIST_REPLY_SENT,
    };

    class GuiServer_C
    {   
        public:
            GuiServer_C();
            ~GuiServer_C();
            void GuiServer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void GuiServer_ProcessTimedActivities();
            WidgetDescriptor_T GuiServer_GetWidgetDesc(uint16_t windowId, uint16_t widgetId, bool isInteractable, bool isStatic, WidgetTypes_E widgetType, WidgetDataTypes_E widgetDataType, std::string& widgetName);
            bool GuiServer_SetWidgetList(std::vector<WidgetDescriptor_T>& descList);

            /* Callbacks */
            /**
             * @brief User is expected to
             * 
             */
            virtual void GuiServer_OnWidgetListRequestReceived() = 0;
            
        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListRequest();

            /* Callbacks */
            virtual int32_t GuiServer_SendMessage(const std::vector<uint8_t>& message) = 0;

            /* Member Variables */
            GuiServerState_E _state = GuiServerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            std::vector<WidgetDescriptor_T> _descList;
            bool _widgetListPopulated = false;
            bool _widgetListReplySent = false;

    };
}

#endif // GUI_SERVER_API_H