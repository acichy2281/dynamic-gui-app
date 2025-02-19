#ifndef BUTTON_WIDGET_H
#define BUTTON_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetButton_C : public WidgetInterface_I
{
public:
    WidgetButton_C();
    ~WidgetButton_C();

    WidgetTypes_E GetType();

    void ShowWidget() override;
    bool SetWidgetValue(const char* label);

private:
    std::string _buttonLabel;
};
#endif // BUTTON_WIDGET_H
