#include "stdafx.h"
#include "event_interface.h"

class EventButtonPress_C : public EventInterface_I
{
	public:
		EventButtonPress_C(uint16_t widgetId);
		~EventButtonPress_C();

		EventTypes_E GetType();

	private:
		uint16_t _widgetId;
};