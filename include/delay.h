#ifndef DELAY_H
#define DELAY_H
#include "stm32f1xx.h" 
/**
 * @brief Khởi tạo Timer4 để sử dụng cho các chức năng delay.
 *        Timer4 sẽ được cấu hình ở chế độ one-pulse với độ phân giải 1 µs.
 */
void Timer4_Delay_Init(void);

/**
 * @brief Tạo độ trễ theo micro giây.
 * 
 * @param us: Số micro giây cần delay.
 */
void Delay_us(uint32_t us);

/**
 * @brief Tạo độ trễ theo mili giây.
 * 
 * @param ms: Số mili giây cần delay.
 */
void Delay_ms(uint32_t ms);

#endif // DELAY_H
