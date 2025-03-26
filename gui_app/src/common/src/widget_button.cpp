/* Project includes */
#include "stdafx.h"
#include "widget_button.h"

WidgetButton_C::WidgetButton_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId) : _eventQueue(eventQueue)
{
    SetWindowId(windowId);
    SetIsStatic(false);
}

WidgetButton_C::~WidgetButton_C()
{

}

void WidgetButton_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Button(_buttonLabel.c_str())) 
    {
        std::cout << "Button Press! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << "\n";
        auto event = std::make_shared<EventButtonPress_C>(GetWindowId(), GetWidgetId());
        _eventQueue.Enqueue(std::move(event));
    }
}

bool WidgetButton_C::SetWidgetValue(const char* label)
{
    if (GetIsStatic()) 
    {
        return false;
    }
    if (!label)
    {
        _buttonLabel.clear();
        return false;
    }
    _buttonLabel = label;
    return true;
}

WidgetTypes_E WidgetButton_C::GetType()
{
    return WidgetTypes_E::BUTTON;
}