#ifndef TEXT_WIDGET_H
#define TEXT_WIDGET_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetText_C : public WidgetInterface_I
{
    public:
        WidgetText_C(uint16_t windowId);
        ~WidgetText_C();
        void ShowWidget() override;
        bool SetWidgetValue(const char* format, ...);

        WidgetTypes_E GetType() override;
    
    private:
        std::string _widgetText;
};
#endif // TEXT_WIDGET_H