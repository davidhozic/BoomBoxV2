
#pragma once


class class_TIMER
{
private:
	bool timer_enabled = false;
	unsigned short timer_value = 0;

	
public:
    unsigned long vrednost();
    void ponastavi();
	void increment();
	class_TIMER(class_TIMER* it);
};


void increment_timers();	






