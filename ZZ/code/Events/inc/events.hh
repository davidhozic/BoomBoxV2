
#include "linked_list.hh"

enum enum_system_event
{
	POWER_UP,	
	SHUTDOWN,
	POWER_SWITCH_EXTERNAL,
	POWER_SWITCH_INTERNAL,
	INITIALIZATION
};

void system_event(enum_system_event event);
extern LIST_t <enum_system_event> m_event_stack;
