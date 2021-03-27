#ifndef castimer_H
#define castimer_H
#include <src/ArduinoSTL.h>
#include <src/vector>



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

extern class_TIMER* timer_list[50];
extern unsigned char timer_num;

#endif

