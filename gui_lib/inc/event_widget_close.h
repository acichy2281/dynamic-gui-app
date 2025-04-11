#ifndef EVENT_WIDGET_CLOSE_H
#define EVENT_WIDGET_CLOSE_H

#include "stdafx.h"
#include "event_interface.h"

class EventWidgetClose_C : public EventInterface_I
{
	public:
		EventWidgetClose_C(uint16_t windowId, uint16_t widgetId);
		~EventWidgetClose_C();

        EventTypes_E GetType() override { return EventTypes_E::WidgetClose; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        WidgetValueVariant_T GetValue() const override { return true; } // The value of this event is true as a close event has occured.
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;

};
#endif // EVENT_WIDGET_CLOSE_H