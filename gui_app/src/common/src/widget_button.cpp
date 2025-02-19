/* Project includes */
#include "stdafx.h"
#include "widget_button.h"

WidgetButton_C::WidgetButton_C()
{
    this->_id = -1;
}

WidgetButton_C::~WidgetButton_C()
{

}

void WidgetButton_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Button(_buttonLabel.c_str())) 
    {
        std::cout << "Button Press!\n";
        // TODO: How do I send the event to the main window?
    }
}

bool WidgetButton_C::SetWidgetValue(const char* label)
{
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

void WidgetButton_C::AssignId(uint16_t widgetId)
{
    this->_id = widgetId;
}
