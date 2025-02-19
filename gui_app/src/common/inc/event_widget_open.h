#include "stdafx.h"
#include "event_interface.h"

class EventWidgetOpen_C : public EventInterface_I
{
	public:
		EventWidgetOpen_C(uint16_t widgetId);
		~EventWidgetOpen_C();

		EventTypes_E GetType();
	
	private:
		uint16_t _widgetId;

};