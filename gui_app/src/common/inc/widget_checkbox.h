#ifndef WIDGET_CHECKBOX_H
#define WIDGET_CHECKBOX_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_checkbox_toggle.h"

class WidgetCheckbox_C : public WidgetInterface_I
{
    public:
        WidgetCheckbox_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId);
        ~WidgetCheckbox_C();

        void ShowWidget() override;
        bool SetWidgetValue(const char* label, bool status);

        WidgetTypes_E GetType() override;
    
    private:
        std::string _checkLabel;
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
        bool _status;
};
#endif // WIDGET_CHECKBOX_H