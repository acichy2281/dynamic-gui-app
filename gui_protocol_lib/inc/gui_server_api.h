#ifndef GUI_SERVER_API_H
#define GUI_SERVER_API_H

/* System Includes */
#include <memory>
#include <inttypes.h>
#include <map>
#include <functional>

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
        READY,
        WIDGET_EVENT_NOTIFICATION_SENT,
    };

    enum class GuiServerReqStatus_E
    {
        SUCCESS,
        FAILED_TO_SEND_MSG,
        REQ_STATUS_ERROR,
    };

    class GuiServer_C
    {   
        public:            
            GuiServer_C(
                std::function<int32_t(const std::vector<uint8_t>&)> sendMessage,
                std::function<void()> onWidgetListRequestReceived,
                std::function<WidgetReplyStatus_E(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>&)> onWidgetSetValueRequestReceived,
                std::function<void(WidgetReplyStatus_E, uint16_t, uint16_t)> onWidgetEventNotificationAckReceived
            );
            ~GuiServer_C();
            void ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();
            WidgetDescriptor_T GetWidgetDesc(uint16_t windowId, 
                                             uint16_t widgetId,
                                             bool isInteractable, 
                                             bool isStatic, 
                                             bool isReadable,
                                             bool isWritable,
                                             WidgetTypes_E widgetType, 
                                             WidgetDataTypes_E widgetDataType, 
                                             std::string& widgetName
                                            );
            bool SetWidgetList(std::vector<WidgetDescriptor_T>& descList);
            GuiServerReqStatus_E SendWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T val);

            /* Callbacks */
            /**
             * @brief User is expected to
             * 
             */
            
        private:
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListRequest();
            void ProcessReceivedWidgetSetValueRequest(Message_T& msg);
            void ProcessReceivedWidgetEventNotificationAck(Message_T& msg);

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void()> OnWidgetListRequestReceived;
            std::function<WidgetReplyStatus_E(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>&)>  OnWidgetSetValueRequestReceived;
            std::function<void(WidgetReplyStatus_E, uint16_t, uint16_t)> OnWidgetEventNotificationAckReceived;

            /* Member Variables */
            GuiServerState_E _state = GuiServerState_E::INITIALIZED;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            // std::vector<WidgetDescriptor_T> _descList;
            std::map<uint32_t, WidgetDescriptor_T> _widgetMap;
            bool _widgetListPopulated = false;
            bool _widgetListReplySent = false;
            bool _widgetSetValueReplySent = false;
            bool _widgetEventNotificationSent = false;
            bool _widgetEventNotificationAckReceived = false;

    };
}

#endif // GUI_SERVER_API_H