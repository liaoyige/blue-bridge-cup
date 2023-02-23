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
#include "stm32f10x_adc.h"
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
void ADC_Config(void);
float Read_ADC(void);
void LED_Control(uint16_t LED,uint8_t LED_Status);
void LED_Init(void);
uint8_t Key_Scan(void);
//设置界面函数
void setting();
//渲染函数
void fflush();

/* Private functions ---------------------------------------------------------*/


u8 second = 55;
u8 minute = 59;
u8 hour = 23;
//读取adc
float vol = 0.0;
//这个变量用来读取GPIO的电平
//利用标志位取反来显示 OFF 或者 ON
//通过主循环不断读取引脚的电平状态即可。
char led[4] = "OFF";

char showADC[15];
char showK[15];
char showLED[15];
char showTime[24];
/**
  * @说明     主函数
  * @参数     None 
  * @返回值   None
 */
int main(void)
{   

	//????
	float k = 0.1;

  uint8_t key_temp;
	//LED_Init();
	Key_Init();  //按键接口初始化	
	//LCD工作模式配置
	STM3210B_LCD_Init();
	TIM_Config();
	ADC_Config();
	
	//拼接区
	sprintf(showADC,"V1:%.2lfV",vol);
	sprintf(showK,"K:%.2f",k);
	sprintf(showLED,"LED:%s",led);
	sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);

  SysTick_Config(SystemCoreClock/1000);  //1ms中断一次

	LCD_SetTextColor(Blue);
	LCD_SetBackColor(White);
	
  while(1)
	{
		LED_Control(GPIO_Pin_10,0);
		key_temp = Key_Scan();
		vol = Read_ADC();
		switch(key_temp)
		{
			case '1':
				setting();
				TIM_Cmd(TIM3, ENABLE);
				break;
		}
		fflush();
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
//ADC配置函数
void ADC_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//PB0-ADC channel 8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// ADC1 工作模式配置
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_13Cycles5);    

	ADC_Cmd(ADC1, ENABLE);   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
}

/**
  * @说明     读取ADC转换结果
  * @参数     none
  * @返回值   ADC_VALUE:ADC转换结果
  */

float Read_ADC(void)
{
	float ADC_VALUE;
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	Delay_Ms(5);
	ADC_VALUE = ADC_GetConversionValue(ADC1)*3.30/0xfff;
	
	return ADC_VALUE;
}
/**
  * @说明     LED 相关GPIO引脚工作模式及时钟源配置
  * @参数     None 
  * @返回值   None
  */
void LED_Init(void)
{
	
	//两种不同的操作都可以 一种是寄存器法 一种是库函数法 	
	RCC->APB2ENR |= 3<<4;
	GPIOD->CRL = 0x3<<2;
	GPIOC->CRH = 0x33333333;
	GPIOC->ODR = 0xff<<8;
}

/**
  * @说明     控制LED打开或关闭
  * @参数     Led: LED编号,GPIO_Pin_8到GPIO_Pin_15
  * @参数     Ledstatus: 0，关闭LED；1，打开LED
  * @返回值   None
  */
void LED_Control(uint16_t LED,uint8_t LED_Status)
{
	//两种不同的操作都可以 一种是寄存器法 一种是库函数法 		
	#if 0
    if(LED_Status == 0){
        GPIO_SetBits(GPIOC,LED);
        GPIO_SetBits(GPIOD,GPIO_Pin_2);
        GPIO_ResetBits(GPIOD,GPIO_Pin_2);  //状态锁存
    }        
    else
    {
        GPIO_ResetBits(GPIOC,LED);
        GPIO_SetBits(GPIOD,GPIO_Pin_2);
        GPIO_ResetBits(GPIOD,GPIO_Pin_2);  //状态锁存    
    }
		#endif
		if(LED_Status == 1)
			GPIOC->BRR |= LED;
		else
			GPIOC->BSRR |= LED;
			GPIOD->BRR |= 1<<2;
			GPIOD->BSRR|= 1<<2;
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

void fflush(){
		//重新渲染拼接
		sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
		sprintf(showADC,"V1:%.2lfV",vol);
		LCD_DisplayStringLine(Line1,showADC);
		LCD_DisplayStringLine(Line3,showK);
		LCD_DisplayStringLine(Line5,showLED);
		LCD_DisplayStringLine(Line7,showTime);
}
