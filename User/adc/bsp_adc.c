#include "./adc/bsp_adc.h"
#include "./systick/bsp_SysTick.h"

void ADC1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable ADC1, GPIOA (Temp Sensor) and GPIOC (Potentiometer PC1) clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    // Configure PC1 as Analog Input (Potentiometer)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure ADC1
    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // Configure Temperature Sensor Channel (ADC1 Channel 16)
    // Max ADC clock is 14MHz. PCLK2 is 72MHz. 72/6 = 12MHz.
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    // Enable Internal Temperature Sensor
    ADC_TempSensorVrefintCmd(ENABLE);

    // Enable ADC1
    ADC_Cmd(ADC1, ENABLE);

    // Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

u16 Get_ADC_Value(u8 ch)
{
    // Set channel and sampling time
    // T_smp must be > 17.1 us for temp sensor. 
    // At 12MHz, 1 cycle = 0.083us. 239.5 cycles = ~20us.
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    
    return ADC_GetConversionValue(ADC1);
}

float Get_Internal_Temp(void)
{
    u32 ad_sum = 0;
    float ad_avg;
    float voltage;
    float temperature;
    u8 i;
    
    // Oversampling for stability (Average 100 samples)
    for (i = 0; i < 100; i++)
    {
        ad_sum += Get_ADC_Value(ADC_Channel_16);
    }
    // Use float division to preserve precision from oversampling
    ad_avg = (float)ad_sum / 100.0f;
    
    voltage = ad_avg * (3.3f / 4096.0f);
    
    // Formula: T = (V25 - Vsense) / Avg_Slope + 25
    // V25 = 1.43V, Avg_Slope = 4.3mV/C = 0.0043V/C
    temperature = (1.43f - voltage) / 0.0043f + 25.0f;
    
    return temperature;
}

float Get_Pot_Voltage(void)
{
    u16 ad_value;
    float voltage;
    
    ad_value = Get_ADC_Value(ADC_Channel_11); // PC1 is ADC1_IN11
    voltage = (float)ad_value * (3.3f / 4096.0f);
    
    return voltage;
}
