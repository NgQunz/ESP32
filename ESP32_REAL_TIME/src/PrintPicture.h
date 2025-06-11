#ifndef __LIBS_H__
#define __LIBS_H__

#include "SPI.h"
#include <TFT_eSPI.h>
#include <PNGdec.h>
PNG png;
#define MAX_IMAGE_WIDTH 240
extern int16_t xpos ;
extern int16_t ypos ;
TFT_eSPI tft = TFT_eSPI(240, 240);
////////////////////////////////////////////////////////
void pngDraw(PNGDRAW *pDraw);
void DrawPNGBitmap();
#endif