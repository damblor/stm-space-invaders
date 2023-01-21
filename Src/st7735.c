#include "stm32f4xx_hal.h"
#include "st7735.h"
#include "string.h"

#define DELAY 0x80

uint16_t frameBuffer1[ST7735_WIDTH * ST7735_HEIGHT];
uint16_t frameBuffer2[ST7735_WIDTH * ST7735_HEIGHT];

uint16_t *transmitBuffer;
uint16_t *writeBuffer;

int isBusy = 0;

// based on Adafruit ST7735 library for Arduino
static const uint8_t
    init_cmds1[] = {           // Init for 7735R, part 1 (red or green tab)
        15,                    // 15 commands in list:
        ST7735_SWRESET, DELAY, //  1: Software reset, 0 args, w/delay
        150,                   //     150 ms delay
        ST7735_SLPOUT, DELAY,  //  2: Out of sleep mode, 0 args, w/delay
        255,                   //     500 ms delay
        ST7735_FRMCTR1, 3,     //  3: Frame rate ctrl - normal mode, 3 args:
        0x01, 0x2C, 0x2D,      //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR2, 3,     //  4: Frame rate control - idle mode, 3 args:
        0x01, 0x2C, 0x2D,      //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
        ST7735_FRMCTR3, 6,     //  5: Frame rate ctrl - partial mode, 6 args:
        0x01, 0x2C, 0x2D,      //     Dot inversion mode
        0x01, 0x2C, 0x2D,      //     Line inversion mode
        ST7735_INVCTR, 1,      //  6: Display inversion ctrl, 1 arg, no delay:
        0x07,                  //     No inversion
        ST7735_PWCTR1, 3,      //  7: Power control, 3 args, no delay:
        0xA2,
        0x02,             //     -4.6V
        0x84,             //     AUTO mode
        ST7735_PWCTR2, 1, //  8: Power control, 1 arg, no delay:
        0xC5,             //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
        ST7735_PWCTR3, 2, //  9: Power control, 2 args, no delay:
        0x0A,             //     Opamp current small
        0x00,             //     Boost frequency
        ST7735_PWCTR4, 2, // 10: Power control, 2 args, no delay:
        0x8A,             //     BCLK/2, Opamp current small & Medium low
        0x2A,
        ST7735_PWCTR5, 2, // 11: Power control, 2 args, no delay:
        0x8A, 0xEE,
        ST7735_VMCTR1, 1, // 12: Power control, 1 arg, no delay:
        0x0E,
        ST7735_INVOFF, 0, // 13: Don't invert display, no args, no delay
        ST7735_MADCTL, 1, // 14: Memory access control (directions), 1 arg:
        ST7735_ROTATION,  //     row addr/col addr, bottom to top refresh
        ST7735_COLMOD, 1, // 15: set color mode, 1 arg, no delay:
        0x05},            //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
    init_cmds2[] = {     // Init for 7735R, part 2 (1.44" display)
        2,               //  2 commands in list:
        ST7735_CASET, 4, //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,      //     XSTART = 0
        0x00, 0x7F,      //     XEND = 127
        ST7735_RASET, 4, //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,      //     XSTART = 0
        0x00, 0x7F},     //     XEND = 127
#endif                   // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
    init_cmds2[] = {      // Init for 7735S, part 2 (160x80 display)
        3,                //  3 commands in list:
        ST7735_CASET, 4,  //  1: Column addr set, 4 args, no delay:
        0x00, 0x00,       //     XSTART = 0
        0x00, 0x4F,       //     XEND = 79
        ST7735_RASET, 4,  //  2: Row addr set, 4 args, no delay:
        0x00, 0x00,       //     XSTART = 0
        0x00, 0x9F,       //     XEND = 159
        ST7735_INVON, 0}, //  3: Invert colors
#endif

    init_cmds3[] = {                                                                                                         // Init for 7735R, part 3 (red or green tab)
        4,                                                                                                                   //  4 commands in list:
        ST7735_GMCTRP1, 16,                                                                                                  //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
        0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, ST7735_GMCTRN1, 16,  //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
        0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, ST7735_NORON, DELAY, //  3: Normal display on, no args, w/delay
        10,                                                                                                                  //     10 ms delay
        ST7735_DISPON, DELAY,                                                                                                //  4: Main screen turn on, no args w/delay
        100};                                                                                                                //     100 ms delay

/**
 * @brief Send command to ST7735
 * @param cmd Command
 */
static void ST7735_WriteCommand(uint8_t cmd)
{
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

/**
 * @brief Send data to ST7735
 * @param buff Data buffer
 * @param buff_size Data buffer size
 */
static void ST7735_WriteData(uint8_t *buff, size_t buff_size)
{
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&ST7735_SPI_PORT, buff, buff_size, HAL_MAX_DELAY);
}

/**
 * @brief Send commands to ST7735
 * @param addr Command list
 */
static void ST7735_ExecuteCommandList(const uint8_t *addr)
{
  uint8_t numCommands, numArgs;
  uint16_t ms;

  numCommands = *addr++;
  while (numCommands--)
  {
    uint8_t cmd = *addr++;
    ST7735_WriteCommand(cmd);

    numArgs = *addr++;
    // If high bit set, delay follows args
    ms = numArgs & DELAY;
    numArgs &= ~DELAY;
    if (numArgs)
    {
      ST7735_WriteData((uint8_t *)addr, numArgs);
      addr += numArgs;
    }

    if (ms)
    {
      ms = *addr++;
      if (ms == 255)
        ms = 500;
      HAL_Delay(ms);
    }
  }
}

/**
 * @brief Set the address window for the display
 * @param x0 x start position
 * @param y0 y start position
 * @param x1 x end position
 * @param y1 y end position
 */
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  // column address set
  ST7735_WriteCommand(ST7735_CASET);
  uint8_t data[] = {0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART};
  ST7735_WriteData(data, sizeof(data));

  // row address set
  ST7735_WriteCommand(ST7735_RASET);
  data[1] = y0 + ST7735_YSTART;
  data[3] = y1 + ST7735_YSTART;
  ST7735_WriteData(data, sizeof(data));

  // write to RAM
  ST7735_WriteCommand(ST7735_RAMWR);
}

/**
 * @brief Initialize the ST7735 LCD
 */
void ST7735_Init()
{
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_RESET);
  HAL_Delay(5);
  HAL_GPIO_WritePin(ST7735_RES_GPIO_Port, ST7735_RES_Pin, GPIO_PIN_SET);

  ST7735_ExecuteCommandList(init_cmds1);
  ST7735_ExecuteCommandList(init_cmds2);
  ST7735_ExecuteCommandList(init_cmds3);

  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);

  transmitBuffer = frameBuffer1;
  writeBuffer = frameBuffer2;
}

/**
 * @brief Put a pixel on the screen
 * @param x x coordinate
 * @param y y coordinate
 * @param color color of the pixel
 */
void ST7735_PutPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
    return;
  color = ST7735_SWAPBYTES(color);
  writeBuffer[x + y * ST7735_WIDTH] = color;
}

/**
 * @brief Write a character on the screen
 * @param x x coordinate
 * @param y y coordinate
 * @param ch character to write
 * @param font font to use
 * @param color color of the character
 */
void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color)
{
  uint32_t b;
  for (size_t i = 0; i < font.height; i++)
  {
    b = font.data[(ch - 32) * font.height + i];
    for (size_t j = 0; j < font.width; j++)
    {
      if ((b << j) & 0x8000)
      {
        ST7735_PutPixel(x + j, y + i, color);
      }
    }
  }
}

/**
 * @brief Write a string on the screen
 * @param x x coordinate of the top left corner
 * @param y y coordinate of the top left corner
 * @param str string to write
 * @param font font to use
 * @param color color of the string
 */
void ST7735_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color)
{
  while (*str)
  {
    if (x + font.width >= ST7735_WIDTH)
    {
      x = 0;
      y += font.height;
      if (y + font.height >= ST7735_HEIGHT)
        break;
      if (*str == ' ')
      {
        str++;
        continue;
      }
    }
    ST7735_WriteChar(x, y, *str, font, color);
    x += font.width;
    str++;
  }
}

/**
 * @brief Put a filled rectangle on the screen
 * @param x x coordinate of the top left corner
 * @param y y coordinate of the top left corner
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param color color of the rectangle
 */
void ST7735_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
    return;
  if ((x + w - 1u) >= ST7735_WIDTH)
    w = ST7735_WIDTH - x;
  if ((y + h - 1u) >= ST7735_HEIGHT)
    h = ST7735_HEIGHT - y;
  
  for (size_t i = x; i < x + w; i++)
  {
    for (size_t j = y; j < y + h; j++)
    {
      ST7735_PutPixel(i, j, color);
    }
  }
}

/**
 * @brief Set the screen to black
 */
void ST7735_ClearScreen()
{
  ST7735_DrawRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, ST7735_BLACK);
}

/**
 * @brief Set the screen to a specific color
 * @param color color to set the screen to
 */
void ST7735_ClearScreenColor(uint16_t color)
{
  ST7735_DrawRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

/**
 * @brief Flush the transmit buffer to the screen
 */
void ST7735_FlushBuffer()
{
  if (isBusy)
    return;
  isBusy = 1;
  uint16_t *tmp = transmitBuffer;
  transmitBuffer = writeBuffer;
  writeBuffer = tmp;
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
  ST7735_SetAddressWindow(0, 0, ST7735_WIDTH, ST7735_HEIGHT);
  HAL_GPIO_WritePin(ST7735_DC_GPIO_Port, ST7735_DC_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit_DMA(&ST7735_SPI_PORT, (uint8_t *)transmitBuffer, sizeof(frameBuffer1));
}

/**
 * @brief Check if the screen is busy
 * @return 1 if the screen is busy, 0 if it is not
 */
int ST7735_IsBusy()
{
  return isBusy;
}

/**
 * @brief Callback function for when the SPI transfer is done
 */
void ST7735_OnTransferDone()
{
  isBusy = 0;
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_SET);
}
