#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_checkbox_toggle.h"
#include "thread_safe_queue.h"

class WidgetCheckbox_C : public WidgetInterface_I
{
    public:
        WidgetCheckbox_C(const std::shared_ptr<const AddWidgetInfo_T>& info);
        ~WidgetCheckbox_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Checkbox; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::Bool; } 
    
    private:
        bool _status;
};
#endif // WIDGET_CHECKBOX_H