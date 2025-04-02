#ifndef WIDGET_SLIDER_H
#define WIDGET_SLIDER_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_slider_set.h"
#include "thread_safe_queue.h"

struct AddSliderWidgetInfo_T : public AddWidgetInfo_T {
    SliderValueVariant_T sliderMin;
    SliderValueVariant_T sliderMax;
};

class WidgetSlider_C : public WidgetInterface_I
{
    public:
        WidgetSlider_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, SliderValueVariant_T min, SliderValueVariant_T max);
        ~WidgetSlider_C();

        void ShowWidget() override;
        
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetDescriptor_T GetDescriptor() override;
        void SetFlags(uint8_t flags) override;

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
