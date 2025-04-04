#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include "stdafx.h"
#include "event_interface.h"

struct AddWidgetInfo_T
{
    uint16_t windowId;
    uint16_t widgetId;
    uint8_t flags;
    WidgetTypes_E widgetType;
    WidgetDataTypes_E dataType;
    std::string widgetName;
    WidgetValueVariant_T defaultValue;
    std::optional<std::reference_wrapper<ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>>> eventQueue;
    AddWidgetInfo_T() = default;
    AddWidgetInfo_T(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& queue)
        : eventQueue(queue) { }
    virtual ~AddWidgetInfo_T() = default;
};

class WidgetInterface_I 
{
    public:
        WidgetInterface_I(const std::shared_ptr<const AddWidgetInfo_T>& info) : 
        _eventQueue(info->eventQueue), 
        _widgetName(info->widgetName),
        _widgetId(info->widgetId),
        _windowId(info->windowId)
        {
            _isReadable = (info->flags & WidgetFlags_E::Readable) != 0;
            _isWritable = (info->flags & WidgetFlags_E::Writeable) != 0;
            _isStatic = (info->flags & WidgetFlags_E::Static) != 0;
            _isInteractable = (info->flags & WidgetFlags_E::Interactable) != 0;
        }
        virtual ~WidgetInterface_I() {}
        
        virtual void ShowWidget() = 0;
        virtual WidgetTypes_E GetWidgetType() = 0;
        virtual WidgetDataTypes_E GetDataType() = 0;
        virtual WidgetValueVariant_T GetWidgetValue() = 0; 
        virtual bool SetWidgetValue(WidgetValueVariant_T val) = 0;
        const WidgetDescriptor_T GetDescriptor()
        {
            uint8_t flags = 0;
            flags |= (_isReadable ? WidgetFlags_E::Readable : 0);
            flags |= (_isWritable ? WidgetFlags_E::Writeable : 0);
            flags |= (_isStatic ? WidgetFlags_E::Static : 0);
            flags |= (_isInteractable ? WidgetFlags_E::Interactable : 0);
            
            return GuiProtocol::GetWidgetDescriptor(
                                   _windowId, 
                                   _widgetId, 
                                   flags,
                                   GetWidgetType(), 
                                   GetDataType(),
                                   _widgetName);
        }   

    protected:
        bool _isReadable = false;
        bool _isWritable = false;
        bool _isStatic = false;
        bool _isInteractable = false;
        std::string _widgetName;
        uint16_t _windowId = 0; // Assigned by the window that contains this widget
        uint16_t _widgetId = 0; // Assigned by the window that contains this widget
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>* EventQueue() 
        {
            if (_eventQueue.has_value()) 
            {
                return &_eventQueue.value().get();
            }
            else
            {
                std::throw_with_nested(std::runtime_error("Event queue not set for widget"));
            }
            return nullptr;
        }

    private:
        nlohmann::json _jsonData;
        std::optional<std::reference_wrapper<ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>>> _eventQueue;
};
#endif // WIDGET_INTERFACE_H