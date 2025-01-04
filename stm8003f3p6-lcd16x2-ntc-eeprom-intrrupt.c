#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "stm8s_i2c.h"
#include "stm8s_tim1.h"
#include "stm8s_clk.h"
#include "stm8s_itc.h"

#define LCD_ADDR 0x4E  // آدرس ال‌سی‌دی در I2C
#define EEPROM_ADDR 0x4000  // آدرس ذخیره‌سازی دما در EEPROM
#define TEMP_PIN GPIOB, GPIO_PIN_3  // پایه ورودی دما

// تابع تاخیر به میلی‌ثانیه
void delay_ms(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        for (uint16_t j = 0; j < 800; j++) {
            __asm__("nop");
        }
    }
}

// تابع ارسال داده به ال‌سی‌دی از طریق I2C
void I2C_Write(uint8_t address, uint8_t data) {
    I2C->CR2 |= I2C_CR2_START;
    while (!(I2C->SR1 & I2C_SR1_SB));
    I2C->DR = address;
    while (!(I2C->SR1 & I2C_SR1_ADDR));
    (void)I2C->SR3;
    I2C->DR = data;
    while (!(I2C->SR1 & I2C_SR1_TXE));
    I2C->CR2 |= I2C_CR2_STOP;
}

// تابع ارسال دستورات به ال‌سی‌دی
void LCD_Command(uint8_t cmd) {
    I2C_Write(LCD_ADDR, cmd & 0xF0);
    I2C_Write(LCD_ADDR, (cmd << 4) & 0xF0);
}

// تابع راه‌اندازی ال‌سی‌دی
void LCD_Init() {
    delay_ms(20);
    LCD_Command(0x33);
    LCD_Command(0x32);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);
    delay_ms(2);
}

// تابع پاک کردن ال‌سی‌دی
void LCD_Clear() {
    LCD_Command(0x01);
    delay_ms(2);
}

// تنظیم مکان‌نما در ال‌سی‌دی
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0) ? 0x80 + col : 0xC0 + col;
    LCD_Command(addr);
}

// تابع چاپ متن در ال‌سی‌دی
void LCD_Print(char *str) {
    while (*str) {
        I2C_Write(LCD_ADDR, *str++);
    }
}

// تابع خواندن ولتاژ از ADC
uint16_t Read_ADC() {
    ADC1->CSR |= ADC1_CSR_CH1;  // انتخاب کانال
    ADC1->CR1 |= ADC1_CR1_ADON;  // روشن کردن ADC
    ADC1->CR1 |= ADC1_CR1_ADON;  // شروع تبدیل
    while (!(ADC1->CSR & ADC1_CSR_EOC));  // انتظار برای اتمام تبدیل
    uint16_t result = ADC1->DRH << 8 | ADC1->DRL;  // خواندن نتیجه
    ADC1->CSR &= ~ADC1_CSR_EOC;  // پاک کردن بیت اتمام تبدیل
    return result;
}

// تابع نوشتن در EEPROM
void Write_EEPROM(uint16_t address, uint8_t data) {
    FLASH->DUKR = 0xAE;
    FLASH->DUKR = 0x56;
    *((uint8_t*)(uint32_t)address) = data;
    while (!(FLASH->IAPSR & FLASH_IAPSR_EOP));
}

// تابع خواندن از EEPROM
uint8_t Read_EEPROM(uint16_t address) {
    return *((uint8_t*)(uint32_t)address);
}

// تنظیم وقفه
void EXTI_Config() {
    ITC->ISPR1 &= ~ITC_ISPR1_VECT3SPR;  // تنظیم اولویت وقفه
    ITC->ISPR1 |= ITC_ISPR1_VECT3SPR_1;
    GPIOB->CR2 |= GPIO_PIN_4;  // فعال‌سازی وقفه خارجی برای پایه PB4
    EXTI->CR1 |= EXTI_CR1_PBIS;  // پیکربندی وقفه برای پایه PB4
}

// تابع وقفه
INTERRUPT_HANDLER(EXTI_PortB_IRQHandler, 4) {
    GPIOB->ODR &= ~(1 << 3);  // خاموش کردن پین خروجی
    delay_ms(20000);  // تاخیر ۲۰ ثانیه
    EXTI->SR1 &= ~EXT