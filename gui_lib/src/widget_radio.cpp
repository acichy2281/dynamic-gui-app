/* Project includes */
#include "stdafx.h"
#include "widget_radio.h"

WidgetRadio_C::WidgetRadio_C(const std::shared_ptr<const AddRadioWidgetInfo_T>& info) : WidgetInterface_I(info), _optionsList(info->radioWidgetOptionsList) 
{
    SetWidgetValue(info->defaultValue);
}
    
WidgetRadio_C::~WidgetRadio_C()
{

}

void WidgetRadio_C::ShowWidget()
{
    // ShowWidget override
    for (int i = 0; i < _optionsList.size(); i++){
        if(ImGui::RadioButton(_optionsList[i].c_str(), &_selectedOptionIndex, i))
        {
            std::cout << "Radio button selected! Window ID: " << _windowId << " Widget ID: " << _widgetId << " Option: " << i << "\n";
            auto event = std::make_shared<EventRadioSelected_C>(_windowId, _widgetId, i);
            EventQueue()->Enqueue(std::move(event));
        }
    }     
}

WidgetValueVariant_T WidgetRadio_C::GetWidgetValue()
{
    // Return the currently selected option index
    return _selectedOptionIndex;
}

bool WidgetRadio_C::SetWidgetValue(WidgetValueVariant_T val)
{
    if (false == _isWritable) 
    {
        return false;
    }
    else if (false == std::holds_alternative<int>(val))
    {
        return false;
    }
    else if (std::get<int>(val) < 0 || std::get<int>(val) >= _optionsList.size())
    {
        return false;
    }
    _selectedOptionIndex = std::get<int>(val);
    return true;
}

void WidgetRadio_C::AddOption(const std::string& newOption)
{
    _optionsList.push_back(newOption);
}