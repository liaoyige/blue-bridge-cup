#include "tim.h"
int PA1_SETTING_Frep = 999;
int PA1_PWM_SET = 500;
int PA2_PWM_SET = 500;
 void TIM2_Init()
{
 
}

//PA1：TIM2ch2开启输入捕获
//PA2: TIM2ch3开启输入捕获
void TIM3_Init()
{

}

float PA1_DutyCycle;
float PA1_Frequency;
float PA2_DutyCycle;
float PA2_Frequency;
uint16_t PA1Value;
uint16_t PA2Value;
void TIM3_IRQHandler(void)
{
  
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
 
  PA1Value = TIM_GetCapture2(TIM2);
	PA2Value = TIM_GetCapture3(TIM2);
	
  if (PA1Value != 0)
  {
    PA1_DutyCycle = (TIM_GetCapture2 (TIM2) * 100) / PA1Value;
    PA1_Frequency = SystemCoreClock / PA1Value/72;
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);

  }
	if(PA2Value != 0)
	{
    PA2_DutyCycle = (TIM_GetCapture3 (TIM2) * 100) / PA2Value;
    PA2_Frequency = SystemCoreClock / PA2Value/72;		
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
	}
	
	if(PA2Value == 0)
	{
		PA1_DutyCycle = 0;
		PA1_Frequency = 0;
	}
	if(PA1Value == 0)
	{
		PA2_DutyCycle = 0;
		PA2_Frequency = 0;
	}
	
}


