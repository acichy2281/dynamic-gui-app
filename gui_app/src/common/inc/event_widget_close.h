#include "stdafx.h"
#include "event_interface.h"

class EventWidgetClose_C : public EventInterface_I
{
	public:
		EventWidgetClose_C(uint16_t widgetId);
		~EventWidgetClose_C();

		EventTypes_E GetType();
	
	private:
		uint16_t _widgetId;

};