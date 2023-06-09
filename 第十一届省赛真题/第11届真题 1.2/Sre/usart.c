#include "usart.h"



char USART_RXBUF[30];

uint8_t RXOVER = 0;
uint8_t RXCUNT = 0;


/**
  * @说明     USART1相关GPIO和工作模式配置
  * @参数     None 
  * @返回值   None
  */
void USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//引脚时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//串口时钟 注意串口2的时钟在APB1下面

    //配置USART1 TX引脚工作模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //配置USART1 RX引脚工作模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //串口1工作模式配置
    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
		
}

/**
  * @说明     USART1字符串发送函数
  * @参数     str: 指向字符串的指针
  * @返回值   None
  */
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

/**
  * @说明     配置中断向量控制器
  * @参数     None
  * @返回值   None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART2_IRQHandler(void) //串口2的接收中断
{
	uint8_t temp;
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
		{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);		
		temp = USART_ReceiveData(USART2);
		if(temp=='\n' || RXCUNT==10)
		{
			RXCUNT = 0;
			RXOVER = 1;  //接收完成标志位置位
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//关中断
		 }
		else
			{
			USART_RXBUF[RXCUNT] = temp;
			RXCUNT++;			
		  }
	}
}
