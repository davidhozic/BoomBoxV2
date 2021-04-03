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


class class_TIMER_framework
{
private:
	std::vector <class_TIMER*> timer_list;
	unsigned char timer_num = 0;
	
public:
	void add_timer_to_list(class_TIMER* timer_to_add);
	void increment_timers();	
	class_TIMER_framework();
};




extern class_TIMER_framework timer_control;


