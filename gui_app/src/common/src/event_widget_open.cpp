#include "stdafx.h"
#include "event_widget_open.h"

EventWidgetOpen_C::EventWidgetOpen_C(uint16_t widgetId)
{
	this->_widgetId = widgetId;
}

EventWidgetOpen_C::~EventWidgetOpen_C()
{

}

EventTypes_E EventWidgetOpen_C::GetType()
{
	return EventTypes_E::WIDGET_OPEN;
}