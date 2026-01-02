#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

// OLED I2C Address
#define OLED_ADDRESS 0x78

// I2C GPIO Configuration
#define OLED_I2C_GPIO_PORT      GPIOB
#define OLED_I2C_GPIO_CLK       RCC_APB2Periph_GPIOB
#define OLED_I2C_SCL_PIN        GPIO_Pin_6
#define OLED_I2C_SDA_PIN        GPIO_Pin_7

// Function Prototypes
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);
void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2);
void OLED_ShowDecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2);

#endif
