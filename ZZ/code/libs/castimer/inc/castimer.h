#ifndef castimer_H
#define castimer_H





struct castimer{
	unsigned int elapsed_time = 0;	
	bool enabled = 0;
	castimer(){
		cas_timer_add_to_arr(this);
	}
};

void castimer_reset(castimer* timer_struct);
unsigned int  castimer_vrednost(castimer* timer_struct);
void cas_timer_increment();
void cas_timer_add_to_arr(castimer* timer_struct);

#endif