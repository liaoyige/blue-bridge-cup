#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "adc.h"
#include "iic.h"
#include "stdio.h"
#include "usart.h"

void Delay_Ms(u32 nTime);
u32 TimingDelay;

extern uint8_t RXOVER;
extern char USART_RXBUF[28];




int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	STM3210B_LCD_Init();
	KEY_Init();
	LED_Init();
	i2c_init();
	ADC_Config();
	TIM3_Init();
  TIM2_Init(71,999);
	
	USART_Config();
	NVIC_Configuration();
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Red);
	LED_Control(LEDALL,0);

	while(1)
	{
		 Receive_Data();
		 Key_Control();
	}
}


void Key_Control()
{
	char key = KEY_Scan();
	switch(key)
	{
		case '1':

			break;
		case '2':

			break;
		case '3':

			break;
		case '4':

			break;
	}
}

void Receive_Data()
{
	if(RXOVER == 1)
	{

	}
	RXOVER = 0;
	USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//¿ªÖÐ¶Ï
	}
}


void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

