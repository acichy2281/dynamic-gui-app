#include "stdafx.h"
#include "event_input_text_changed.h"

EventInputTextChanged_C::EventInputTextChanged_C(uint16_t windowId, uint16_t widgetId, std::string widgetText) : _windowId(windowId), _widgetId(widgetId), _widgetText(widgetText)
{
}

EventInputTextChanged_C::~EventInputTextChanged_C()
{

}