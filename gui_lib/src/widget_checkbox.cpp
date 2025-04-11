/* Project includes */
#include "stdafx.h"
#include "widget_checkbox.h"

WidgetCheckbox_C::WidgetCheckbox_C(const std::shared_ptr<const AddWidgetInfo_T>& info) : WidgetInterface_I(info) 
{
    _isWritable = true;
    _isReadable = true;
    _isInteractable = true;
    _isStatic = false;
    SetWidgetValue(info->defaultValue);
}

WidgetCheckbox_C::~WidgetCheckbox_C()
{

}

void WidgetCheckbox_C::ShowWidget()
{
    // ShowWidget override
    if (ImGui::Checkbox(_widgetName.c_str(), &_status)) 
    {
        std::cout << "Checkbox toggled! Window ID: " << _windowId << " Widget ID: " << _widgetId << "\n";
        auto event = std::make_shared<EventCheckboxToggle_C>(_windowId, _widgetId, _status);
        EventQueue()->Enqueue(std::move(event));
    }
}

WidgetValueVariant_T WidgetCheckbox_C::GetWidgetValue()
{
    // Return the current status of the checkbox (checked or not)
    return _status;
}

bool WidgetCheckbox_C::SetWidgetValue(WidgetValueVariant_T val)
{   
    if (false == _isWritable) 
    {
        return false;
    }
    else if (false == std::holds_alternative<bool>(val))
    {
        return false;
    }
    _status = std::get<bool>(val);
    return true;
}