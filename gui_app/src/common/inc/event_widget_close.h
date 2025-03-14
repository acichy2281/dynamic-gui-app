#ifndef EVENT_WIDGET_CLOSE_H
#define EVENT_WIDGET_CLOSE_H

#include "stdafx.h"
#include "event_interface.h"

class EventWidgetClose_C : public EventInterface_I
{
	public:
		EventWidgetClose_C(uint16_t windowId, uint16_t widgetId);
		~EventWidgetClose_C();

        EventTypes_E GetType() override { return EventTypes_E::WIDGET_CLOSE; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;

};
#endif // EVENT_WIDGET_CLOSE_H