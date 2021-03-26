

#include "castimer/castimer.h"
#include "common/inc/global.h"

unsigned long class_TIMER::vrednost(void)
{
  if (tr == false)
  {
    ts = Hardware.sys_time;
    tr = true;
    return 0;
  }
  return Hardware.sys_time - ts;
}

void class_TIMER::ponastavi(void)
{
  tr = false;
}

