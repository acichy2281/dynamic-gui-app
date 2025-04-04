#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_button_press.h"
#include "thread_safe_queue.h"

class WidgetButton_C : public WidgetInterface_I
{
    public:
        WidgetButton_C(const std::shared_ptr<const AddWidgetInfo_T>& info);
        ~WidgetButton_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Button; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::Bool; } 

    private:
};
#endif // WIDGET_BUTTON_H
