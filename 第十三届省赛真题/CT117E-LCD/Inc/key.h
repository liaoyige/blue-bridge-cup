#include "stm32f10x_gpio.h"
#include "delay.h"

#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)      //KEY1 A0
#define KEY2 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)	  //KEY2 A8
#define KEY3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)   	  //KEY3 B1
#define KEY4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)      //KEY4 B2


void KEY_Init();
uint8_t KEY_Scan(); //????