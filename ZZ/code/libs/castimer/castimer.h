#pragma once


#include <vector>


class class_TIMER
{
private:
	bool timer_enabled = false;
	unsigned short timer_value = 0;

	
public:
    unsigned long vrednost();
    void ponastavi();
	void increment();
	class_TIMER();
};





void add_timer_to_list(class_TIMER* timer_to_add);
void increment_timers();	






