#include "stdafx.h"
#include "event_button_press.h"

EventButtonPress_C::EventButtonPress_C(uint16_t windowId, uint16_t widgetId)
{
    this->_windowId = windowId;
	this->_widgetId = widgetId;
}

EventButtonPress_C::~EventButtonPress_C()
{

}