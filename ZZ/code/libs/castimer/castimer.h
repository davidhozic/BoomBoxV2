
#pragma once
#ifndef SEZNAM_INC
#include "./povezan_seznam/povezan_seznam.h"
#endif


class class_TIMER
{
private:
	bool timer_enabled = false;
	unsigned short timer_value = 0;
	
public:
    unsigned short vrednost();
    void ponastavi();
	void increment();
	class_TIMER(Vozlisce <class_TIMER*> &timer_list);
};








