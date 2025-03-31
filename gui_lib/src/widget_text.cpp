/* Project includes */
#include "stdafx.h"
#include "widget_text.h"
#include "custom_types.h"

WidgetText_C::WidgetText_C(uint16_t windowId)
{
    SetWindowId(windowId);
}

WidgetText_C::~WidgetText_C()
{

}


void WidgetText_C::ShowWidget()
{
    //std::cout << "Showing text widget with value: " << _widgetText << "\n";
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

void WidgetText_C::SetFlags(uint8_t flags)
{
    if (flags & WidgetFlags_E::Readable)
    {
        _isReadable = true;
    }
    if (flags & WidgetFlags_E::Writeable)
    {
        _isWritable = true;
    }
    if (flags & WidgetFlags_E::Interactable)
    {
        _isInteractable = true;
    }
    if (flags & WidgetFlags_E::Static)
    {
        _isStatic = true;
    }
}

WidgetDescriptor_T WidgetText_C::GetDescriptor() 
{
    uint8_t flags = 0;
    if (_isReadable)
    {
        flags |= WidgetFlags_E::Readable;
    }
    if (_isWritable)
    {
        flags |= WidgetFlags_E::Writeable;
    }
    if (_isInteractable)
    {
        flags |= WidgetFlags_E::Interactable;
    }
    if (_isStatic)
    {
        flags |= WidgetFlags_E::Static;
    }
    
    return GuiProtocol::GetWidgetDescriptor(
                           GetWindowId(), 
                           GetWidgetId(), 
                           flags,
                           WidgetTypes_E::Text,
                           GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_STRING,
                           GetWidgetName());
}

WidgetTypes_E WidgetText_C::GetType()
{
    return WidgetTypes_E::Text;
}