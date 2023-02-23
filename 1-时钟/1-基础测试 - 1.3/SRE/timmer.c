#include "timmer.h"
extern u8 second,minute,hour;
void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//�ж���������
	NVIC_Configuration();		
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 7199;
	TIM_TimeBaseStructure.TIM_Prescaler = 1000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//TIM3Ԥ��Ƶ����:1MHZ,APB1��Ƶϵ��2��TIM3ʱ��Ϊ36MHzx2 = 72MHz  
	TIM_PrescalerConfig(TIM3,7199, TIM_PSCReloadMode_Immediate);	
	
	//ͨ�ö�ʱ��TIM3�ж�����
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @˵��     �ж��������ú���
  * @����     none
  * @����ֵ   None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
			second = second+1;
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			if(second>=60){
				second = 0;
				minute = minute + 1;
			}
			if(minute>=60){
				minute = 0;
				hour= hour + 1;
			}
			if(hour>24){
				hour = 1;
			}
		}
}