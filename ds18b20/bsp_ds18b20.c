/**
  ******************************************************************************
  * @file    bsp_ds18b20.c
  * @author  lxzhao
  * @version V1.0
  * @date    2025-09-xx
  * @brief   DS18B20¶ȴӦúӿ
  ******************************************************************************
  * @attention
  *
  * ʵƽ̨:  STM32 ָ   
  *
  ******************************************************************************
  */

#include "bsp_ds18b20.h"
#include "../systick/bsp_SysTick.h"

static void                           DS18B20_GPIO_Config                       ( void );
static void                           DS18B20_Mode_IPU                          ( void );
static void                           DS18B20_Mode_Out_PP                       ( void );
static void                           DS18B20_Rst                               ( void );
static uint8_t                        DS18B20_Presence                          ( void );
static uint8_t                        DS18B20_ReadBit                           ( void );
static uint8_t                        DS18B20_ReadByte                          ( void );
static void                           DS18B20_WriteByte                         ( uint8_t dat );
static void                           DS18B20_SkipRom                           ( void );
static void                           DS18B20_MatchRom                          ( void );

 /**
  * @brief  DS18B20 ʼ
  * @param  
  * @retval 
  */
uint8_t DS18B20_Init(void)
{
	DS18B20_GPIO_Config ();
	
	DS18B20_DQ_1;
	
	DS18B20_Rst();
	
	return DS18B20_Presence();  //⵽巵ֵΪ0δ⵽ֵΪ1
}


/*
 * DS18B20_GPIO_Config
 *   DS18B20õI/O
 *   
 *   
 */
static void DS18B20_GPIO_Config(void)
{		
	/*һGPIO_InitTypeDef͵Ľṹ*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*DS18B20_DQ_GPIO_PORTʱ*/
	DS18B20_DQ_SCK_APBxClock_FUN ( DS18B20_DQ_GPIO_CLK, ENABLE); 

	/*ѡҪƵDS18B20_DQ_GPIO_PORT*/															   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;	

	/*ģʽΪͨ*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*ÿ⺯ʼDS18B20_DQ_GPIO_PORT*/
  	GPIO_Init ( DS18B20_DQ_GPIO_PORT , &GPIO_InitStructure );
}

/*
 * DS18B20_Mode_IPU
 *   ʹDS18B20-DATAűΪģʽ
 *   
 *   
 */
static void DS18B20_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*ѡҪƵDS18B20_DQ_GPIO_PORT*/	
	  GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;

	   /*ģʽΪģʽ*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	

	  /*ÿ⺯ʼDS18B20_DQ_GPIO_PORT*/
	  GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}


/*
 * DS18B20_Mode_Out_PP
 *   ʹDS18B20-DATAűΪģʽ
 *   
 *   
 */
static void DS18B20_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;

	 	/*ѡҪƵDS18B20_DQ_GPIO_PORT*/															   
  	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;	

	/*ģʽΪͨ*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*ÿ⺯ʼDS18B20_DQ_GPIO_PORT*/
  	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}


/*
 *ӻ͸λ
 */
static void DS18B20_Rst(void)
{
	/* Ϊ */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DQ_0;
	
	/* ٲ480usĵ͵ƽλźţ480-960us  */
	Delay_us(750);
	
	/* ڲλźź轫ߣı߼ֲв̫һ£ע⣡ */
	/* Űֲе˵ر̣¶ȴΪ  */
	DS18B20_DQ_1;
	
	/* ӻյĸλźź󣬻15~60usһ */
	Delay_us(15);
}


/*
 * ӻصĴ壺˵иΪΪһ60~240usĵ͵ƽźţнжޣδжޣ
 * 0ɹ
 * 1ʧ
 */
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* Ϊ򸡿ɣ */
	DS18B20_Mode_IPU();
	
	/* ȴĵΪһ60~240usĵ͵ƽź 
	 * ûʱӻյĸλźź󣬻15~60usһ
	 * 15~60usδһ壬˵ûм⵽شֵΪ6080100һģ
	 */
	while( DS18B20_DQ_IN() && pulse_time<100 )
	{
		pulse_time++;
		Delay_us(1);
	}	
	/* 100us󣬴嶼ûе*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* 嵽Ҵڵʱ䲻ܳ240us */
	while( !DS18B20_DQ_IN() && pulse_time<240 )
	{
		pulse_time++;
		Delay_us(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}


/*
 * DS18B20ȡһbitϸʱɲοֲ
 */
static uint8_t DS18B20_ReadBit(void)
{
	uint8_t dat;
	
	/* 0Ͷ1ʱ>=60us */	
	DS18B20_Mode_Out_PP();
	/* ʱʼ >1us <15us ĵ͵ƽź */
	DS18B20_DQ_0;
	Delay_us(10);
	
	/* ó룬ͷߣⲿ轫 */
	DS18B20_Mode_IPU();
	//Delay_us(2);
	
	/*DS18B20ϵݴ15us */
	if( DS18B20_DQ_IN() == SET )
		dat = 1;
	else
		dat = 0;
	
	/* ʱοʱͼǰ15+45=60usҲʵѸòһЩ  */
	Delay_us(50);
	
	return dat;
}


/*
 * DS18B20һֽλ
 */
static uint8_t DS18B20_ReadByte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_ReadBit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}


/*
 * дһֽڵDS18B20λ
 */
static void DS18B20_WriteByte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* д0д1ʱҪ60us */
		if (testb)
		{			
			DS18B20_DQ_0;
			/* 1us < ʱ < 15us */
			Delay_us(8);
			
			DS18B20_DQ_1;
			Delay_us(58);
		}		
		else
		{			
			DS18B20_DQ_0;
			/* 60us < Tx 0 < 120us */
			Delay_us(70);
			
			DS18B20_DQ_1;			
			/* 1us < Trec(ָʱ) < */
			Delay_us(2);
		}
	}
}


 /**
  * @brief  ƥ DS18B20 ROM
  * @param  
  * @retval 
  */
static void DS18B20_SkipRom ( void )
{
	DS18B20_Rst();	   
	
	DS18B20_Presence();	 
	
	DS18B20_WriteByte(0XCC);		/*  ROM */
	
}


 /*
  * @brief  ִƥ DS18B20 ROM
  * @param  
  * @retval 
  */

/* Դֲ̫ˣκηжϣֱ߼׼ȷ   */

static void DS18B20_MatchRom ( void )
{
	DS18B20_Rst();	   
	
	DS18B20_Presence();	 
	
	DS18B20_WriteByte(0X55);		/* ƥ ROM */
	
}


/*
 * 洢¶16 λĴչĶƲʽ
 * 12λֱʱ5λ7λ4Сλ
 *
 *         |-------------------|-----С ֱ 1/(2^4)=0.0625----|
 * ֽ  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----λ0->  1->-------|----------------------|
 * ֽ  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * ¶ = λ +  + С*0.0625
 */
 /**
  * @brief  ƥ ROM »ȡ DS18B20 ¶ֵ 
  * @param  
  * @retval ¶ֵ
  */
float DS18B20_GetTemp_SkipRom ( void )
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	
	DS18B20_SkipRom ();
	DS18B20_WriteByte(0X44);				/* ʼת */
	
	
	DS18B20_SkipRom ();
  DS18B20_WriteByte(0XBE);				/* ¶ֵ */
	
	tplsb = DS18B20_ReadByte();		 
	tpmsb = DS18B20_ReadByte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ¶ */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
}


 /**
  * @brief  ƥ ROM »ȡ DS18B20 ¶ֵ 
  * @param  ds18b20_idڴ DS18B20 кŵ׵ַ
  * @retval 
  */
void DS18B20_ReadId ( uint8_t * ds18b20_id )
{
	uint8_t uc;

	DS18B20_WriteByte(0x33);       //ȡк
	
	for ( uc = 0; uc < 8; uc ++ )
	  ds18b20_id [ uc ] = DS18B20_ReadByte();
	
}


 /**
  * @brief  ƥ ROM »ȡ DS18B20 ¶ֵ 
  * @param  ds18b20_id DS18B20 кŵ׵ַ
  * @retval ¶ֵ
  */
float DS18B20_GetTemp_MatchRom ( uint8_t * ds18b20_id )
{
	uint8_t tpmsb, tplsb, i;
	short s_tem;
	float f_tem;
	
	
	DS18B20_MatchRom ();            //ƥROM޷ֵδؼ
	
  for(i=0;i<8;i++)
		DS18B20_WriteByte ( ds18b20_id [ i ] );		//ֻܽ64λϢͣƥжϣ
	
	DS18B20_WriteByte(0X44);				/* ʼת */

	
	DS18B20_MatchRom ();            //ƥROM
	
	for(i=0;i<8;i++)
		DS18B20_WriteByte ( ds18b20_id [ i ] );	
	
	DS18B20_WriteByte(0XBE);				/* ¶ֵ */
	
	tplsb = DS18B20_ReadByte();		 
	tpmsb = DS18B20_ReadByte(); 
	
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ¶ */
		f_tem = (~s_tem+1) * 0.0625;	 //ǰ
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	

}

/*************************************END OF FILE******************************/
