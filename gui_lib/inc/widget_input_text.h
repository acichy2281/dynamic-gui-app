#ifndef WIDGET_INPUT_TEXT_H
#define WIDGET_INPUT_TEXT_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_input_text_changed.h"
#include "thread_safe_queue.h"

class WidgetInputText_C : public WidgetInterface_I
{
    public:
        WidgetInputText_C(const std::shared_ptr<const AddWidgetInfo_T>& info);
        ~WidgetInputText_C();
        
        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Text; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::String; } // Text widget returns a string value
    
    private:
        std::string _widgetText;
        char _inputBuffer[128] = "";
};
#endif // WIDGET_INPUT_TEXT_H