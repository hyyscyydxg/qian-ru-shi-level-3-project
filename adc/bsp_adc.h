#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f10x.h"

void ADC1_Init(void);
float Get_Internal_Temp(void);
float Get_Pot_Voltage(void);

#endif
