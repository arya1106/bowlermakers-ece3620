#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "device_drivers/lcd.h"

#define FLASH_TIMER 255
#define PLANKS                                                                 \
  {255,  403,  681,  865,  1009, 1264, 1374, 1484, 1629,                       \
   1807, 2064, 2228, 2706, 2857, 2935, 3084, 3612}
// regenerate with [random.randint(1,310*12) for i in range(24)]
#define FRAME_TICK 0

void alley(int x, int y, int w, int h, Picture *pi);

#endif // GRAPHICS_H
