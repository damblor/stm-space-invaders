#ifndef __ST7735_H
#define __ST7735_H

#include "fonts.h"
#include <stdbool.h>
#include "main.h"
#include "spi.h"

#define ST7735_MADCTL_MY 0x80
#define ST7735_MADCTL_MX 0x40
#define ST7735_MADCTL_MV 0x20
#define ST7735_MADCTL_ML 0x10
#define ST7735_MADCTL_RGB 0x00
#define ST7735_MADCTL_BGR 0x08
#define ST7735_MADCTL_MH 0x04

/*** Redefine if necessary ***/

#define ST7735_SPI_PORT hspi1
extern SPI_HandleTypeDef ST7735_SPI_PORT;

#define ST7735_RES_Pin RES_Pin
#define ST7735_RES_GPIO_Port RES_GPIO_Port
#define ST7735_CS_Pin CS_Pin
#define ST7735_CS_GPIO_Port CS_GPIO_Port
#define ST7735_DC_Pin DC_Pin
#define ST7735_DC_GPIO_Port DC_GPIO_Port

/****************************/

#ifndef ST7735_SPI_PORT
#error "ST7735_SPI_PORT not defined"
#endif

#ifndef ST7735_RES_Pin
#error "ST7735_RES_Pin not defined"
#endif

#ifndef ST7735_RES_GPIO_Port
#error "ST7735_RES_GPIO_Port not defined"
#endif

#ifndef ST7735_CS_Pin
#error "ST7735_CS_Pin not defined"
#endif

#ifndef ST7735_CS_GPIO_Port
#error "ST7735_CS_GPIO_Port not defined"
#endif

#ifndef ST7735_DC_Pin
#error "ST7735_DC_Pin not defined"
#endif

#ifndef ST7735_DC_GPIO_Port
#error "ST7735_DC_GPIO_Port not defined"
#endif

// AliExpress/eBay 1.8" display, default orientation

#define ST7735_IS_160X128 1
#define ST7735_WIDTH 128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY)

// AliExpress/eBay 1.8" display, rotate right
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV)
*/

// AliExpress/eBay 1.8" display, rotate left
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 128
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV)
*/

// AliExpress/eBay 1.8" display, upside down
/*
#define ST7735_IS_160X128 1
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160
#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_ROTATION (0)
*/

/****************************/

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09

#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13

#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_GAMSET 0x26
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E

#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD

#define ST7735_PWCTR6 0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

// Basic colors

#define ST7735_BLACK 0x0000
#define ST7735_WHITE 0xFFFF
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0

// Other colors

#define ST7735_NAVY 0x000F
#define ST7735_DARKGREEN 0x03E0
#define ST7735_DARKCYAN 0x03EF
#define ST7735_MAROON 0x7800
#define ST7735_PURPLE 0x780F
#define ST7735_OLIVE 0x7BE0
#define ST7735_LIGHTGREY 0xC618
#define ST7735_DARKGREY 0x7BEF
#define ST7735_ORANGE 0xFD20
#define ST7735_GREENYELLOW 0xAFE5
#define ST7735_PINK 0xF81F

// Custom colors from rgb565 to 16-bit color
#define ST7735_RGB565(r, g, b) (((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3))

// Big endian to little endian color conversion
#define ST7735_SWAPBYTES(x) (((x & 0xFF) << 8) | ((x >> 8) & 0xFF))

#ifdef __cplusplus
extern "C"
{
#endif

  void ST7735_Init();
  void ST7735_PutPixel(uint16_t x, uint16_t y, uint16_t color);
  void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color);
  void ST7735_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color);
  void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
  void ST7735_ClearScreen();
  void ST7735_ClearScreenColor(uint16_t color);
  void ST7735_FlushBuffer();
  int ST7735_IsBusy();
  void ST7735_OnTransferDone();

#ifdef __cplusplus
}
#endif

#endif /* __ST7735_H */
