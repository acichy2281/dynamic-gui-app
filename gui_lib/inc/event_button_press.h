#ifndef EVENT_BUTTON_PRESS_H
#define EVENT_BUTTON_PRESS_H

#include "stdafx.h"
#include "event_interface.h"

class EventButtonPress_C : public EventInterface_I
{
	public:
		EventButtonPress_C(uint16_t windowId, uint16_t widgetId);
		~EventButtonPress_C();

        EventTypes_E GetType() override { return EventTypes_E::ButtonPress; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        WidgetValueVariant_T GetValue() const override { return true; } // The value of this event is true as a button press indicates an action occurred.
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
};

#endif // EVENT_BUTTON_PRESS_H