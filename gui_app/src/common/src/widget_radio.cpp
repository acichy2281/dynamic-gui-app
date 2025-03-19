/* Project includes */
#include "stdafx.h"
#include "widget_radio.h"

WidgetRadiobutton_C::WidgetRadiobutton_C()
{
    this->_id = -1;
    this->_selected = 0;
}

WidgetRadiobutton_C::~WidgetRadiobutton_C()
{

}

void WidgetRadiobutton_C::ShowWidget()
{
    // ShowWidget override
    for (int i = 0; i < _options.size(); i++){
        ImGui::RadioButton(_options[i].c_str(), &_selected, i);
    }     
}

bool WidgetRadiobutton_C::SetWidgetValue(std::vector<std::string> options, int selected)
{
    if (options.empty() || selected < 0 || selected >= static_cast<int>(options.size()))
    {
        _options.clear();
        _selected = 0;
        return false;
    }
    _options = options;
    _selected = selected;
    return true;
}

WidgetTypes_E WidgetRadiobutton_C::GetType()
{
    return WidgetTypes_E::RADIOBUTTON;
}

void WidgetRadiobutton_C::AssignId(uint16_t widgetId)
{
    this->_id = widgetId;
}