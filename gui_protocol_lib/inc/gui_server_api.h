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
        Uninitialized,
        Initialized,
        WidgetListPopulated,
        Ready,
        WidgetEventNotificationSent,
        Error,
    };

    enum class GuiServerStatus_E
    {
        Success,
        FailedToSendMsg,
        ReqStatusError,
        Error,
        WidgetListNotReceived,
        NotInitialized,
        RequestInProgress,
        WidgetListAlreadyReceived,
    };

    struct GuiServerCallbacks_T
    {
        std::function<int32_t(const std::vector<uint8_t>&)> sendMessage;
        std::function<void()> onWidgetListRequestReceived;
        std::function<WidgetReplyStatus_E(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>&)> onWidgetSetValueRequestReceived;
        std::function<WidgetReplyStatus_E(uint32_t, WidgetValueVariant_T&)> onWidgetGetValueRequestReceived;
        std::function<void(WidgetReplyStatus_E, uint16_t, uint16_t)> onWidgetEventNotificationAckReceived;
        std::function<WidgetReplyStatus_E(std::vector<nlohmann::json>&, std::vector<WidgetDescriptor_T>&)> onAddWidgetRequestReceived;
    };

    struct GuiServerInitParams_T
    {
        GuiServerCallbacks_T callbacks;
    };

    class GuiServer_C
    {   
        public:            
            GuiServer_C();
            ~GuiServer_C();
            GuiServerStatus_E Initialize(GuiServerInitParams_T& initParams);
            GuiServerStatus_E ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size);
            void ProcessTimedActivities();
            GuiServerStatus_E SendWidgetEventNotification(uint16_t windowId, uint16_t widgetId, WidgetValueVariant_T val);
            WidgetDescriptor_T& GetWidgetDescriptor(uint32_t widgetId)
            {
                return (*_widgetMap)[widgetId];
            }
            const std::map<uint32_t, WidgetDescriptor_T>& GetWidgetList() const
            {
                return *_widgetMap;
            }
            bool SetWidgetList(std::shared_ptr<std::map<uint32_t, WidgetDescriptor_T>>& descList)
            {
                std::cout << "Setting widget list\n";
                _widgetMap = descList;
                _widgetListPopulated = true;
                return true;
            }

            /* Callbacks */
            /**
             * @brief User is expected to
             * 
             */
            
        private:
            bool SetCallbacks(const GuiServerCallbacks_T& callbacks);
            uint64_t GetCurrentTimeMs();
            void ProcessStateMachine();
            void ProcessReceivedMessageQueue();
            void ProcessReceivedWidgetListRequest();
            void ProcessReceivedWidgetSetValueRequest(Message_T& msg);
            void ProcessReceivedWidgetGetValueRequest(Message_T& msg);
            void ProcessReceivedWidgetEventNotificationAck(Message_T& msg);
            void ProcessReceivedAddWidgetRequest(Message_T& msg);

            /* Callbacks */
            std::function<int32_t(const std::vector<uint8_t>&)> SendMessage;
            std::function<void()> OnWidgetListRequestReceived;
            std::function<WidgetReplyStatus_E(std::vector<GuiProtocol::WidgetSetValueResponseReturn_T>&)>  OnWidgetSetValueRequestReceived;
            std::function<WidgetReplyStatus_E(uint32_t, WidgetValueVariant_T&)> OnWidgetGetValueRequestReceived;
            std::function<void(WidgetReplyStatus_E, uint16_t, uint16_t)> OnWidgetEventNotificationAckReceived;
            std::function<WidgetReplyStatus_E(std::vector<nlohmann::json>&, std::vector<WidgetDescriptor_T>&)> OnAddWidgetRequestReceived;

            /* Member Variables */
            GuiServerState_E _state = GuiServerState_E::Initialized;
            ThreadSafeQueue_C<Message_T> _msgQueue;
            GuiProtocolMessageSerializer _msgSerializer;
            std::shared_ptr<std::map<uint32_t, WidgetDescriptor_T>> _widgetMap;
            bool _initialized = false;
            bool _errorOccured = false;
            bool _widgetListPopulated = false;
            bool _widgetListReplySent = false;
            bool _widgetSetValueReplySent = false;
            bool _widgetGetValueReplySent = false;
            bool _widgetEventNotificationSent = false;
            bool _widgetEventNotificationAckReceived = false;
            bool _addWidgetReplySent = false;
    };
}

#endif // GUI_SERVER_API_H