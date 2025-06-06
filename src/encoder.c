#include "encoder.h"

uint8_t mode = 3; // 0: Freq, 1: Scale, 2: Duty Cycle, 3: Save
uint8_t mode2 = 0; // 0: Select, 1: Edit
uint8_t save_option = 0; // 0: No, 1: Yes

uint32_t Encoder_volume = 0;              // Giá trị Encoder_volume khởi tạo
const uint32_t max_volume = 8000000;       // Giá trị Encoder_volume tối đa
const uint32_t min_volume = 0;             // Giá trị Encoder_volume tối thiểu
uint32_t Encoder_scale = 1;

const uint32_t max_dc = 100;
const uint32_t min_dc = 0;
uint32_t Encoder_dc = 50;

// Hàm cấu hình TIM2 cho chế độ Encoder
void TIM2_Encoder_Init(void) {
    // Bật Clock cho TIM2 và GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;    // Enable TIM2 clock
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;    // Enable GPIOA clock

    // Cấu hình GPIOA: PA0 và PA1 ở chế độ Input Floating
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_MODE1);  // Input mode
    GPIOA->CRL |= GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1;  // Floating input (CNF = 01)

    // Cấu hình TIM2 ở chế độ Encoder mode 3
    TIM2->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;     // Chế độ Encoder mode 3
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // CC1 và CC2 được nối tới TI1, TI2
    TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);    // Không đảo tín hiệu đầu vào

    // Cấu hình giá trị đếm
    TIM2->ARR = 0xFFFF;    // Giá trị đếm tối đa
    TIM2->CNT = 0x7FFF;    // Giá trị đếm khởi tạo ở giữa (giảm tràn)

    // Kích hoạt TIM2
    TIM2->CR1 |= TIM_CR1_CEN;
}

// Hàm cấu hình GPIO cho nút nhấn (SW)
void GPIO_Button_Init(void) {
    // Bật clock cho GPIOB và AFIO
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    // Cấu hình PB0 làm input pull-up
    GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0); // Reset cấu hình PB0
    GPIOB->CRL |= GPIO_CRL_CNF0_1;                  // Input pull-up/pull-down
    GPIOB->ODR |= GPIO_ODR_ODR0;                    // Kéo lên (Pull-up)

    // Cấu hình EXTI cho PB0
    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0;         // Chọn Port B cho EXTI0
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB;       // EXTI0 -> PB0

    EXTI->IMR |= EXTI_IMR_MR0;                      // Bật ngắt cho dòng 0 (PB0)
    EXTI->FTSR |= EXTI_FTSR_TR0;                    // Kích hoạt cạnh xuống (Falling Edge)
    EXTI->RTSR &= ~EXTI_RTSR_TR0;                   // Vô hiệu hóa cạnh lên
}

// Hàm cấu hình NVIC cho EXTI0 (nút nhấn)
void NVIC_Button_Init(void) {
    NVIC_EnableIRQ(EXTI0_IRQn);                     // Bật ngắt EXTI0
    NVIC_SetPriority(EXTI0_IRQn, 1);                // Đặt mức ưu tiên
}

// Trình xử lý ngắt cho EXTI Line0 (PB0)
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {                
        Delay_ms(500);
        if (EXTI->PR & EXTI_PR_PR0){
            mode2 = (mode2 == 1)?0:1;
            EXTI->PR |= EXTI_PR_PR0;  
        }
    }
}

void Update_Volume(void) {
    static int16_t prev_count = 0x7FFF;
    int16_t current_count = (int16_t)TIM2->CNT;
    int16_t delta = current_count - prev_count;

    if (mode2 == 0) {
        if (delta > 0 && mode < 3) {
            mode++;
        } else if (delta < 0 && mode > 0) {
            mode--;
        }
    } else {
        switch (mode) {
            case 0: // Chỉnh volume
                if (delta > 0 && (Encoder_volume / Encoder_scale) % 10 < 9 &&
                    Encoder_volume + Encoder_scale - 1 < max_volume) {
                    Encoder_volume += Encoder_scale;
                } else if (delta < 0 && (Encoder_volume / Encoder_scale) % 10 > 0 &&
                           Encoder_volume - Encoder_scale + 1 > min_volume) {
                    Encoder_volume -= Encoder_scale;
                }
                break;

            case 1: // Chỉnh scale
                if (delta > 0 && Encoder_scale < 1000000) {
                    Encoder_scale *= 10;
                } else if (delta < 0 && Encoder_scale > 1) {
                    Encoder_scale /= 10;
                }
                break;

            case 2: // Chỉnh Duty Cycle
                if (delta > 0 && Encoder_dc < max_dc) Encoder_dc ++;
                if (delta < 0 && Encoder_dc > min_dc) Encoder_dc --;
                break;

            case 3: // Chế độ Save
                if (delta > 0 || delta < 0) {
                    save_option = !save_option; // Chuyển đổi giữa Yes (1) và No (0)
                }
                break;

            default:
                break;
        }
    }

    prev_count = current_count;
}

