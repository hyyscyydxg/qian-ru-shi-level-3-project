#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"


void SysTick_Init(void);
void Delay_us(__IO u32 nTime);         // λ1us
void TimingDelay_Decrement(void);
void Delay_ms_Safe(u16 nms);

#define Delay_ms(x) Delay_ms_Safe(x)	 //λms


#endif /* __SYSTICK_H */
