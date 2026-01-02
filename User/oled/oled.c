#include "oled.h"
#include "oled_font.h"
#include "../systick/bsp_SysTick.h"

// Software I2C Macros
#define OLED_SCL_H GPIO_SetBits(OLED_I2C_GPIO_PORT, OLED_I2C_SCL_PIN)
#define OLED_SCL_L GPIO_ResetBits(OLED_I2C_GPIO_PORT, OLED_I2C_SCL_PIN)
#define OLED_SDA_H GPIO_SetBits(OLED_I2C_GPIO_PORT, OLED_I2C_SDA_PIN)
#define OLED_SDA_L GPIO_ResetBits(OLED_I2C_GPIO_PORT, OLED_I2C_SDA_PIN)

void OLED_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(OLED_I2C_GPIO_CLK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = OLED_I2C_SCL_PIN | OLED_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // Open Drain for I2C
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(OLED_I2C_GPIO_PORT, &GPIO_InitStructure);
    
    OLED_SCL_H;
    OLED_SDA_H;
}

void OLED_I2C_Start(void)
{
    OLED_SDA_H;
    OLED_SCL_H;
    Delay_us(1);
    OLED_SDA_L;
    Delay_us(1);
    OLED_SCL_L;
}

void OLED_I2C_Stop(void)
{
    OLED_SDA_L;
    OLED_SCL_H;
    Delay_us(1);
    OLED_SDA_H;
}

void OLED_I2C_WaitAck(void)
{
    OLED_SDA_H;
    OLED_SCL_H;
    Delay_us(1);
    OLED_SCL_L;
}

void OLED_I2C_WriteByte(uint8_t data)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if(data & 0x80)
            OLED_SDA_H;
        else
            OLED_SDA_L;
        Delay_us(1);
        OLED_SCL_H;
        Delay_us(1);
        OLED_SCL_L;
        data <<= 1;
    }
    OLED_I2C_WaitAck();
}

void OLED_WriteCmd(uint8_t cmd)
{
    OLED_I2C_Start();
    OLED_I2C_WriteByte(OLED_ADDRESS);
    OLED_I2C_WriteByte(0x00); // Co = 0, D/C = 0
    OLED_I2C_WriteByte(cmd);
    OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t data)
{
    OLED_I2C_Start();
    OLED_I2C_WriteByte(OLED_ADDRESS);
    OLED_I2C_WriteByte(0x40); // Co = 0, D/C = 1
    OLED_I2C_WriteByte(data);
    OLED_I2C_Stop();
}

void OLED_SetPos(uint8_t x, uint8_t y)
{
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd((x & 0x0f));
}

void OLED_Clear(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++)
    {
        OLED_WriteCmd(0xb0 + i);
        OLED_WriteCmd(0x00);
        OLED_WriteCmd(0x10);
        for(n = 0; n < 128; n++)
        {
            OLED_WriteData(0);
        }
    }
}

void OLED_Init(void)
{
    OLED_I2C_Init();
    Delay_ms(200);
    
    OLED_WriteCmd(0xAE); // Display Off
    OLED_WriteCmd(0x00); // Set Lower Column Start Address
    OLED_WriteCmd(0x10); // Set Higher Column Start Address
    OLED_WriteCmd(0x40); // Set Display Start Line
    OLED_WriteCmd(0x81); // Set Contrast Control
    OLED_WriteCmd(0xCF);
    OLED_WriteCmd(0xA1); // Set Segment Re-map
    OLED_WriteCmd(0xC8); // Set COM Output Scan Direction
    OLED_WriteCmd(0xA6); // Set Normal/Inverse Display
    OLED_WriteCmd(0xA8); // Set Multiplex Ratio
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xD3); // Set Display Offset
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); // Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xD9); // Set Pre-charge Period
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDA); // Set COM Pins Hardware Configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); // Set VCOMH Deselect Level
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x20); // Set Memory Addressing Mode
    OLED_WriteCmd(0x02);
    OLED_WriteCmd(0x8D); // Set Charge Pump
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xA4); // Entire Display On/Off
    OLED_WriteCmd(0xA6); // Set Normal/Inverse Display
    OLED_WriteCmd(0xAF); // Display On
    OLED_Clear();
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    unsigned char c = 0, i = 0;
    c = chr - ' ';
    if(x > 128 - 1)
    {
        x = 0;
        y = y + 2;
    }
    if(Char_Size == 16)
    {
        OLED_SetPos(x, y);
        for(i = 0; i < 8; i++)
            OLED_WriteData(F8X16[c * 16 + i]);
        OLED_SetPos(x, y + 1);
        for(i = 0; i < 8; i++)
            OLED_WriteData(F8X16[c * 16 + i + 8]);
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, char *chr, uint8_t Char_Size)
{
    unsigned char j = 0;
    while(chr[j] != '\0')
    {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if(x > 120)
        {
            x = 0;
            y += 2;
        }
        j++;
    }
}

uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            }
            else enshow = 1;
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}

void OLED_ShowDecimal(uint8_t x, uint8_t y, float num, uint8_t z_len, uint8_t f_len, uint8_t size2)
{
    uint32_t z_part = (uint32_t)num;
    uint32_t f_part = (uint32_t)((num - z_part) * oled_pow(10, f_len));
    int i;
    
    OLED_ShowNum(x, y, z_part, z_len, size2);
    OLED_ShowChar(x + (size2 / 2) * z_len, y, '.', size2);
    // Show fractional part with leading zeros if necessary
    for(i=0; i<f_len; i++) {
        uint32_t div = oled_pow(10, f_len - 1 - i);
        uint8_t digit = (f_part / div) % 10;
        OLED_ShowChar(x + (size2 / 2) * (z_len + 1 + i), y, digit + '0', size2);
    }
}
