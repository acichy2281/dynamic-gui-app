/* Project includes */
#include "stdafx.h"
#include "widget_slider.h"

WidgetSlider_C::WidgetSlider_C(const std::shared_ptr<const AddSliderWidgetInfo_T>& info) :
    WidgetInterface_I(info),
    _sliderMin(info->sliderMin),
    _sliderMax(info->sliderMax)
{
    /* Initialize values to ensure variant type safety - Setting value to min just incase no default is provided. */
    if (std::holds_alternative<int>(_sliderMin) && std::holds_alternative<int>(_sliderMax))
    {
        _sliderValue = std::get<int>(_sliderMin);
        _previousValue = std::get<int>(_sliderMin);
        _sliderValueType = SliderValueType_E::Int;
        SetWidgetValue(info->defaultValue);
    }
    else if (std::holds_alternative<float>(_sliderMin) && std::holds_alternative<float>(_sliderMax))
    {
        _sliderValue = std::get<float>(_sliderMin);
        _previousValue = std::get<float>(_sliderMin);
        _sliderValueType = SliderValueType_E::Float;
        SetWidgetValue(info->defaultValue);
    }
    else
    {
        throw std::invalid_argument("Slider Widget data type mismatch.");
    }
}

WidgetSlider_C::~WidgetSlider_C()
{

}

void WidgetSlider_C::ShowWidget()
{
    ImGui::Text(_widgetName.c_str());
    if (SliderValueType_E::Int == _sliderValueType)
    {
        int currentValue = std::get<int>(_sliderValue);
        if (ImGui::SliderInt("##slider", &currentValue, std::get<int>(_sliderMin), std::get<int>(_sliderMax)))
        {
            _sliderValue = currentValue;
        }
    }
    else if (SliderValueType_E::Float == _sliderValueType)
    {
        float currentValue = std::get<float>(_sliderValue);
        if (ImGui::SliderFloat("##slider", &currentValue, std::get<float>(_sliderMin), std::get<float>(_sliderMax)))
        {
            _sliderValue = currentValue;
        }
    }
    else
    {
        std::cout << "Slider value type mismatch\n";
        std::throw_with_nested(std::runtime_error("Slider value type mismatch"));
    }
    
    if (ImGui::IsItemActive()) {
        _isSliderActive = true; // The user is modifying the slider
    } 
    else if (_isSliderActive) 
    {
        _isSliderActive = false; // The user just stopped modifying the slider
        if (_previousValue != _sliderValue) 
        {
            std::cout << "Show Widget: Updated Slider value: ";
            std::visit([](auto&& arg) { std::cout << arg; }, _sliderValue);
            std::cout << std::endl;
            auto event = std::make_shared<EventSliderSet_C>(_windowId, _widgetId, _sliderValue);
            EventQueue()->Enqueue(std::move(event));
            _previousValue = _sliderValue; // Update previous value
        }
    }
}

WidgetValueVariant_T WidgetSlider_C::GetWidgetValue()
{
    // Return the current value of the slider as a WidgetValueVariant_T
    return std::visit([](auto&& arg) -> WidgetValueVariant_T { return arg; }, _sliderValue);
}

bool WidgetSlider_C::SetWidgetValue(WidgetValueVariant_T val)
{
    bool retVal = false;
    if (false == _isWritable) 
    {
        retVal = false;
    }
    else if (std::holds_alternative<int>(val) && SliderValueType_E::Int == _sliderValueType && 
            (std::get<int>(val) >= std::get<int>(_sliderMin) && std::get<int>(val) <= std::get<int>(_sliderMax)))
    {
        _sliderValue = std::get<int>(val);
        _previousValue = std::get<int>(val);
        retVal = true;
    }
    else if (std::holds_alternative<float>(val) && SliderValueType_E::Float == _sliderValueType && 
            (std::get<float>(val) >= std::get<float>(_sliderMin) && std::get<float>(val) <= std::get<float>(_sliderMax)))
    {
        _sliderValue = std::get<float>(val);
        _previousValue = std::get<float>(val);
        retVal = true;
    }
    else
    {
        retVal = false;
    }
    return retVal;
}

WidgetDataTypes_E WidgetSlider_C::GetDataType()
{ 
    if (SliderValueType_E::Int == _sliderValueType)
    {
        return WidgetDataTypes_E::Int;
    }
    else if (SliderValueType_E::Float == _sliderValueType)
    {
        return WidgetDataTypes_E::Float;
    }
    else
    {
        return WidgetDataTypes_E::Unknown;
    }
}