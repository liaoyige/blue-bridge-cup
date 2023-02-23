#include "pwm.h"
void PWM_IO_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2; //�����ź��������PA1 PA2 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void TIM_Config()
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	PWM_IO_Config();
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 1000;  //1KHz
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	//TIM3Ԥ��Ƶ����:1MHZ,APB1��Ƶϵ��2�����뵽TIM3ʱ��Ϊ36MHzx2 = 72MHz  
	TIM_PrescalerConfig(TIM2,71, TIM_PSCReloadMode_Immediate);		

	/* Channel 2 and 3 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;	
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	
//	TIM_OCInitStructure.TIM_Pulse=PA2_Compare;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	//ʹ��TIM3��ʱ������
	TIM_Cmd(TIM2, ENABLE);
	//ʹ��TIM3 PWM���ģʽ
	TIM_CtrlPWMOutputs(TIM2,ENABLE);
}


