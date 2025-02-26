#ifndef TEXT_WIDGET_H
#define TEXT_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetText_C : public WidgetInterface_I
{
    public:
        WidgetText_C();
        ~WidgetText_C();
        void ShowWidget() override;
        bool SetWidgetValue(const char* format, ...);

        WidgetTypes_E GetType() override;
        void AssignId(uint16_t widgetId) override;
    
    private:
        uint16_t _id;
        std::string _widgetText;
};
#endif // TEXT_WIDGET_H