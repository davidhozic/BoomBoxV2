

#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "FreeRTOS.h"

class_TIMER* timer_list[50];
unsigned char timer_num = 0;

unsigned long class_TIMER::vrednost()
{
	this->timer_enabled = true;
	return this->timer_value;
}

void class_TIMER::ponastavi()
{
	this->timer_enabled = false;
	this->timer_value = 0;
}

void class_TIMER::increment(){
	if (this->timer_enabled)
		timer_value += 1;	
}

class_TIMER::class_TIMER(){
	timer_list[timer_num++] = this;
}
