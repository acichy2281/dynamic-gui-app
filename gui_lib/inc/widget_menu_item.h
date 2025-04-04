#ifndef WIDGET_MENU_ITEM_H
#define WIDGET_MENU_ITEM_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_button_press.h"
#include "thread_safe_queue.h"

struct AddMenuItemWidgetInfo_T : public AddWidgetInfo_T {
    std::vector<std::string> menuItemWidgetButtonsList;
};

class WidgetMenuItem_C : public WidgetInterface_I
{
    public:
        WidgetMenuItem_C(const std::shared_ptr<const AddMenuItemWidgetInfo_T>& info);
        ~WidgetMenuItem_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::MenuItem; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::None; } // Menu items do not have a data type

    private:
};

#endif // WIDGET_MENU_ITEM_H