

#include "libs/castimer/castimer.h"
#include "common/inc/global.h"

unsigned long castimer::vrednost(void)
{
  if (tr == false)
  {
    ts = Hardware.sys_time;
    tr = true;
    return 0;
  }
  return Hardware.sys_time - ts;
}

void castimer::ponastavi(void)
{
  tr = false;
}

