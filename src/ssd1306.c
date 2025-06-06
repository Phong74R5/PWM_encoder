#include "ssd1306.h"
// Buffer lưu trạng thái màn hình
uint8_t SSD1306_Buffer[128 * 64 / 8];
// ==== 2. Giao tiếp SSD1306 ====
void SSD1306_WriteCommand(uint8_t command) {
    I2C1_Start();
    I2C1_SendAddress(0x78); // SSD1306 address
    I2C1_WriteData(0x00);   // Control byte: Command
    I2C1_WriteData(command);
    I2C1_Stop();
}

void SSD1306_WriteData(uint8_t data) {
    I2C1_Start();
    I2C1_SendAddress(0x78); // SSD1306 address
    I2C1_WriteData(0x40);   // Control byte: Data
    I2C1_WriteData(data);
    I2C1_Stop();
}

// ==== 3. Khởi tạo SSD1306 ====
void SSD1306_Init() {
    SSD1306_WriteCommand(0xAE); // Display OFF
    SSD1306_WriteCommand(0xAE); // Display OFF
    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio/oscillator frequency
    SSD1306_WriteCommand(0x80); // Suggested ratio
    SSD1306_WriteCommand(0xA8); // Set multiplex ratio
    SSD1306_WriteCommand(0x3F); // 1/64 duty
    SSD1306_WriteCommand(0xD3); // Set display offset
    SSD1306_WriteCommand(0x00); // No offset
    SSD1306_WriteCommand(0x40); // Set start line address
    SSD1306_WriteCommand(0xA1); // Set segment re-map
    SSD1306_WriteCommand(0xC8); // Set COM output scan direction
    SSD1306_WriteCommand(0xDA); // Set COM pins hardware configuration
    SSD1306_WriteCommand(0x12); // Alternative COM pin configuration
    SSD1306_WriteCommand(0x81); // Set contrast control
    SSD1306_WriteCommand(0x7F); // Contrast value
    SSD1306_WriteCommand(0xA4); // Entire display ON
    SSD1306_WriteCommand(0xA6); // Normal display
    SSD1306_WriteCommand(0xD5); // Set display clock divide ratio
    SSD1306_WriteCommand(0x80); // Ratio value
    SSD1306_WriteCommand(0x8D); // Enable charge pump regulator
    SSD1306_WriteCommand(0x14); // Enable charge pump
    SSD1306_WriteCommand(0xAF); // Display ON
}

void SSD1306_UpdateScreen() {

    for (uint8_t col = 0; col < 128; col++) {  // Quét cột từ 0 đến 127
        for (uint8_t page = 0; page < 8; page++) {  // Quét trang từ 0 đến 7
            SSD1306_WriteCommand(0xB0 + page);  // Chọn trang
            SSD1306_WriteCommand(0x00 + (col & 0x0F));  // Chọn cột thấp
            SSD1306_WriteCommand(0x10 + ((col >> 4) & 0x0F));  // Chọn cột cao
            SSD1306_WriteData(SSD1306_Buffer[page * 128 + col]);  // Gửi dữ liệu cho cột của trang
        }
    }
}


void SSD1306_Clear() {
    for (uint16_t i = 0; i < sizeof(SSD1306_Buffer); i++) {
        SSD1306_Buffer[i] = 0x00;
    }
    SSD1306_UpdateScreen();
}

// ==== 4. Hiển thị chuỗi ====
void SSD1306_DrawChar(uint8_t x, uint8_t y, const uint8_t *ch, const uint8_t *font) {
    for (uint8_t i = 0; i < font[1]; i++) {
        SSD1306_Buffer[x + (y / 8) * 128 + i] = ch[i];
    }
}

void SSD1306_DrawString(uint8_t x, uint8_t y, const char *str, const uint8_t *font) {
    while (*str) {
        uint8_t num = *str; 
        if (num >= 32 && num <= 127) {
            const uint8_t *charFont = &font[4 + (num - 32) * font[1]];

            // Vẽ ký tự lên màn hình
            SSD1306_DrawChar(x, y, charFont, font);
        }
        x += font[1];
        if (x + font[1] >= 128) { // Nếu vượt quá màn hình, xuống dòng
            x = 0;
            y += font[2];
        }
        str++;
    }
}
void SSD1306_DrawChar8x16(uint8_t x, uint8_t y, char c, const uint8_t *font) {
    // `x` và `y` là tọa độ pixel (y cần chia thành các trang)
    // `font` là con trỏ tới dữ liệu font 8x16 (mỗi ký tự chiếm 16 byte)

    uint16_t charIndex = (c - 32) * 16; // Tính chỉ số ký tự trong font (ASCII offset)
    for (uint8_t i = 0; i < 8; i++) {  // Duyệt theo chiều ngang (8 pixel)
        // Phần trên (trang đầu tiên)
        SSD1306_Buffer[x + (y / 8) * 128 + i] = font[4 + charIndex + i];
        // Phần dưới (trang thứ hai)
        SSD1306_Buffer[x + ((y / 8) + 1) * 128 + i] = font[4 + charIndex + 8 + i];
    }
}
void SSD1306_DrawString8x16(uint8_t x, uint8_t y, const char *str, const uint8_t *font) {
    while (*str) {
        SSD1306_DrawChar8x16(x, y, *str, font);
        x += font[1]; // Dịch sang phải 8 pixel cho ký tự tiếp theo
        if (x + font[1] >= 128) { // Nếu vượt quá màn hình, xuống dòng
            x = 0;
            y += font[2];
        }
        str++;
    }
}

void SSD1306_DrawImgBG(const uint8_t *img) {
    int byte_index = 0;

    for (int row = 0; row < 8; row++) { // 64 / 8 = 8
        for (int col = 0; col < 16; col++) { // 128 / 8 = 16
            for (int bit = 7; bit >= 0; bit--) {
                uint8_t buffer = 0x00;

                for (int pixel = 0; pixel < 8; pixel++) {
                    int img_index = (128 * row) + (16 * pixel) + col;
                    buffer |= ((img[img_index] >> bit) & 0x1) << pixel;
                }

                SSD1306_Buffer[byte_index++] = buffer;
            }
        }
    }
}

