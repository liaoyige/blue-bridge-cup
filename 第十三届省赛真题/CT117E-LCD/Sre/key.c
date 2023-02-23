#include "key.h"



void KEY_Init()
{
	GPIO_InitTypeDef init;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	
	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8;
	GPIO_Init(GPIOA,&init);
	init.GPIO_Mode = GPIO_Mode_IPU;
	init.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_Init(GPIOB,&init);
}

uint8_t KEY_Scan()
{
	uint8_t key_value = 0;
	if(KEY1 == 0){
		Delay_Ms(200);
		key_value = '1';
	}
	if(KEY2 == 0){
		Delay_Ms(200);
		key_value = '2';
	}
	if(KEY3 == 0){
		Delay_Ms(200);
		key_value = '3';
	}
	if(KEY4 == 0){
		Delay_Ms(200);
		key_value = '4';
	}

	return key_value;
}