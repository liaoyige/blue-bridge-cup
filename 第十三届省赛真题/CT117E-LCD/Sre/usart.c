#include "usart.h"

char USART_RXBUF[30];
uint8_t RXOVER = 0;
uint8_t RXCUNT = 0;
void USART_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE)
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART_SendString(int8_t *str)
{
	uint8_t index = 0;
	do
	{
		USART_SendData(USART2,str[index]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
		index++;
	}
	while(str[index] != 0); 
}

void USART2_IRQHandler(void)
{
	uint8_t temp;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
  {
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		temp = USART_ReceiveData(USART2);
		if(temp=='\n' || RXCUNT==10)
		{
			RXCUNT = 0;
			RXOVER = 1;
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
		}
		else
		{
			USART_RXBUF[RXCUNT] = temp;
			RXCUNT++;	
		}
 }
}