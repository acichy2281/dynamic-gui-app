#ifndef CHECKBOX_WIDGET_H
#define CHECKBOX_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetCheckbox_C : public WidgetInterface_I
{
    public:
        WidgetCheckbox_C();
        ~WidgetCheckbox_C();
        void ShowWidget() override;
        bool SetWidgetValue(const char* label, bool status);

        WidgetTypes_E GetType() override;
        void AssignId(uint16_t widgetId) override;
    
    private:
        uint16_t _id;
        std::string _checkLabel;
        bool _status;
};
#endif // CHECKBOX_WIDGET_H