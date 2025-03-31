#ifndef EVENT_SLIDER_SET_H
#define EVENT_SLIDER_SET_H

#include "stdafx.h"
#include "event_interface.h"
#include <variant>

enum class SliderValueType_E
{
    Int,
    Float,
};

using SliderValueVariant_T = std::variant<int, float>;

class EventSliderSet_C : public EventInterface_I
{
	public:
		EventSliderSet_C(uint16_t windowId, uint16_t widgetId, SliderValueVariant_T sliderValue);
		~EventSliderSet_C();

        EventTypes_E GetType() override { return EventTypes_E::sliderSet; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        WidgetValueVariant_T GetValue() const override { return std::visit([](auto&& arg) -> WidgetValueVariant_T { return arg; }, _sliderValue); }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
        SliderValueVariant_T _sliderValue;
};
#endif // EVENT_SLIDER_SET_H