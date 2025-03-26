/* Project includes */
#include "stdafx.h"
#include "widget_slider.h"

WidgetSlider_C::WidgetSlider_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId) : _eventQueue(eventQueue)
{
    SetWindowId(windowId);
    SetWidgetId(0); // Default widget ID
    SetIsStatic(false);
}

WidgetSlider_C::~WidgetSlider_C()
{

}

void WidgetSlider_C::ShowWidget()
{
    // ShowWidget override
    ImGui::SliderFloat(_sliderLabel.c_str(), &_sliderValue, _sliderMin, _sliderMax);
    
    if (ImGui::IsItemActive()) {
        _isSliderActive = true; // The user is modifying the slider
    } 
    else if (_isSliderActive) 
    {
        _isSliderActive = false; // The user just stopped modifying the slider
        if (_previousValue != _sliderValue) 
        {
            std::cout << "Show Widget: Updated Slider value: " << _sliderValue << std::endl;
            auto event = std::make_shared<EventSliderSet_C>(GetWindowId(), GetWidgetId(), _sliderValue);
            _eventQueue.Enqueue(std::move(event));
            _previousValue = _sliderValue; // Update previous value
        }
    }
}

bool WidgetSlider_C::SetWidgetValue(const char* label, float* value, float min, float max)
{
    if (GetIsStatic()) 
    {
        return false;
    }
    // Check for invalid input for the slider values, abort if so
    if (!label || !value || min >= max)
    {
        _sliderLabel.clear();
        return false;
    }

    _sliderLabel = label;
    _sliderValue = *value;
    _sliderMin = min;
    _sliderMax = max;
    return true;
}

WidgetTypes_E WidgetSlider_C::GetType()
{
    return WidgetTypes_E::SLIDER;
}