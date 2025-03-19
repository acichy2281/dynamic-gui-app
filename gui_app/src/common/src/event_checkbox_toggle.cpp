#include "stdafx.h"
#include "event_checkbox_toggle.h"

EventCheckboxToggle::EventCheckboxToggle(uint16_t windowId, uint16_t widgetId, bool isChecked) : _windowId(windowId), _widgetId(widgetId), _isChecked(isChecked)
{
}

EventCheckboxToggle::~EventCheckboxToggle()
{

}