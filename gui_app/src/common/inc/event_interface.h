#include "stdafx.h"

class EventInterface_I
{
	public:
		virtual ~EventInterface_I() {}
		virtual EventTypes_E GetType() = 0;
};