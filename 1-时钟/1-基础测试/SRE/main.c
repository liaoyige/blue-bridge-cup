/*******************************************************************************  
* 文件名称：按键控制实验
* 实验目的：1.掌握按键扫描及去抖的基本方法
*           2.掌握STM32处理器 GPIO的操作配置方法
* 程序说明：1.使用程序前，确认按键相关引脚已经通过跳线正确连接
*           2.B1-PA0   B2-PA8   B3-PB1   B4-PB2
      现象：按下按键，LCD显示对应的按键按下
* 日期版本：2015-NUAA
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "lcd.h"
#include "misc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define RB1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define RB2	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)
#define RB3 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)
#define RB4 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TimingDelay = 0;
/* Private function prototypes -----------------------------------------------*/
void Delay_Ms(uint32_t nTime);
void Key_Init(void);
void NVIC_Configuration(void);
void TIM_Config(void);


//设置界面函数
void setting();


uint8_t Key_Scan(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @说明     主函数
  * @参数     None 
  * @返回值   None
  */
		u8 second = 55;
	u8 minute = 59;
	u8 hour = 23;
	
int main(void)
{   
	char adc[15];
	char showK[15];
	char showLED[15];
	char showTime[24];
	

	
	float vol = 1.84;
	float k = 0.1;
	char led[4] = "OFF";
  uint8_t key_temp;
	
	Key_Init();  //按键接口初始化
	TIM_Config();
	//拼接区
	sprintf(adc,"V1:%.2lfV",vol);
	sprintf(showK,"K:%.2f",k);
	sprintf(showLED,"LED:%s",led);
	sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
	
	//LCD工作模式配置
	STM3210B_LCD_Init();
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);
    
	LCD_ClearLine(Line0);
	LCD_ClearLine(Line1);
	LCD_ClearLine(Line2);
	LCD_ClearLine(Line3);
	LCD_ClearLine(Line4);
	
//	LCD_DisplayStringLine(Line1,adc);
//	LCD_DisplayStringLine(Line3,showK);
//	LCD_DisplayStringLine(Line5,showLED);
//	LCD_DisplayStringLine(Line7,showTime);

  SysTick_Config(SystemCoreClock/1000);  //1ms中断一次

	LCD_SetTextColor(Blue);
	LCD_SetBackColor(White);
	
    while(1){
		key_temp = Key_Scan();
		switch(key_temp)
		{
			case '1':
				setting();
				TIM_Cmd(TIM3, ENABLE);
				break;
		}
		//重新渲染拼接
		sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
		LCD_DisplayStringLine(Line1,adc);
		LCD_DisplayStringLine(Line3,showK);
		LCD_DisplayStringLine(Line5,showLED);
		LCD_DisplayStringLine(Line7,showTime);
		key_temp = 0;  //清除按键
	}         
}

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

/**
  * @说明     延时函数
  * @参数     nTime: 延时时间
  * @返回值   None
  */
void Delay_Ms(uint32_t nTime)
{
    TimingDelay = nTime;
    while(TimingDelay != 0);
}
void TIM_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//中断向量配置
	NVIC_Configuration();		
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 7200;
	TIM_TimeBaseStructure.TIM_Prescaler = 1000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//TIM3预分频设置:1MHZ,APB1分频系数2，TIM3时钟为36MHzx2 = 72MHz  
	TIM_PrescalerConfig(TIM3,7199, TIM_PSCReloadMode_Immediate);	
	
	//通用定时器TIM3中断配置
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @说明     中断向量配置函数
  * @参数     none
  * @返回值   None
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
//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
			second = second+1;
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
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

/******************************************END OF FILE*************************/
void setting()
{

	int flag = 0;
	u8 key_temp;
	char showTime[20];
	sprintf(showTime,"  %d-%d-%d",hour,minute,second);
	LCD_Clear(White);
	LCD_DisplayStringLine(Line2,"   Setting");
	LCD_DisplayStringLine(Line4,showTime);
	
	TIM_Cmd(TIM3, DISABLE);

	while(1)
	{
		key_temp = Key_Scan();
		switch(key_temp)
		{
			case '1':
				//每按一次flag+1
				flag++;
				if(flag==1){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line7,"hour");
				} 
				else if(flag == 2){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line7,"minute");
				}
				else if(flag == 3){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line7,"second");
				}
				else if(flag == 4){
					LCD_Clear(White);
					return;
				}
				break;
			case '2':
				if(flag==1){
					hour = hour+1;
					if(hour>24){
						hour = 0;
					}
				} 
				else if(flag == 2){
					minute = minute+1;
					if(minute >= 60){
						minute=0;
						hour = hour+1;
						if(hour>24){
							hour=1;
						}
					}
				}
				else if(flag == 3){
					second = second+1;
					if(second >= 60){
						second=0;
						minute = minute+1;
						if(minute>=60){
							minute  = 0;
							hour = hour + 1;
							if(hour>=24){
								hour = 1;
							}
						}
					}
				}
				else{
					LCD_Clear(White);
					return;
				}
				sprintf(showTime,"  %2d-%2d-%2d",hour,minute,second);
				break;
		}
		LCD_DisplayStringLine(Line4,showTime);
	}
}
