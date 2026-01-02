#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"
#include <stdio.h>

void USART1_Config(void);
int fputc(int ch, FILE *f);

#endif
