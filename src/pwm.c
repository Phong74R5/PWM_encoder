#include "pwm.h"
// Hàm cấu hình TIM3 cho PWM
void TIM3_PWM_Init(void) {
    // Bật clock cho GPIOA và TIM3
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Bật clock cho GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Bật clock cho TIM3

    // Cấu hình PA6 (TIM3_CH1) ở chế độ Alternate Function Push-Pull
    GPIOA->CRL &= ~GPIO_CRL_MODE6;       // Xóa MODE6
    GPIOA->CRL &= ~GPIO_CRL_CNF6;        // Xóa CNF6
    GPIOA->CRL |= (0x03 << GPIO_CRL_MODE6_Pos);  // Output mode, max speed 50 MHz
    GPIOA->CRL |= (0x02 << GPIO_CRL_CNF6_Pos);   // Alternate Function Push-Pull
}


void TIM3_PWM_Config(uint32_t frequency, uint32_t duty_cycle) {
    if (frequency == 0 || duty_cycle == 0) {
        // Nếu frequency = 0, tắt kênh PWM và bộ đếm
        TIM3->CCER &= ~TIM_CCER_CC1E; // Vô hiệu hóa Channel 1
        TIM3->CR1 &= ~TIM_CR1_CEN;    // Tắt Counter
        return;
    }

    uint32_t prescaler = 0;  // Đặt prescaler mặc định
    uint32_t arr = SystemCoreClock / frequency - 1;  // Tính giá trị ARR từ tần số mong muốn

    if (arr > 65535) {  // Nếu ARR vượt quá giá trị tối đa, tăng prescaler
        prescaler = (arr / 65536) + 1;  // Tính prescaler để giảm ARR
        arr = (SystemCoreClock / (prescaler + 1)) / frequency - 1;
    }

    TIM3->PSC = prescaler;    // Đặt giá trị prescaler
    TIM3->ARR = arr;          // Đặt giá trị ARR
    TIM3->CCR1 = (arr + 1) * duty_cycle / 100;  // Tính Duty Cycle

    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;  // Clear OC1M settings
    TIM3->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos);  // PWM Mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;  // Enable Preload

    TIM3->CCER |= TIM_CCER_CC1E;  // Enable Channel 1
    TIM3->CR1 |= TIM_CR1_ARPE;    // Enable Auto-Reload Preload
    TIM3->CR1 |= TIM_CR1_CEN;     // Enable Timer
}

