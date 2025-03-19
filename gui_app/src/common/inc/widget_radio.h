#ifndef WIDGET_RADIO_H
#define WIDGET_RADIO_H

#include "stdafx.h"
#include "widget_interface.h"
#include "event_radio_selected.h"

class WidgetRadio_C : public WidgetInterface_I
{
    public:
        WidgetRadio_C(ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& eventQueue, uint16_t windowId);
        ~WidgetRadio_C();
        void ShowWidget() override;
        bool SetWidgetValue(std::vector<std::string> _options, int selected);

        WidgetTypes_E GetType() override;

    private:
        uint16_t _id;
        std::vector<std::string> _options;
        ThreadSafeQueue_C<std::shared_ptr<EventInterface_I>>& _eventQueue;
        int _selected;
};
#endif // WIDGET_RADIO_H