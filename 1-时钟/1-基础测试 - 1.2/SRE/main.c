/*******************************************************************************  
* �ļ����ƣ���������ʵ��
* ʵ��Ŀ�ģ�1.���հ���ɨ�輰ȥ���Ļ�������
*           2.����STM32������ GPIO�Ĳ������÷���
* ����˵����1.ʹ�ó���ǰ��ȷ�ϰ�����������Ѿ�ͨ��������ȷ����
*           2.B1-PA0   B2-PA8   B3-PB1   B4-PB2
      ���󣺰��°�����LCD��ʾ��Ӧ�İ�������
* ���ڰ汾��2015-NUAA
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
//���ý��溯��
void setting();
//��Ⱦ����
void fflush();

/* Private functions ---------------------------------------------------------*/


u8 second = 55;
u8 minute = 59;
u8 hour = 23;
//��ȡadc
float vol = 0.0;
//�������������ȡGPIO�ĵ�ƽ
//���ñ�־λȡ������ʾ OFF ���� ON
//ͨ����ѭ�����϶�ȡ���ŵĵ�ƽ״̬���ɡ�
char led[4] = "OFF";

char showADC[15];
char showK[15];
char showLED[15];
char showTime[24];
/**
  * @˵��     ������
  * @����     None 
  * @����ֵ   None
 */
int main(void)
{   

	//????
	float k = 0.1;

  uint8_t key_temp;
	//LED_Init();
	Key_Init();  //�����ӿڳ�ʼ��	
	//LCD����ģʽ����
	STM3210B_LCD_Init();
	TIM_Config();
	ADC_Config();
	
	//ƴ����
	sprintf(showADC,"V1:%.2lfV",vol);
	sprintf(showK,"K:%.2f",k);
	sprintf(showLED,"LED:%s",led);
	sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);

  SysTick_Config(SystemCoreClock/1000);  //1ms�ж�һ��

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
		key_temp = 0;  //�������
	}         
}

/**
  * @˵��     �����ӿڳ�ʼ������
  * @����     none
  * @����ֵ   None
  */
void Key_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //B1��B2������������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8; // 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //B3��B4������������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @˵��     ����ɨ�躯��
  * @����     none
  * @����ֵ   None
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
  * @˵��     ��ʱ����
  * @����     nTime: ��ʱʱ��
  * @����ֵ   None
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
	
	//�ж���������
	NVIC_Configuration();		
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 7200;
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
//ADC���ú���
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
	
	// ADC1 ����ģʽ����
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  //����ת��
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
  * @˵��     ��ȡADCת�����
  * @����     none
  * @����ֵ   ADC_VALUE:ADCת�����
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
  * @˵��     LED ���GPIO���Ź���ģʽ��ʱ��Դ����
  * @����     None 
  * @����ֵ   None
  */
void LED_Init(void)
{
	
	//���ֲ�ͬ�Ĳ��������� һ���ǼĴ����� һ���ǿ⺯���� 	
	RCC->APB2ENR |= 3<<4;
	GPIOD->CRL = 0x3<<2;
	GPIOC->CRH = 0x33333333;
	GPIOC->ODR = 0xff<<8;
}

/**
  * @˵��     ����LED�򿪻�ر�
  * @����     Led: LED���,GPIO_Pin_8��GPIO_Pin_15
  * @����     Ledstatus: 0���ر�LED��1����LED
  * @����ֵ   None
  */
void LED_Control(uint16_t LED,uint8_t LED_Status)
{
	//���ֲ�ͬ�Ĳ��������� һ���ǼĴ����� һ���ǿ⺯���� 		
	#if 0
    if(LED_Status == 0){
        GPIO_SetBits(GPIOC,LED);
        GPIO_SetBits(GPIOD,GPIO_Pin_2);
        GPIO_ResetBits(GPIOD,GPIO_Pin_2);  //״̬����
    }        
    else
    {
        GPIO_ResetBits(GPIOC,LED);
        GPIO_SetBits(GPIOD,GPIO_Pin_2);
        GPIO_ResetBits(GPIOD,GPIO_Pin_2);  //״̬����    
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
				//ÿ��һ��flag+1
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
		//������Ⱦƴ��
		sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
		sprintf(showADC,"V1:%.2lfV",vol);
		LCD_DisplayStringLine(Line1,showADC);
		LCD_DisplayStringLine(Line3,showK);
		LCD_DisplayStringLine(Line5,showLED);
		LCD_DisplayStringLine(Line7,showTime);
}
