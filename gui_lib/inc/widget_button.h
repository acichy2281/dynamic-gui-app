#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_button_press.h"
#include "thread_safe_queue.h"

struct AddButtonWidgetInfo_T
{
    uint16_t windowId;
    uint8_t flags;
    std::string widgetName;
};

class WidgetButton_C : public WidgetInterface_I
{
    public:
        WidgetButton_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId);
        ~WidgetButton_C();

        void ShowWidget() override;
        WidgetValueVariant_T GetWidgetValue() override;
        bool SetWidgetValue(WidgetValueVariant_T val) override;
        void SetFlags(uint8_t flags) override;
        WidgetDescriptor_T GetDescriptor() override;

        WidgetTypes_E GetType() override;

    private:
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};
#endif // WIDGET_BUTTON_H
