#ifndef EVENT_INPUT_TEXT_CHANGED_H
#define EVENT_INPUT_TEXT_CHANGED_H

#include "stdafx.h"
#include "event_interface.h"

class EventInputTextChanged_C : public EventInterface_I
{
	public:
		EventInputTextChanged_C(uint16_t windowId, uint16_t widgetId, std::string widgetText);
		~EventInputTextChanged_C();

        EventTypes_E GetType() override { return EventTypes_E::InputTextChanged; } // Event type for checkbox toggle
        uint16_t GetWindowId() const override { return _windowId; }
        uint16_t GetWidgetId() const override { return _widgetId; }
        WidgetValueVariant_T GetValue() const override { return _widgetText; }
    
    private:
        uint16_t _windowId;
        uint16_t _widgetId;
        std::string _widgetText;
};
#endif // EVENT_INPUT_TEXT_CHANGED_H