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
    if (ImGui::Button(GetWidgetName().c_str())) 
    {
        std::cout << "Button Press! Window ID: " << GetWindowId() << " Widget ID: " << GetWidgetId() << "\n";
        auto event = std::make_shared<EventButtonPress_C>(GetWindowId(), GetWidgetId());
        _eventQueue.Enqueue(std::move(event));
    }
}

WidgetValueVariant_T WidgetButton_C::GetWidgetValue()
{
    // Return false as the button only indicates a press event and does not have a value. True is indicated when a button event occurs. 
    return false;
}

bool WidgetButton_C::SetWidgetValue(WidgetValueVariant_T val)
{
    // Cannot set value for a button widget
    return false;
}

WidgetTypes_E WidgetButton_C::GetType()
{
    return WidgetTypes_E::BUTTON;
}