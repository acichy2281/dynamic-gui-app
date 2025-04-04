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
        WidgetSlider_C(const std::shared_ptr<const AddSliderWidgetInfo_T>& info);
        ~WidgetSlider_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Slider; }
        WidgetDataTypes_E GetDataType() override;


    private:
        bool _isSliderActive = false;
        SliderValueVariant_T _sliderValue;
        SliderValueVariant_T _previousValue;
        SliderValueVariant_T _sliderMin;
        SliderValueVariant_T _sliderMax;
        SliderValueType_E _sliderValueType;
};
#endif // WIDGET_SLIDER_H
