#include "stdafx.h"
#include "event_interface.h"

class EventSliderSet_C : public EventInterface_I
{
	public:
		EventSliderSet_C(uint16_t widgetId);
		~EventSliderSet_C();

		EventTypes_E GetType();
	
	private:
		uint16_t _widgetId;

};