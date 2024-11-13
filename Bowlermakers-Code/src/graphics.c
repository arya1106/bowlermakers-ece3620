#include "graphics.h"

void alley() {
    LCD_Clear(0xa325);
    for (int i = 0; i < 12; i++)
    {
        LCD_DrawLine(0,20*i,319,20*i,0x79e0);
        LCD_DrawLine(0,19+20*i,319,19+20*i,0x79e0);
    }

    int p[] = PLANKS;

    for (int i = 0; i < 24; i++)
    {
        int c = p[i] % 310;
        int r = 20*(p[i] / 310);

        LCD_DrawFillRectangle(c, r+9, c+1, r+10, 0xb5b6);
        LCD_DrawFillRectangle(c+4, r, c+5, r+19, 0x79e0);
        LCD_DrawFillRectangle(c+8, r+9, c+9, r+10, 0xb5b6);
    }
}