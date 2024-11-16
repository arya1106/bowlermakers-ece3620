#include "assets/ball_graphic.h"
#include "device_drivers/lcd.h"
#include "graphics.h"

void move_ball(u16 oldX, u16 oldY, u16 newX, u16 newY) {
  TempPicturePtr(tmp, 30, 30);
  alley(oldX - (tmp->width / 2), oldY - (tmp->height / 2), tmp->width,
        tmp->height, tmp);
  LCD_DrawPicture(oldX - (tmp->width / 2), oldY - (tmp->height / 2), tmp,
                  false);
  if (newX + (ball_graphic.width) < LCD_H &&
      newY + (ball_graphic.width) < LCD_W) {
    alley(newX - (tmp->width / 2), newY - (tmp->height / 2), tmp->width,
          tmp->height, tmp);
    pic_overlay(tmp, (tmp->width / 2) - (ball_graphic.width / 2),
                (tmp->height / 2) - (ball_graphic.height / 2), &ball_graphic,
                WHITE);
    LCD_DrawPicture(newX - (tmp->width / 2), newY - (tmp->height / 2), tmp,
                    false);
  }
}