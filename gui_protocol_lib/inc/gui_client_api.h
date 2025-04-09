#ifndef GUI_CLIENT_API_H
#define GUI_CLIENT_API_H

/* System Includes */
#include <memory>
#include <inttypes.h>
#include <map>
#include <functional>

/* Shared includes */
#include "custom_types.h"
#include "thread_safe_queue.h"

/* Project Includes */
#include "gui_protocol_messages.h"

namespace GuiProtocol
{    
    enum class GuiClientState_E
    {
        Uninitialized,
        Initialized,
        WidgetListRequested,
        WidgetSetValueRequestSent,
        WidgetGetValueRequestSent,
        Ready,
        Error,
    };

    enum class GuiClientStatus_E
    {
        Success,
        FailedToSendMsg,
        FailedToCreateRequest,
        Error,
        WidgetListNotReceived,
        NotInitialized,
        RequestInProgress,
        WidgetListAlreadyReceived,
    };

    struct GuiClientCallbacks_T
    {
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage;
        std::function<void(WidgetReplyStatus_E)> onWidgetListReplyReceived;
        std::function<void(WidgetReplyStatus_E, std::vector<WidgetSetValueReplyContainer_T>&)> onWidgetSetValueReplyReceived;
        std::function<void(uint32_t, WidgetValueVariant_T, WidgetReplyStatus_E)> onWidgetGetValueReplyReceived;
        std::function<void(uint32_t, WidgetValueVariant_T)> onWidgetEventNotificationReceived;
    };

    struct GuiClientInitParams_T
    {
        const GuiClientCallbacks_T& callbacks;
    };

    class GuiClient_C
    {
        public:
            GuiClient_C();
            ~GuiClient_C();
            GuiClientStatus_E Initialize(GuiClientInitParams_T& initParams);
            GuiClientStatus_E ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();
            GuiClientStatus_E SendWidgetListRequest();
            GuiClientStatus_E SendSetValueRequest(WidgetSetValueIdentifier_T& widgetKeyValPairs);
            GuiClientStatus_E SendGetValueRequest(uint32_t widgetId);

            const std::map<uint32_t, WidgetDescriptor_T>& WidgetList() const
            {
                return _widgetList;
            }

        private:
            bool SetCallbacks(const GuiClientCallbacks_T& callbacks);
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListReply(Message_T& msg);
            void ProcessReceivedWidgetSetValueReply(Message_T& msg);
            void ProcessReceivedWidgetGetValueReply(Message_T& msg);
            void ProcessReceivedWidgetEventNotification(Message_T& msg);   
            // std::vector<WidgetValueStorage_T> GenerateWidgetValueList(WidgetSetValueIdentifier_T& widgetKeyValPairs);

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void(WidgetReplyStatus_E)> OnWidgetListReplyReceived;
            std::function<void(WidgetReplyStatus_E, std::vector<WidgetSetValueReplyContainer_T>&)> OnWidgetSetValueReplyReceived;
            std::function<void(uint32_t, WidgetValueVariant_T, WidgetReplyStatus_E)> OnWidgetGetValueReplyReceived;
            std::function<void(uint32_t, WidgetValueVariant_T)> OnWidgetEventNotificationReceived;

            /* Member Variables */
            GuiClientState_E _state = GuiClientState_E::Initialized;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            bool _initialized = false;
            bool _errorOccured = false;
            bool _widgetListRequested = false;
            bool _widgetListReceived = false;
            bool _widgetSetValueReqSent = false;
            bool _widgetSetValueReplyReceived = false;
            bool _widgetGetValueReqSent = false;
            bool _widgetGetValueReplyReceived = false;
            std::map<uint32_t, WidgetDescriptor_T> _widgetList;
            std::vector<std::string> _updatedWidgets;
    };
}

#endif // GUI_CLIENT_API_H