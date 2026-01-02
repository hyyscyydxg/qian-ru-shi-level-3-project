#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "stm32f10x.h"

#define BEEP_GPIO_PORT    GPIOA
#define BEEP_GPIO_PIN     GPIO_Pin_8
#define BEEP_GPIO_CLK     RCC_APB2Periph_GPIOA

#define BEEP_ON           GPIO_SetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
#define BEEP_OFF          GPIO_ResetBits(BEEP_GPIO_PORT, BEEP_GPIO_PIN)
#define BEEP_TOGGLE       {BEEP_GPIO_PORT->ODR ^= BEEP_GPIO_PIN;}

void BEEP_Init(void);

#endif
