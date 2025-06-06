#ifndef PWM_H
#define PWM_H

#include "stm32f1xx.h" // Thay đổi nếu sử dụng dòng vi điều khiển khác

void TIM3_PWM_Init(void);
void TIM3_PWM_Config(uint32_t frequency, uint32_t duty_cycle);


#endif
