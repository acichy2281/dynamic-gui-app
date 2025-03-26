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
    if (ImGui::Checkbox(_checkLabel.c_str(), &_status)) 
    {
        std::cout << "Checkbox toggled! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << "\n";
        auto event = std::make_shared<EventCheckboxToggle_C>(GetWindowId(), GetWidgetId(), _status);
        _eventQueue.Enqueue(std::move(event));
    }
}

bool WidgetCheckbox_C::SetWidgetValue(const char* label, bool status)
{   
    if (GetIsStatic()) 
    {
        return false;
    }
    if (!label)
    {
        _checkLabel.clear();
        return false;
    }
    _status = status;
    _checkLabel = label;
    return true;
}

WidgetTypes_E WidgetCheckbox_C::GetType()
{
    return WidgetTypes_E::CHECKBOX;
}