#include "stdafx.h"
#include "event_radio_selected.h"

EventRadioSelected_C::EventRadioSelected_C(uint16_t windowId, uint16_t widgetId, int selected) : _windowId(windowId), _widgetId(widgetId), _selected(selected)
{
}

EventRadioSelected_C::~EventRadioSelected_C()
{

}