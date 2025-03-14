#include "stdafx.h"
#include "event_interface.h"

class EventButtonPress_C : public EventInterface_I
{
	public:
		EventButtonPress_C(uint16_t windowId, uint16_t widgetId);
		~EventButtonPress_C();

        EventTypes_E GetType() override { return EventTypes_E::BUTTON_PRESS; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
};