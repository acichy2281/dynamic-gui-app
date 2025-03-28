/* Project includes */
#include "stdafx.h"
#include "widget_checkbox.h"

WidgetCheckbox_C::WidgetCheckbox_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId) : _eventQueue(eventQueue)
{
    SetWindowId(windowId);
    SetIsStatic(false);
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
    if (true == GetIsStatic()) 
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

WidgetTypes_E WidgetCheckbox_C::GetType()
{
    return WidgetTypes_E::CHECKBOX;
}