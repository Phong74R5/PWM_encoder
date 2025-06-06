#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f1xx.h" // Thay đổi nếu sử dụng dòng vi điều khiển khác
#include "delay.h"
// Biến toàn cục
extern uint32_t Encoder_volume;  // Declare global variable
extern uint32_t Encoder_scale;  // Declare global variable
extern uint32_t Encoder_dc;
extern uint8_t mode;
extern uint8_t mode2;
extern uint8_t save_option;
void TIM2_Encoder_Init(void);
void GPIO_Button_Init(void);
void NVIC_Button_Init(void);
void EXTI0_IRQHandler(void);
void Update_Volume(void);


#endif
