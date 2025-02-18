#include "gui_server_api.h"

/* System includes */
#include <chrono>
namespace GuiProtocol
{
    GuiServer_C::GuiServer_C()
    {

    }

    GuiServer_C::~GuiServer_C()
    {

    }

    void GuiServer_C::GuiServer_ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        Message_T rxMsg = {std::move(msg), size};
        _msgQueue.AddMessageToQueue(std::move(rxMsg));
    }

    void GuiServer_C::GuiServer_ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            auto msg = _msgQueue.GetMessageFromQueue();
            // Process msg based on msg id 
        }
        else 
        {
            ProcessStateMachine();
        }
    }

    WidgetDescriptor_T GuiServer_C::GuiServer_GetWidgetDesc(uint16_t windowId, uint16_t widgetId, bool isInteractable, bool isStatic, WidgetTypes_E widgetType, WidgetDataTypes_E widgetDataType, std::string& widgetName)
    {
        WidgetDescriptor_T retVal;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.isInteractable = isInteractable;
        retVal.isStatic = isStatic;
        retVal.widgetType = static_cast<uint8_t>(widgetType);
        retVal.dataType = static_cast<uint8_t>(widgetDataType);
        retVal.widgetName = widgetName;
        return retVal;
    }

    bool GuiServer_C::GuiServer_SetWidgetList(std::vector<WidgetDescriptor_T>& descList)
    {
        _descList = descList;
        _widgetListPopulated = true;
        return true;
    }

    uint64_t GuiServer_C::GetCurrentTimeMs()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        );
    }

    void GuiServer_C::ProcessStateMachine()
    {
        switch (_state)
        {
            case GuiServerState_E::INITIALIZED:
                if (true == _widgetListPopulated)
                {
                    _state = GuiServerState_E::WIDGET_LIST_POPULATED;
                }
                break;

            case GuiServerState_E::WIDGET_LIST_POPULATED:
                
                if (true == _widgetListReplySent)
                {
                    _state = GuiServerState_E::WIDGET_LIST_REPLY_SENT;
                } 
                break;

            case GuiServerState_E::WIDGET_LIST_REPLY_SENT:
                
                break;
                
            default:
                break;
        }
    }

    void GuiServer_C::ProcessReceivedMessageQueue()
    {
        auto msg = _msgQueue.GetMessageFromQueue();
        uint16_t msgId = (static_cast<uint8_t>(msg.data[2]) << 8) | static_cast<uint8_t>(msg.data[3]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::WIDGET_LIST_REQ:
                ProcessReceivedWidgetListRequest();
                break;
            
            default:
                break;
        }
    }

    void GuiServer_C::ProcessReceivedWidgetListRequest()
    {
        if (GuiServerState_E::WIDGET_LIST_POPULATED == _state)
        {
            std::vector<uint8_t> buffer;
            _msgSerializer.Serialize(GetWidgetListReply(_descList), buffer);
            if (0 > GuiServer_SendMessage(buffer))
            {
                _widgetListReplySent = true;
            }
        }
        else if (GuiServerState_E::WIDGET_LIST_POPULATED == _state)
        {
            
        }
    }
}