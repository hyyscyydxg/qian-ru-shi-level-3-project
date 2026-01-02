#include "./key/bsp_key.h"
#include "./systick/bsp_SysTick.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable PWR and BKP clock for PC13 access if needed (though standard GPIO usually works)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    
    // Allow access to Backup domain
    PWR_BackupAccessCmd(ENABLE);
    // Disable Tamper pin to ensure PC13 is GPIO
    BKP_TamperPinCmd(DISABLE);

    RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY2_GPIO_CLK, ENABLE);

    // KEY1 - PA0 - Active High (Wakeup)
    GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull-down default
    GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);

    // KEY2 - PC13 - Active High (Modified to fix stuck issue)
    GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // Pull-down default
    GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);
}

// Call this function every ~10ms
uint8_t KEY_Scan(void)
{
    static uint8_t key_state = 0; // 0: None, 1: Pressed
    static uint16_t hold_timer = 0;
    
    uint8_t k1, k2;
    uint8_t current_key = 0;

    k1 = GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY1_GPIO_PIN); // 1 is press
    k2 = GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY2_GPIO_PIN); // 1 is press (Modified)
    
    if (k1 == 1) current_key = 1;
    else if (k2 == 1) current_key = 2;
    
    if (current_key != 0)
    {
        if (key_state == 0)
        {
            // First press
            key_state = 1;
            hold_timer = 0;
            return current_key;
        }
        else
        {
            // Holding
            hold_timer++;
            if (hold_timer > 50) // 50 * 10ms = 500ms wait before repeat
            {
                // Start repeating
                if (hold_timer % 5 == 0) // Repeat every 5 * 10ms = 50ms
                {
                    return current_key;
                }
            }
        }
    }
    else
    {
        key_state = 0;
        hold_timer = 0;
    }
    
    return 0;
}
