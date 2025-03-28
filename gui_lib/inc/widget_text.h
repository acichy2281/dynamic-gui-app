#ifndef WIDGET_TEXT_H
#define WIDGET_TEXT_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetText_C : public WidgetInterface_I
{
    public:
        WidgetText_C(uint16_t windowId);
        ~WidgetText_C();
        void ShowWidget() override;
        
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;

        WidgetTypes_E GetType() override;
    
    private:
        std::string _widgetText;
};
#endif // WIDGET_TEXT_H