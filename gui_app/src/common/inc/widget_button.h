#ifndef BUTTON_WIDGET_H
#define BUTTON_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetButton_C : public WidgetInterface_I
{
public:
    WidgetButton_C();
    ~WidgetButton_C();

    void ShowWidget() override;
    bool SetWidgetValue(const char* label);

    WidgetTypes_E GetType();
    void AssignId(uint16_t widgetId);

private:
    uint16_t _id;
    std::string _buttonLabel;
};
#endif // BUTTON_WIDGET_H
