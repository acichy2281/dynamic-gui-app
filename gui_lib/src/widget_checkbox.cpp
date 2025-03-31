/* Project includes */
#include "stdafx.h"
#include "widget_checkbox.h"

WidgetCheckbox_C::WidgetCheckbox_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId) : _eventQueue(eventQueue)
{
    SetWindowId(windowId);
}

WidgetCheckbox_C::~WidgetCheckbox_C()
{

}

void WidgetCheckbox_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Checkbox(GetWidgetName().c_str(), &_status)) 
    {
        std::cout << "Checkbox toggled! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << "\n";
        auto event = std::make_shared<EventCheckboxToggle_C>(GetWindowId(), GetWidgetId(), _status);
        _eventQueue.Enqueue(std::move(event));
    }
}

WidgetValueVariant_T WidgetCheckbox_C::GetWidgetValue()
{
    // Return the current status of the checkbox (checked or not)
    return _status;
}

bool WidgetCheckbox_C::SetWidgetValue(WidgetValueVariant_T val)
{   
    if (false == _isWritable) 
    {
        return false;
    }
    else if (false == std::holds_alternative<bool>(val))
    {
        return false;
    }
    _status = std::get<bool>(val);
    return true;
}

void WidgetCheckbox_C::SetFlags(uint8_t flags)
{
    if (flags & WidgetFlags_E::Readable)
    {
        _isReadable = true;
    }
    if (flags & WidgetFlags_E::Writeable)
    {
        _isWritable = true;
    }
    if (flags & WidgetFlags_E::Interactable)
    {
        _isInteractable = true;
    }
    if (flags & WidgetFlags_E::Static)
    {
        _isStatic = true;
    }
}

WidgetDescriptor_T WidgetCheckbox_C::GetDescriptor()
{
    uint8_t flags = 0;
    if (_isReadable)
    {
        flags |= WidgetFlags_E::Readable;
    }
    if (_isWritable)
    {
        flags |= WidgetFlags_E::Writeable;
    }
    if (_isInteractable)
    {
        flags |= WidgetFlags_E::Interactable;
    }
    if (_isStatic)
    {
        flags |= WidgetFlags_E::Static;
    }
    
    return GuiProtocol::GetWidgetDescriptor(
                           GetWindowId(), 
                           GetWidgetId(), 
                           flags,
                           WidgetTypes_E::Checkbox,
                           GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_BOOL,
                           GetWidgetName());
}

WidgetTypes_E WidgetCheckbox_C::GetType()
{
    return WidgetTypes_E::Checkbox;
}