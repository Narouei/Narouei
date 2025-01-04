#include "stm8s.h"

// تعریف تاخیر به میلی‌ثانیه
void delay_ms(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        for (uint16_t j = 0; j < 800; j++) {
            __asm__("nop");
        }
    }
}

void main() {
    // تنظیم PB3 به عنوان خروجی
    GPIOB->DDR |= (1 << 3);
    GPIOB->CR1 |= (1 << 3);

    while (1) {
        // روشن کردن PB3
        GPIOB->ODR |= (1 << 3);
        delay_ms(500); // تاخیر 500 میلی‌ثانیه

        // خاموش کردن PB3
        GPIOB->ODR &= ~(1 << 3);
        delay_ms(500); // تاخیر 500 میلی‌ثانیه
    }
}