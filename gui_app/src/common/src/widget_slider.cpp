/* Project includes */
#include "stdafx.h"
#include "widget_slider.h"

WidgetSlider_C::WidgetSlider_C()
{

}

WidgetSlider_C::~WidgetSlider_C()
{

}

WidgetTypes_E WidgetSlider_C::GetType()
{
    return WidgetTypes_E::SLIDER;
}

void WidgetSlider_C::ShowWidget()
{
    // ShowWidget override
    ImGui::SliderFloat(_sliderLabel.c_str(), &_sliderValue, _sliderMin, _sliderMax);
}

bool WidgetSlider_C::SetWidgetValue(const char* label, float* value, float min, float max)
{
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
