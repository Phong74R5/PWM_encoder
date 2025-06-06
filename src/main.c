#include "stm32f1xx.h"
#include "i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "stdio.h"
#include "pwm.h"
#include "encoder.h"
#include "delay.h"
#include "logo.h"
void SystemClock_Config(void)
{
    // Bật HSE (High-Speed External Oscillator)
    RCC->CR |= 1 << 16;
    while (!(RCC->CR & RCC_CR_HSERDY)); // Đợi HSE sẵn sàng

    // Cấu hình HSE làm nguồn cho PLL
    RCC->CFGR |= 1 << 16; // HSE làm nguồn PLL

    // Đặt Hệ số nhân PLL (PLL_MUL2)
    RCC->CFGR |= 0 << 18;

    // Bật PLL
    RCC->CR |= 1 << 24;
    while (!(RCC->CR & RCC_CR_PLLRDY)); // Đợi PLL sẵn sàng

    // Chọn PLL làm nguồn xung nhịp hệ thống (SYSCLK)

    RCC->CFGR |= 2 << 0;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // Đợi PLL được chọn làm SYSCLK



}

void WriteBackupRegister(uint32_t freq, uint32_t dc) {
    // Bật clock cho PWR và BKP
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;

    // Cho phép truy cập Backup Domain
    PWR->CR |= PWR_CR_DBP;

    // Ghi dữ liệu vào Backup Register
    uint16_t vh = (uint16_t)(freq / 1000);  // Giá trị High
    uint16_t vl = (uint16_t)(freq % 1000);  // Giá trị Low

    BKP->DR1 = vh;  // Ghi giá trị High vào DR1
    BKP->DR2 = vl;  // Ghi giá trị Low vào DR2
    BKP->DR3 = (uint16_t)(dc);


    // Khóa quyền truy cập Backup Domain (tùy chọn)
    PWR->CR &= ~PWR_CR_DBP;
}


void ReadBackupRegister(uint32_t *freq, uint32_t *dc) {
    // Bật clock cho PWR và BKP
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;

    // Cho phép truy cập Backup Domain
    PWR->CR |= PWR_CR_DBP;

    // Đọc dữ liệu từ Backup Register
    uint16_t vh = BKP->DR1;  // Đọc giá trị High từ DR1
    uint16_t vl = BKP->DR2;  // Đọc giá trị Low từ DR2
    uint16_t vdc = BKP->DR3;

    // Khóa quyền truy cập Backup Domain (tùy chọn)
    PWR->CR &= ~PWR_CR_DBP;

    // Kết hợp giá trị High và Low
    *freq = ((uint32_t)vh * 1000) + (uint32_t)vl;
    *dc = (uint32_t)(vdc);
}
int main(void) {
    //cấu hình
    SystemClock_Config();
    SystemCoreClockUpdate();
    Timer4_Delay_Init();
    TIM2_Encoder_Init();
    GPIO_Button_Init();
    TIM3_PWM_Init();
    NVIC_Button_Init();
    I2C1_Init();
    SSD1306_Init();
    //
    SSD1306_Clear();
    SSD1306_DrawImgBG(gImage_logo);
    SSD1306_DrawString(41, 57, "PHONG74R5", digital_font5x7);
    SSD1306_UpdateScreen();
    Delay_ms(500);
    SSD1306_Clear();
    int save_flag = 0;
    // Đọc giá trị Encoder_volume từ Backup Register
    ReadBackupRegister(&Encoder_volume, &Encoder_dc);
    
    int32_t preEV = Encoder_volume;
    int32_t preDC = Encoder_dc;
    while (1) {
        Update_Volume();

        char buffer[50];
        sprintf(buffer, "F : %9ldHz", Encoder_volume);
        SSD1306_DrawString8x16(0, 0, buffer, ssd1306xled_font8x16);

        sprintf(buffer, "S : %9ldx", Encoder_scale);
        SSD1306_DrawString8x16(0, 16, buffer, ssd1306xled_font8x16);

        sprintf(buffer, "DC: %9ld%%", Encoder_dc);
        SSD1306_DrawString8x16(0, 32, buffer, ssd1306xled_font8x16);
        SSD1306_DrawString(0,48,"-------------------------",digital_font5x7);
        if (preEV != Encoder_volume||mode == 3|| preDC != Encoder_dc) { // Chế độ Save
            SSD1306_DrawString(70, 57, (preEV != Encoder_volume || preDC != Encoder_dc)?"Save?":"Saved", ssd1306xled_font5x7);
            SSD1306_DrawString(105, 57, "Y", save_option ? ssd1306xled_font5x7 : digital_font5x7);
            SSD1306_DrawString(120, 57, "N", !save_option ? ssd1306xled_font5x7 : digital_font5x7);
            SSD1306_DrawString(110, 57, save_option?"<-":"->", digital_font5x7);
            save_flag = save_option;
        } else {
            SSD1306_DrawString(70,57,"           ",ssd1306xled_font5x7);
        }
        if (mode != 3){
            SSD1306_DrawString8x16(17, mode * 16, "<==", ssd1306xled_font8x16);
        }
        if(save_flag && mode2 == 0){
                WriteBackupRegister(Encoder_volume, Encoder_dc);
                preEV = Encoder_volume;
                preDC = Encoder_dc;
                save_flag = 0;
                save_option = 0;
            }
        const char *mode_str = mode2 ? "MODE: EDIT  " : "MODE: SELECT";
        SSD1306_DrawString(0, 57, mode_str, digital_font5x7);

        TIM3_PWM_Config(preEV, Encoder_dc);
        SSD1306_UpdateScreen();
    
    }
}
