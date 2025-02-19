#include "stdafx.h"
#include "event_slider_set.h"

EventSliderSet_C::EventSliderSet_C(uint16_t widgetId)
{
	this->_widgetId = widgetId;
}

EventSliderSet_C::~EventSliderSet_C()
{

}

EventTypes_E EventSliderSet_C::GetType()
{
	return EventTypes_E::SLIDER_SET;
}