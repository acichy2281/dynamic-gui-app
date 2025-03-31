/* Project includes */
#include "stdafx.h"
#include "widget_slider.h"

WidgetSlider_C::WidgetSlider_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, 
                               uint16_t windowId,
                               SliderValueVariant_T min,
                               SliderValueVariant_T max) : 
    _eventQueue(eventQueue),
    _sliderMin(min),
    _sliderMax(max)
{
    SetWindowId(windowId);

    /* Initialize values to ensure type safety */
    _sliderValue = min;
    _previousValue = min;
}

WidgetSlider_C::~WidgetSlider_C()
{

}

void WidgetSlider_C::ShowWidget()
{
    if (std::holds_alternative<int>(_sliderValue))
    {
        int currentValue = std::get<int>(_sliderValue);
        if (ImGui::SliderInt(GetWidgetName().c_str(), &currentValue, std::get<int>(_sliderMin), std::get<int>(_sliderMax)))
        {
            _sliderValue = currentValue;
        }
    }
    else if (std::holds_alternative<float>(_sliderValue))
    {
        float currentValue = std::get<float>(_sliderValue);
        if (ImGui::SliderFloat(GetWidgetName().c_str(), &currentValue, std::get<float>(_sliderMin), std::get<float>(_sliderMax)))
        {
            _sliderValue = currentValue;
        }
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
            auto event = std::make_shared<EventSliderSet_C>(GetWindowId(), GetWidgetId(), _sliderValue);
            _eventQueue.Enqueue(std::move(event));
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
    if (false == _isWritable) 
    {
        return false;
    }
    // Check for invalid input for the slider values, abort if so
    if (std::holds_alternative<int>(val) && (std::get<int>(val) < std::get<int>(_sliderMin) || std::get<int>(val) > std::get<int>(_sliderMax)))
    {
        return false;
    }
    else if (std::holds_alternative<float>(val) && (std::get<float>(val) < std::get<float>(_sliderMin) || std::get<float>(val) > std::get<float>(_sliderMax)))
    {
        return false;
    }
    else if (false == std::holds_alternative<int>(val) && false == std::holds_alternative<float>(val))
    {
        return false;
    }

    if (std::holds_alternative<int>(val)) {
        _sliderValue = std::get<int>(val);
    } else if (std::holds_alternative<float>(val)) {
        _sliderValue = std::get<float>(val);
    }

    return true;
}

void WidgetSlider_C::SetFlags(uint8_t flags)
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

WidgetDescriptor_T WidgetSlider_C::GetDescriptor()
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
    
    GuiProtocol::WidgetDataTypes_E dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_INT;
    if (std::holds_alternative<float>(_sliderValue))
    {
        dataType = GuiProtocol::WidgetDataTypes_E::WIDGET_DATA_TYPE_FLOAT;
    }
    return GuiProtocol::GetWidgetDescriptor(
                           GetWindowId(), 
                           GetWidgetId(), 
                           flags,
                           WidgetTypes_E::Slider,
                           dataType,
                           GetWidgetName());
}

WidgetTypes_E WidgetSlider_C::GetType()
{
    return WidgetTypes_E::Slider;
}