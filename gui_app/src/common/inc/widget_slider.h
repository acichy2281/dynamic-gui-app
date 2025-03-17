#ifndef SLIDER_WIDGET_H
#define SLIDER_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_slider_set.h"

class WidgetSlider_C : public WidgetInterface_I
{
public:
    WidgetSlider_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId);
    ~WidgetSlider_C();

    void ShowWidget() override;
    bool SetWidgetValue(const char* label, float* value, float min, float max);

    WidgetTypes_E GetType() override;

private:
    std::string _sliderLabel;
    bool _isSliderActive = false;
    float _sliderValue;
    float _previousValue;
    float _sliderMin;
    float _sliderMax;
    ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};
#endif // SLIDER_WIDGET_H
