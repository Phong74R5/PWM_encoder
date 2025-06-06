#ifndef I2C_H
#define I2C_H

#include "stm32f1xx.h" // Thay đổi nếu sử dụng dòng vi điều khiển khác

// ==== Khởi tạo I2C ====
void I2C1_Init(void);

// ==== Các chức năng giao tiếp I2C ====
void I2C1_Start(void);
void I2C1_SendAddress(uint8_t address);
void I2C1_WriteData(uint8_t data);
void I2C1_Stop(void);

#endif // I2C_H
