#ifndef WIDGET_BUTTON_H
#define WIDGET_BUTTON_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_button_press.h"

class WidgetButton_C : public WidgetInterface_I
{
public:
    WidgetButton_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId);
    ~WidgetButton_C();

    void ShowWidget() override;
    bool SetWidgetValue(const char* label);

    WidgetTypes_E GetType() override;

private:
    std::string _buttonLabel;
    ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
};
#endif // WIDGET_BUTTON_H
