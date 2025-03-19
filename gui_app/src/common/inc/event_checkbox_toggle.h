#ifndef EVENT_CHECKBOX_TOGGLE_H
#define EVENT_CHECKBOX_TOGGLE_H

#include "stdafx.h"
#include "event_interface.h"

class EventCheckboxToggle : public EventInterface_I
{
	public:
		EventCheckboxToggle(uint16_t windowId, uint16_t widgetId, bool isChecked);
		~EventCheckboxToggle();

        EventTypes_E GetType() override { return EventTypes_E::CHECKBOX_TOGGLE; }
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        bool GetValue() const { return _isChecked; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
        bool _isChecked;
};
#endif // EVENT_CHECKBOX_TOGGLE_H