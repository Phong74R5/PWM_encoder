#ifndef SSD1306_H
#define SSD1306_H

#include "stm32f1xx.h" 
#include "i2c.h"
// Định nghĩa địa chỉ I2C của SSD1306 (có thể cần thay đổi theo thiết lập)
#define SSD1306_I2C_ADDR 0x78

// Định nghĩa buffer hiển thị
extern uint8_t SSD1306_Buffer[128 * 64 / 8];

// ==== 1. Cấu hình và giao tiếp SSD1306 ====
void SSD1306_WriteCommand(uint8_t command);
void SSD1306_WriteData(uint8_t data);

// ==== 2. Khởi tạo SSD1306 ====
void SSD1306_Init(void);

// ==== 3. Điều khiển màn hình ====
void SSD1306_UpdateScreen(void);
void SSD1306_Clear(void);

// ==== 4. Vẽ và hiển thị ====
void SSD1306_DrawChar(uint8_t x, uint8_t y, const uint8_t *ch, const uint8_t *font);
void SSD1306_DrawString(uint8_t x, uint8_t y, const char *str, const uint8_t *font);
void SSD1306_DrawChar8x16(uint8_t x, uint8_t y, char c, const uint8_t *font);
void SSD1306_DrawString8x16(uint8_t x, uint8_t y, const char *str, const uint8_t *font);
void SSD1306_DrawImgBG(const uint8_t *img);
#endif // SSD1306_H
