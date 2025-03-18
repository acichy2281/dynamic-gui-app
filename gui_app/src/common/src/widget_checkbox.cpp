/* Project includes */
#include "stdafx.h"
#include "widget_checkbox.h"

WidgetCheckbox_C::WidgetCheckbox_C()
{
    this->_id = -1;
}

WidgetCheckbox_C::~WidgetCheckbox_C()
{

}

void WidgetCheckbox_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Checkbox(_checkLabel.c_str(), &_status)) 
    {
        std::cout << "Checkbox toggled!\n";
        // TODO: How do I send the event to the main window?
    }
}

bool WidgetCheckbox_C::SetWidgetValue(const char* label, bool status)
{
    if (!label)
    {
        _checkLabel.clear();
        return false;
    }
    _checkLabel = label;
    return true;
}

WidgetTypes_E WidgetCheckbox_C::GetType()
{
    return WidgetTypes_E::CHECKBOX;
}

void WidgetCheckbox_C::AssignId(uint16_t widgetId)
{
    this->_id = widgetId;
}
