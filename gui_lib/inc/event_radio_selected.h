#ifndef EVENT_RADIO_SELECTED_H
#define EVENT_RADIO_SELECTED_H

#include "stdafx.h"
#include "event_interface.h"

class EventRadioSelected_C : public EventInterface_I
{
	public:
		EventRadioSelected_C(uint16_t windowId, uint16_t widgetId, int selected);
		~EventRadioSelected_C();

        EventTypes_E GetType() override { return EventTypes_E::radioSelected; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        WidgetValueVariant_T GetValue() const override { return _selected; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
        int _selected;
};
#endif // EVENT_RADIO_SELECTED_H