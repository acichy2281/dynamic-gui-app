#ifndef WIDGET_TEXT_H
#define WIDGET_TEXT_H

#include "stdafx.h"
#include "widget_interface.h"

class WidgetText_C : public WidgetInterface_I
{
    public:
        WidgetText_C(const std::shared_ptr<const AddWidgetInfo_T>& info);
        ~WidgetText_C();
        
        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Text; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::String; } // Text widget returns a string value
    
    private:
        std::string _widgetText;
};
#endif // WIDGET_TEXT_H