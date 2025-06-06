#include "i2c.h"
// ==== 1. Khởi tạo I2C ====
void I2C1_Init() {
    // Bật clock cho GPIOB và I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;   // Kích hoạt clock cho GPIOB
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;  // Kích hoạt clock cho I2C1

    // Kiểm tra lại để đảm bảo clock đã được bật
    while (!(RCC->APB2ENR & RCC_APB2ENR_IOPBEN)); // Đợi cho GPIOB clock sẵn sàng
    while (!(RCC->APB1ENR & RCC_APB1ENR_I2C1EN)); // Đợi cho I2C1 clock sẵn sàng

    // Cấu hình GPIOB cho I2C
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7); // Xóa cấu hình
    GPIOB->CRL |= (GPIO_CRL_MODE6_1 | GPIO_CRL_CNF6_1); // PB6: Output mode, max speed 2 MHz, AF Open-Drain
    GPIOB->CRL |= (GPIO_CRL_MODE7_1 | GPIO_CRL_CNF7_1); // PB7: Output mode, max speed 2 MHz, AF Open-Drain

    // Reset I2C1
    I2C1->CR1 |= I2C_CR1_SWRST;  // Bật chế độ Reset
    I2C1->CR1 &= ~I2C_CR1_SWRST; // Thoát chế độ Reset

    // Cấu hình clock cho I2C1
    I2C1->CR2 = 16;   // Tần số APB1 = 16 MHz 
    I2C1->CCR = 10;   // f = 800KHz
    I2C1->TRISE = 17;     

    // Bật I2C1
    I2C1->CR1 |= I2C_CR1_PE;     // Kích hoạt Peripheral
}


void I2C1_Start() {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C1_SendAddress(uint8_t address) {
    I2C1->DR = address;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR1;
    (void)I2C1->SR2;
}

void I2C1_WriteData(uint8_t data) {
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_TXE));
}

void I2C1_Stop() {
    I2C1->CR1 |= I2C_CR1_STOP;
}
