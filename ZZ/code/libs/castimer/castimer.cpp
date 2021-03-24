

#include "libs/castimer/castimer.h"
#include "common/inc/global.h"

unsigned long castimer::vrednost(void)
{
  if (tr == false)
  {
    ts = Hardware.timeFROMboot;
    tr = true;
    return 0;
  }
  return Hardware.timeFROMboot - ts;
}

void castimer::ponastavi(void)
{
  tr = false;
}

