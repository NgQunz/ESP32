#include "PrintPicture.h"
#include "weather.h"
int16_t xpos = 0;
int16_t ypos = 0;
void pngDraw(PNGDRAW *pDraw)
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
    tft.pushImage(xpos, ypos + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}
////////////////////////////////////////////////////////
void DrawPNGBitmap()
{
    int16_t rc = png.openFLASH((uint8_t *)weather, sizeof(weather), pngDraw);
    if (rc == PNG_SUCCESS)
    {
        tft.startWrite();
        uint32_t dt = millis();
        rc = png.decode(NULL, 0);
        tft.endWrite();
        // png.close(); // not needed for memory->memory decode
    }
}