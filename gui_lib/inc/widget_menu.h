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
    WidgetMenu_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, std::vector<std::shared_ptr<WidgetInterface_I>> menuItems);
    ~WidgetMenu_C();

    void ShowWidget() override;
    WidgetValueVariant_T GetWidgetValue() override;
    bool SetWidgetValue(WidgetValueVariant_T val) override;
    std::vector<std::shared_ptr<WidgetInterface_I>>& GetMenuItems();
    void SetFlags(uint8_t flags) override;
    WidgetDescriptor_T GetDescriptor() override;
    WidgetTypes_E GetType() override;

private:
    uint16_t _id;
    std::vector<std::shared_ptr<WidgetInterface_I>> _menuItems;
    // std::map<uint16_t, std::shared_ptr<WidgetInterface_I>> _menuItemList;
    ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};

#endif // WIDGET_MENU_H