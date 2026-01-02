/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   LED, OLED, Internal Temp, Keys, Buzzer, USART Demo
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "bsp_led.h"
#include "./systick/bsp_SysTick.h"
#include "./adc/bsp_adc.h"
#include "./oled/oled.h"
#include "./beep/bsp_beep.h"
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{	
    float temperature;
    float alarm_threshold = 30.0f; // Default threshold
    uint8_t key;
    uint16_t time_counter = 0;
    
    /* Initialize SysTick */
    SysTick_Init();
    
    /* Initialize Peripherals */
    LED_GPIO_Config();
    BEEP_Init();
    USART1_Config();
    OLED_Init();
    ADC1_Init();
    KEY_Init();
    
    OLED_ShowString(0, 0, "STM32 Smart Temp", 16);
    OLED_ShowString(0, 2, "System Init...", 16);
    
    printf("System Initialized\n");
    Delay_ms(1000);
    OLED_Clear();
    
    // Static UI
    OLED_ShowString(0, 0, "Temp:", 16);
    OLED_ShowString(0, 2, "Thres:", 16);
    OLED_ShowString(96, 0, "C", 16);
    OLED_ShowString(96, 2, "C", 16);

    while (1)
    {
        // 1. Key Scanning (Fast Loop)
        key = KEY_Scan();
        if (key == 1) // Key1: Increase Threshold
        {
            if (alarm_threshold < 40.0f)
            {
                alarm_threshold += 1.0f;
            }
            
            // Update UI immediately for responsiveness
            OLED_ShowDecimal(48, 2, alarm_threshold, 2, 2, 16);
        }
        else if (key == 2) // Key2: Decrease Threshold
        {
            if (alarm_threshold > 20.0f)
            {
                alarm_threshold -= 1.0f;
            }
            
            // Update UI immediately for responsiveness
            OLED_ShowDecimal(48, 2, alarm_threshold, 2, 2, 16);
        }
        
        // Limit threshold (Safety)
        if (alarm_threshold > 40.0f) alarm_threshold = 40.0f;
        if (alarm_threshold < 20.0f) alarm_threshold = 20.0f;

        // 2. Periodic Tasks (Slow Loop ~200ms)
        if (++time_counter >= 20) 
        {
            time_counter = 0;
            
            // Read Sensor
            temperature = Get_Internal_Temp();
            
            // Display Data
            OLED_ShowDecimal(48, 0, temperature, 2, 2, 16);
            // Threshold is updated in key event, but refresh here to be safe
            OLED_ShowDecimal(48, 2, alarm_threshold, 2, 2, 16);
            
            // Alarm Logic
            if (temperature > alarm_threshold)
            {
                OLED_ShowString(0, 4, "Status: ALARM! ", 16);
                LED_RED;
                BEEP_ON;
            }
            else
            {
                OLED_ShowString(0, 4, "Status: Normal ", 16);
                LED_GREEN;
                BEEP_OFF;
            }
            
            // Serial Output
            printf("Temp: %.2f C, Threshold: %.2f C, Status: %s\n", 
                   temperature, alarm_threshold, (temperature > alarm_threshold) ? "ALARM" : "NORMAL");
        }
        
        Delay_ms(10); // Base time slice 10ms
    }
}
