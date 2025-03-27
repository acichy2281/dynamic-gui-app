#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_slider_set.h"
#include "thread_safe_queue.h"

class WidgetSlider_C : public WidgetInterface_I
{
public:
    WidgetSlider_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId, SliderValueVariant_T min, SliderValueVariant_T max);
    ~WidgetSlider_C();

    void ShowWidget() override;
    
    bool SetWidgetValue(WidgetValueVariant_T val) override;

    WidgetTypes_E GetType() override;

private:
    bool _isSliderActive = false;
    SliderValueVariant_T _sliderValue;
    SliderValueVariant_T _previousValue;
    SliderValueVariant_T _sliderMin;
    SliderValueVariant_T _sliderMax;
    ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};
#endif // WIDGET_SLIDER_H
