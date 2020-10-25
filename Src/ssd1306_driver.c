#include "ssd1306_driver.h"

#include <string.h>

#include "fonts.h"
#include "i2c.h"
#include "stm32f4xx_hal.h"

/// @ingroup SSD1306
/// @{

/// Драйвер SSD1306
struct SSD1306_Driver {
  /// Дескриптор шины, соединяющий с экраном
  I2C* connector;

  /// Буфер для хранения предварительного изображения
  uint8_t renderBuffer[128 * 64 / 8];
};

/// Инициализирован ли дисплей
Boolean g_SSD1306_initialized = FALSE;
/// Глобальный дескриптор драйвера
SSD1306_Driver g_SSD1306_Driver = {0};

/**
 * @brief SendCmd отправляет команду дисплею
 * @param cmd команда
 *
 * @remark Если шина занята, то операция будет ждать освобождения
 */
void SendCmd(uint8_t cmd) {
  while (!I2C_Acquire(g_SSD1306_Driver.connector)) {
    __NOP();
  }

  while (!I2C_SendCommand(g_SSD1306_Driver.connector, cmd)) {
    __NOP();
  }
  I2C_Free(g_SSD1306_Driver.connector);
}

/**
 * @brief SendDataCallback функция, вызываемая при завершении передачи данных
 * @details освобождает шины для работы других операций
 */
void SendDataCallback() { I2C_Free(g_SSD1306_Driver.connector); }

/**
 * @brief SendData передача буфера дисплею
 * @param data буфер
 * @param size размер буфера
 */
void SendData(uint8_t* data, uint16_t size) {
  while (!I2C_Acquire(g_SSD1306_Driver.connector)) {
    __NOP();
  }

  I2C_SendData(g_SSD1306_Driver.connector, SendDataCallback, data, size);
}

SSD1306_Driver* SSD1306_Init(I2C* i2c_hnd) {
  if (g_SSD1306_initialized) return &g_SSD1306_Driver;

  g_SSD1306_Driver.connector = i2c_hnd;

  HAL_Delay(100);  // Recommended delay for display starting up
  // Set Power Reset for normal screen operation
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef init = {0};
  init.Pin = GPIO_PIN_8;
  init.Mode = GPIO_MODE_OUTPUT_PP;

  HAL_GPIO_Init(GPIOA, &init);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  HAL_Delay(100);

  SendCmd(0xAE); /*display off*/
  SendCmd(0x20);
  SendCmd(0x00);

  SendCmd(0xC8); /*Com scan direction*/

  SendCmd(0x00); /*set lower column address*/
  SendCmd(0x10); /*set higher column address*/
  SendCmd(0x40); /*set display start line*/

  SendCmd(0xB0); /*set page address*/
  SendCmd(0x81); /*contract control*/
  SendCmd(0xcf); /*128*/

  SendCmd(0xA1); /*set segment remap*/

  SendCmd(0xA6); /*normal / reverse*/

  SendCmd(0xA8); /*multiplex ratio*/
  SendCmd(0x3F); /*duty = 1/64*/

  SendCmd(0xA4); /* Display RAM content*/

  SendCmd(0xD3); /*set display offset*/
  SendCmd(0x00);
  SendCmd(0xD5); /*set osc division*/
  SendCmd(0x80);

  SendCmd(0xD9); /*set pre-charge period*/
  SendCmd(0x22);

  SendCmd(0xDA); /*set COM pins*/
  SendCmd(0x12);

  SendCmd(0xdb); /*set vcomh*/
  SendCmd(0x20);
  SendCmd(0x8D); /*set charge pump disable*/
  SendCmd(0x14);

  SendCmd(0xAF); /*display ON*/

  memset(g_SSD1306_Driver.renderBuffer, 0,
         sizeof(g_SSD1306_Driver.renderBuffer));
  SendData(g_SSD1306_Driver.renderBuffer,
           sizeof(g_SSD1306_Driver.renderBuffer));

  g_SSD1306_initialized = TRUE;

  return &g_SSD1306_Driver;
}

void SSD1306_Clear(SSD1306_Driver* hnd, Point p1, Point p2) {
  if (p1.y > p2.y) {
    int32_t tmp = p1.y;
    p1.y = p2.y;
    p2.y = tmp;
  }

  if (p1.x > p2.x) {
    int32_t tmp = p1.x;
    p1.x = p2.x;
    p2.x = tmp;
  }

  for (int32_t x = p1.x; x < p2.x; x++) {
    for (int32_t y = p1.y; y < p2.y; y++) {
      int32_t page = y / 8;
      uint8_t* byte = &hnd->renderBuffer[page * 128 + x];
      (*byte) &= ~(1 << (y % 8));
    }
  }
}

void SSD1306_DrawRectangle(SSD1306_Driver* hnd, Point p1, Point p2) {
  if (p1.y > p2.y) {
    int32_t tmp = p1.y;
    p1.y = p2.y;
    p2.y = tmp;
  }

  if (p1.x > p2.x) {
    int32_t tmp = p1.x;
    p1.x = p2.x;
    p2.x = tmp;
  }

  // Draw horizontal

  for (int32_t x = p1.x; x < p2.x; x++) {
    int32_t page1 = p1.y / 8;
    uint8_t* byte1 = &hnd->renderBuffer[page1 * 128 + x];
    (*byte1) |= 1 << (p1.y % 8);

    int32_t page2 = p2.y / 8;
    uint8_t* byte2 = &hnd->renderBuffer[page2 * 128 + x];
    (*byte2) |= 1 << (p2.y % 8);
  }

  // Vertical
  for (int32_t y = p1.y; y < p2.y; y++) {
    int32_t page = y / 8;
    uint8_t* byte1 = &hnd->renderBuffer[page * 128 + p1.x];
    (*byte1) |= 1 << (y % 8);

    uint8_t* byte2 = &hnd->renderBuffer[page * 128 + p2.x];
    (*byte2) |= 1 << (y % 8);
  }
}
void SSD1306_DrawRectangleFilled(SSD1306_Driver* hnd, Point p1, Point p2) {
  if (p1.y > p2.y) {
    int32_t tmp = p1.y;
    p1.y = p2.y;
    p2.y = tmp;
  }

  if (p1.x > p2.x) {
    int32_t tmp = p1.x;
    p1.x = p2.x;
    p2.x = tmp;
  }

  for (int32_t x = p1.x; x < p2.x; x++) {
    for (int32_t y = p1.y; y < p2.y; y++) {
      int32_t page = y / 8;
      uint8_t* byte = &hnd->renderBuffer[page * 128 + x];
      (*byte) |= 1 << (y % 8);
    }
  }
}

/**
 * @brief convertChar фильтрует символы и приводит их к формату, используемому в
 * описании шрифта
 * @param ch входной символ
 * @return преобразованный символ
 */
char convertChar(char ch) {
  if (ch < 32 || ch > 126) return 0;

  return ch - 32;
}

void SSD1306_DrawText(SSD1306_Driver* hnd, Point pos, char* text) {
  sFONT font = Font12;
  int32_t bytesForWidth = 1;

  for (char* ptr = text; *ptr != 0; ptr++) {
    char ch = convertChar(*ptr);
    const uint8_t* base = &font.table[ch * bytesForWidth * font.Height];

    for (int y = 0; y < font.Height; y++) {
      const uint8_t* lane = &base[y * bytesForWidth];

      uint32_t page = (pos.y + y) / 8;
      uint8_t* byte = &hnd->renderBuffer[page * 128 + pos.x];

      for (int x = 0; x < font.Width; x++) {
        uint32_t posInLane = x / (8 * bytesForWidth);
        uint32_t needDraw = lane[posInLane] & (0b10000000 >> (x % 8));

        if (needDraw) {
          byte[x] |= 1 << ((pos.y + y) % 8);
        }
      }
    }

    pos.x += font.Width + 1;
  }
}

void SSD1306_Swap(SSD1306_Driver* hnd) {
  SendData(hnd->renderBuffer, sizeof(hnd->renderBuffer));
}

/// @}
