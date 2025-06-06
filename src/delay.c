#include "delay.h"
void Timer4_Delay_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Bật clock cho TIM4
    TIM4->PSC = (SystemCoreClock / 1000000) - 1;
    TIM4->CR1 |= TIM_CR1_OPM; // Chế độ one-pulse (tự dừng sau mỗi update)
}

void Delay_us(uint32_t us) {
    TIM4->ARR = us;              // Cài đặt giá trị auto-reload (thời gian delay)
    TIM4->CNT = 0;               // Reset counter
    TIM4->CR1 |= TIM_CR1_CEN;    // Bật TIM4
    while ((TIM4->SR & TIM_SR_UIF) == 0); // Chờ sự kiện update (UIF flag)
    TIM4->SR &= ~TIM_SR_UIF;     // Xóa cờ update
}

void Delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        Delay_us(1000); // Mỗi lần gọi Delay_us(1000) để tạo 1ms
    }
}