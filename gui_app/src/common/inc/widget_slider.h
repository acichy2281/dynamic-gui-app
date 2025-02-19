#ifndef SLIDER_WIDGET_H
#define SLIDER_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetSlider_C : public WidgetInterface_I
{
public:
    WidgetSlider_C();
    ~WidgetSlider_C();

    void ShowWidget() override;
    bool SetWidgetValue(const char* label, float* value, float min, float max);

    WidgetTypes_E GetType() override;
    void AssignId(uint16_t widgetId) override;

private:
    uint16_t _id;
    std::string _sliderLabel;
    float _sliderValue;
    float _sliderMin;
    float _sliderMax;
};
#endif // SLIDER_WIDGET_H
