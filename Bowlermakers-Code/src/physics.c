#include "physics.h"

void calculate_ball_position(float *x, float *y, float theta, float v) {
  *x += v * cos(theta * M_PI / 90);
  *y += v * sin(theta * M_PI / 90);
}
