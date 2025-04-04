#ifndef WIDGET_MENU_H
#define WIDGET_MENU_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_button_press.h"
#include "thread_safe_queue.h"
#include "widget_menu_item.h"

struct AddMenuWidgetInfo_T : public AddWidgetInfo_T {
    std::vector<AddMenuItemWidgetInfo_T> menuItems;
};

class WidgetMenu_C : public WidgetInterface_I
{
    public:
        WidgetMenu_C(const std::shared_ptr<const AddMenuWidgetInfo_T>& info);
        ~WidgetMenu_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        WidgetTypes_E GetWidgetType() override { return WidgetTypes_E::Menu; }
        WidgetDataTypes_E GetDataType() override { return WidgetDataTypes_E::None; } // Menu does not have a data type

        std::vector<std::shared_ptr<WidgetInterface_I>>& GetMenuItems();

    private:
        std::vector<std::shared_ptr<WidgetInterface_I>> _menuItems;
};

#endif // WIDGET_MENU_H