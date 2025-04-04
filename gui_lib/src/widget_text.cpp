/* Project includes */
#include "stdafx.h"
#include "widget_text.h"
#include "custom_types.h"

WidgetText_C::WidgetText_C(const std::shared_ptr<const AddWidgetInfo_T>& info) : WidgetInterface_I(info) 
{
    SetWidgetValue(info->defaultValue);
}

WidgetText_C::~WidgetText_C()
{

}


void WidgetText_C::ShowWidget()
{
    ImGui::Text("%s", _widgetText.c_str());
}

WidgetValueVariant_T WidgetText_C::GetWidgetValue()
{
    // Return the current text value of the widget
    return _widgetText;
}

bool WidgetText_C::SetWidgetValue(WidgetValueVariant_T val)
{
    if (false == _isWritable) 
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