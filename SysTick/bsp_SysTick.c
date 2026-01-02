/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick Delay (Polling Mode)
  ******************************************************************************
  */
  
#include "bsp_SysTick.h"
#include "misc.h"

static u8  fac_us=0;

/**
  * @brief  Initialize SysTick for Delay
  * @param  None
  * @retval None
  */
void SysTick_Init(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
    fac_us = SystemCoreClock / 8000000;
}

/**
  * @brief  Delay us
  * @param  nus: microseconds
  * @retval None
  */
void Delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do
    {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

void Delay_ms_Safe(u16 nms)
{
    u32 i;
    for(i=0; i<nms; i++) Delay_us(1000);
}

// Empty function for compatibility if needed
void TimingDelay_Decrement(void)
{
}
