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

    void GuiServer_C::ProcessReceivedMessage(std::unique_ptr<char[]>& msg, uint16_t size)
    {
        auto queueSizeBeforeAdd = _msgQueue.Size();
        Message_T rxMsg = {std::move(msg), size};
        _msgQueue.AddMessageToQueue(std::move(rxMsg));

        if (queueSizeBeforeAdd == _msgQueue.Size())
        {
            std::cout << "Failed to add message to received msg queue\n";
        }
        else
        {
            std::cout << "Added message to received msg queue\n";
        }
    }

    void GuiServer_C::ProcessTimedActivities()
    {
        if (false == _msgQueue.IsQueueEmpty())
        {
            ProcessReceivedMessageQueue();
        }
        else
        {
            ProcessStateMachine();
        }
    }

    WidgetDescriptor_T GuiServer_C::GetWidgetDesc(uint16_t windowId, uint16_t widgetId, bool isInteractable, bool isStatic, WidgetTypes_E widgetType, WidgetDataTypes_E widgetDataType, std::string& widgetName)
    {
        WidgetDescriptor_T retVal;
        retVal.widgetId = (static_cast<uint32_t>(windowId) << 16) | static_cast<uint32_t>(widgetId);
        retVal.isInteractable = isInteractable;
        retVal.isStatic = isStatic;
        retVal.reserved = 0;
        retVal.widgetType = static_cast<uint8_t>(widgetType);
        retVal.dataType = static_cast<uint8_t>(widgetDataType);
        retVal.widgetName = widgetName;
        return retVal;
    }

    bool GuiServer_C::SetWidgetList(std::vector<WidgetDescriptor_T>& descList)
    {
        bool retVal = false;

        if (false == _widgetListPopulated)
        {
            for (auto& desc : descList)
            {
                auto it = _widgetMap.find(desc.widgetId);
                if (it == _widgetMap.end())
                {
                    _widgetMap[desc.widgetId] = desc;
                }
                else
                {
                    std::cout << "Cannot add duplicate widget " << desc.widgetId << ":" << desc.widgetName << "\n";
                }
            }
            _widgetListPopulated = true;
        }
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
        uint16_t msgId = (static_cast<uint8_t>(msg.data[3]) << 8) | static_cast<uint8_t>(msg.data[2]);
        switch (static_cast<MessageID_E>(msgId))
        {
            case MessageID_E::WIDGET_LIST_REQ:
                std::cout << "Received Widget List Request\n";
                ProcessReceivedWidgetListRequest();
                break;

            case MessageID_E::WIDGET_SET_VALUE_REQ:
                std::cout << "Received Set Value Request\n";
                ProcessReceivedWidgetSetValueRequest(msg);
                break;

            default:
                std::cout << "Unknown message received with Message ID " << msgId << "\n";
                break;
        }
    }

    void GuiServer_C::ProcessReceivedWidgetListRequest()
    {
        if (GuiServerState_E::WIDGET_LIST_POPULATED == _state)
        {
            std::vector<uint8_t> buffer;
            
            /* Construct a list of widget descriptors */
            std::vector<WidgetDescriptor_T> descList;
            for (auto& [widgetId, widgetDesc] : _widgetMap)
            {
                descList.push_back(widgetDesc);
            }

            /* Generate a WidgetListReply serialized message */
            auto status = WidgetReplyStatus_E::SET_VAL_SUCCESS;
            auto widgetListReply = GetWidgetListReply(descList, status);
            _msgSerializer.Serialize(widgetListReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetListReplySent = true;
            }
        }
        else if (GuiServerState_E::INITIALIZED == _state)
        {
            std::cout << "Error! Widget List not populated yet!\n";
        }
        else
        {
            std::cout << "Error! Widget List already received!\n";
        }
    }

    void GuiServer_C::ProcessReceivedWidgetSetValueRequest(Message_T& msg)
    {
        if (GuiServerState_E::WIDGET_LIST_POPULATED == _state)
        {
            WidgetSetValueRequest_T reqMsg;
            std::vector<uint8_t> msgBuf(msg.data.get(), msg.data.get() + msg.size);
            _msgSerializer.Deserialize(reqMsg, msgBuf);

            /* Generate a list of Widget Set Value requests for widgets */
            std::vector<GuiProtocol::WidgetSetValueResponseReturn_T> widgetSetValueResponseList;
            for (auto& widgetToSet : reqMsg.setValuesList)
            {
                auto it = _widgetMap.find(widgetToSet.widgetId);
                if (it != _widgetMap.end())
                {
                    /* Populate a struct to pass too the GUI app for it to use to Set value */
                    WidgetSetValueResponseReturn_T widgetSetValueResponse;
                    widgetSetValueResponse.windowId = static_cast<uint16_t>(widgetToSet.widgetId >> 16);
                    widgetSetValueResponse.widgetId = static_cast<uint16_t>(widgetToSet.widgetId & 0xFFFF);
                    widgetSetValueResponse.widgetType = it->second.widgetType;
                    widgetSetValueResponse.dataType = it->second.dataType;
                    widgetSetValueResponse.val = widgetToSet.value;
                    widgetSetValueResponse.status = static_cast<uint16_t>(WidgetReplyStatus_E::SET_VAL_ERROR);
                    widgetSetValueResponseList.push_back(widgetSetValueResponse);
                }
            }
            auto status = OnWidgetSetValueRequestReceived(widgetSetValueResponseList);

            std::vector<uint8_t> buffer;
            auto widgetSetValReply = GetWidgetSetValueReply(widgetSetValueResponseList, status);
            _msgSerializer.Serialize(widgetSetValReply, buffer);
            if (0 < SendMessage(buffer))
            {
                _widgetListReplySent = true;
            }
        }
        else 
        {
            std::cout << "Error! Widget List not received\n";
        }
    }
}