#include "physics.h"

void update_ball(float *x, float *y, float theta, float v) {
  *x += v * cos(theta * M_PI / 90);
  *y += v * sin(theta * M_PI / 90);
}
