#ifndef WIDGET_TEXT_H
#define WIDGET_TEXT_H

#include "stdafx.h"
#include "widget_interface.h"

struct AddTextWidgetInfo_T
{
    uint16_t windowId;
    uint8_t flags;
    std::string widgetName;
    WidgetValueVariant_T defaultValue;
};

class WidgetText_C : public WidgetInterface_I
{
    public:
        WidgetText_C(uint16_t windowId);
        ~WidgetText_C();
        void ShowWidget() override;
        
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        void SetFlags(uint8_t flags) override;
        WidgetDescriptor_T GetDescriptor() override;

        WidgetTypes_E GetType() override;
    
    private:
        std::string _widgetText;
};
#endif // WIDGET_TEXT_H