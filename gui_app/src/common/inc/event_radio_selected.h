#ifndef EVENT_RADIO_SELECTED_H
#define EVENT_RADIO_SELECTED_H

#include "stdafx.h"
#include "event_interface.h"

class EventRadioSelected : public EventInterface_I
{
	public:
		EventRadioSelected(uint16_t windowId, uint16_t widgetId, int selected);
		~EventRadioSelected();

        EventTypes_E GetType() override { return EventTypes_E::RADIO_SELECTED; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        int GetValue() const { return _selected; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
        int _selected;
};
#endif // EVENT_RADIO_SELECTED_H