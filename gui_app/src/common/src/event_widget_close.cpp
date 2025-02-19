#include "stdafx.h"
#include "event_widget_close.h"

EventWidgetClose_C::EventWidgetClose_C(uint16_t widgetId)
{
	this->_widgetId = widgetId;
}

EventWidgetClose_C::~EventWidgetClose_C()
{

}

EventTypes_E EventWidgetClose_C::GetType()
{
	return EventTypes_E::WIDGET_CLOSE;
}