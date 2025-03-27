/* Project includes */
#include "stdafx.h"
#include "widget_text.h"
#include "custom_types.h"

WidgetText_C::WidgetText_C(uint16_t windowId)
{
    SetWindowId(windowId);
    SetWidgetId(0); // Default widget ID
    SetIsStatic(false);
}

WidgetText_C::~WidgetText_C()
{

}


void WidgetText_C::ShowWidget()
{
    //std::cout << "Showing text widget with value: " << _widgetText << "\n";
    ImGui::Text("%s", _widgetText.c_str());
}

bool WidgetText_C::SetWidgetValue(WidgetValueVariant_T val)
{
    if (true == GetIsStatic()) 
    {
        return false;
    }
    else if (false == std::holds_alternative<std::string>(val))
    {
        return false;
    }
    _widgetText = std::get<std::string>(val);
    return true;
}

WidgetTypes_E WidgetText_C::GetType()
{
    return WidgetTypes_E::TEXT;
}