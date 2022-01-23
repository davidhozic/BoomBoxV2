


enum enum_system_event
{
	EV_POWER_UP,	
	EV_SHUTDOWN,
	EV_POWER_SWITCH_EXTERNAL,
	EV_POWER_SWITCH_INTERNAL,
	EV_INITIALIZATION,
};

void system_event(enum_system_event event);
