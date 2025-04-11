/* Project includes */
#include "stdafx.h"
#include "widget_input_text.h"
#include "custom_types.h"

WidgetInputText_C::WidgetInputText_C(const std::shared_ptr<const AddWidgetInfo_T>& info) : WidgetInterface_I(info) 
{
    _isWritable = false;
    _isReadable = true;
    _isInteractable = true;
    _isStatic = false;
}

WidgetInputText_C::~WidgetInputText_C()
{

}


void WidgetInputText_C::ShowWidget()
{
    if (ImGui::InputTextWithHint("##withHint", "Enter text...", _inputBuffer, sizeof(_inputBuffer))) 
    {
        std::string newText = _inputBuffer;
        if (newText != _widgetText) 
        {
            _widgetText = newText;
            std::cout << "Input text changed! Window ID: " << _windowId << " Widget ID: " << _widgetId << " New Text: " << _widgetText << "\n";
            auto event = std::make_shared<EventInputTextChanged_C>(_windowId, _widgetId, _widgetText);
            EventQueue()->Enqueue(std::move(event));
        }
    }
}

WidgetValueVariant_T WidgetInputText_C::GetWidgetValue()
{
    // Return the current text value of the widget
    return _widgetText;
}

bool WidgetInputText_C::SetWidgetValue(WidgetValueVariant_T val)
{
    // This is not a writeable widget
    return false;
}