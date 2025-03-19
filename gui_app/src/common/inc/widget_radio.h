#ifndef RADIO_WIDGET_H
#define RADIO_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetRadiobutton_C : public WidgetInterface_I
{
    public:
        WidgetRadiobutton_C();
        ~WidgetRadiobutton_C();
        void ShowWidget() override;
        bool SetWidgetValue(std::vector<std::string> _options, int selected);

        WidgetTypes_E GetType() override;
        void AssignId(uint16_t widgetId) override;

    private:
        uint16_t _id;
        std::vector<std::string> _options;
        int _selected;
};
#endif // RADIO_WIDGET_H