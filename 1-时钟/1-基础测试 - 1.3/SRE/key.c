#include "key.h"


/**
  * @说明     按键接口初始化函数
  * @参数     none
  * @返回值   None
  */
void Key_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //B1、B2按键引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8; // 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //B3、B4按键引脚配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @说明     按键扫描函数
  * @参数     none
  * @返回值   None
  */
uint8_t Key_Scan(void)
{
	uint8_t key_value = 0xff;
	if(RB1 == 0){
		Delay_Ms(400);
		key_value = '1';
	}
	if(RB2 == 0){
		Delay_Ms(400);
		key_value = '2';
	}
	if(RB3 == 0){
		key_value = '3';
	}
	if(RB4 == 0){
		key_value = '4';
	}

	return key_value;
}
