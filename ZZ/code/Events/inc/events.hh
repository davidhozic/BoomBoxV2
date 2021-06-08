
enum enum_system_event
{
	POWER_UP,	
	SHUTDOWN,
	POWER_SWITCH_EXTERNAL,
	POWER_SWITCH_INTERNAL,
	INITIALIZATION
};

void system_event(enum_system_event event);
