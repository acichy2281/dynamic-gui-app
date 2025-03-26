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

bool WidgetText_C::SetWidgetValue(const char* format, ...)
{
    if (GetIsStatic()) {
        return false;
    }

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