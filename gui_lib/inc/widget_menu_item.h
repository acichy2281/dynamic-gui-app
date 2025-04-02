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
    WidgetMenuItem_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue);
    ~WidgetMenuItem_C();

    void ShowWidget() override;
    WidgetValueVariant_T GetWidgetValue() override;
    bool SetWidgetValue(WidgetValueVariant_T val) override;
    void SetFlags(uint8_t flags) override;
    WidgetDescriptor_T GetDescriptor() override;
    WidgetTypes_E GetType() override;

private:
    uint16_t _id;
    ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};

#endif // WIDGET_MENU_ITEM_H