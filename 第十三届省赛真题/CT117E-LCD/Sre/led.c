#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,ENABLE);
	init.GPIO_Mode = GPIO_Mode_Out_PP;
	init.GPIO_Speed = GPIO_Speed_50MHz;
	init.GPIO_Pin = LED1 | LED2 | LED3 | LED4 | LED5 | LED6 | LED7 | LED8;
	GPIO_Init(GPIOC,&init);
	init.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD,&init);
}
void LED_Control(uint16_t LED,int LED_Status)
{
	if(LED_Status==1)
	{
		GPIO_ResetBits(GPIOC,LED);
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
	if(LED_Status==0)
	{
		GPIO_SetBits(GPIOC,LED);
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
	}
}