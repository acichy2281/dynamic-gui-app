#ifndef EVENT_INTERFACE_H
#define EVENT_INTERFACE_H

#include "stdafx.h"

class EventInterface_I
{
	public:
		virtual ~EventInterface_I() {}
		virtual EventTypes_E GetType() = 0;
        virtual uint16_t GetWindowId() const = 0;
        virtual uint16_t GetWidgetId() const = 0;
        virtual WidgetValueVariant_T GetValue() const = 0;
};

#endif // EVENT_INTERFACE_H