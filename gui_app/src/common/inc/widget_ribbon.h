#ifndef RIBBON_WIDGET_H
#define RIBBON_WIDGET_H


#include "stdafx.h"
#include "widget_interface.h"

class WidgetRibbon_C : public WidgetInterface_I
{
public:
    WidgetRibbon_C();
    ~WidgetRibbon_C();

    void ShowWidget() override;
    bool SetRibbonButtons(const std::vector<std::string>& buttonNames);

    WidgetTypes_E GetType() override;
    void AssignId(uint16_t widgetId) override;

private:
    uint16_t _id;
    std::vector<std::string> _buttons;
};

#endif // RIBBON_WIDGET_H