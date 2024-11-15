#include "graphics.h"
#include <stdlib.h>
#include <string.h>

static void draw_point_arr(int c, int r, int x, int y, int w, int h,
                           u16 *pixel_data, u16 co) {
  if (x <= c && c < x + w && y <= r && r < y + h) {
    pixel_data[(r - y) * w + (c - x)] = co;
  }
}

void alley(int x, int y, int w, int h, Picture *pi) {
  u16 *pixel_data = pi->pix2;

  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      pixel_data[j * w + i] = 0xa325;

      if (((y + j) % 20) % 19 == 0) {
        pixel_data[j * w + i] = 0x79e0;
      }
    }
  }

  int p[] = PLANKS;

  for (int i = 0; i < 17; i++) {
    int c = p[i] % 310;
    int r = 20 * (p[i] / 310);

    draw_point_arr(c, r + 9, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c, r + 10, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 1, r + 9, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 1, r + 10, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 8, r + 9, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 8, r + 10, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 9, r + 9, x, y, w, h, pixel_data, 0xb5b6);
    draw_point_arr(c + 9, r + 10, x, y, w, h, pixel_data, 0xb5b6);

    for (int j = 4; j < 6; j++) {
      for (int k = 1; k < 19; k++) {
        draw_point_arr(c + j, r + k, x, y, w, h, pixel_data, 0x79e0);
      }
    }
  }
}