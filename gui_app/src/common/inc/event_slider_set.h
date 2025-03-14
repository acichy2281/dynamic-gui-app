#ifndef EVENT_SLIDER_SET_H
#define EVENT_SLIDER_SET_H

#include "stdafx.h"
#include "event_interface.h"

class EventSliderSet_C : public EventInterface_I
{
	public:
		EventSliderSet_C(uint16_t windowId, uint16_t widgetId);
		~EventSliderSet_C();

        EventTypes_E GetType() override { return EventTypes_E::SLIDER_SET; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;

};
#endif // EVENT_SLIDER_SET_H