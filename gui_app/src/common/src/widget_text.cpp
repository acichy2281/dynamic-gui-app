/* Project includes */
#include "stdafx.h"
#include "widget_text.h"
#include "custom_types.h"

WidgetText_C::WidgetText_C()
{
    this->_id = -1;
}

WidgetText_C::~WidgetText_C()
{

}


void WidgetText_C::ShowWidget()
{
    //std::cout << "Showing text widget with value: " << _widgetText << "\n";
    ImGui::Text("%s", _widgetText.c_str());
}

bool WidgetText_C::SetWidgetValue(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    
    // Calculate required size
    int size = std::vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (size <= 0) {
        _widgetText.clear();
        return false;
    }

    std::vector<char> buffer(size + 1);  // Dynamic buffer
    va_start(args, format);
    std::vsnprintf(buffer.data(), buffer.size(), format, args);
    va_end(args);

    _widgetText = buffer.data();  // Assign formatted text
    return true;
}


WidgetTypes_E WidgetText_C::GetType()
{
    return WidgetTypes_E::TEXT;
}

void WidgetText_C::AssignId(uint16_t widgetId)
{
    this->_id = widgetId;
}
