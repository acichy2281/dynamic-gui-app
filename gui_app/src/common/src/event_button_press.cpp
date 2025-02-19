#include "stdafx.h"
#include "event_button_press.h"

EventButtonPress_C::EventButtonPress_C(uint16_t widgetId)
{
	this->_widgetId = widgetId;
}

EventButtonPress_C::~EventButtonPress_C()
{

}

EventTypes_E EventButtonPress_C::GetType()
{
	return EventTypes_E::SLIDER_SET;
}