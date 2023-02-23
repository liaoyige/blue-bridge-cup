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


#include "lcd.h"
#include "adc.h"
#include "timmer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t TimingDelay = 0;
/* Private function prototypes -----------------------------------------------*/
void Delay_Ms(uint32_t nTime);

void LED_Control(uint16_t LED,uint8_t LED_Status);
void LED_Init(void);

//���ý��溯��
void setting();
//ˢ����Ⱦ����
void fflush();


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
	float k = 0.1;
	//����״̬
  uint8_t key_temp;
	
	//��ʼ��--------------------------------------------------------------
	Key_Init(); 
	STM3210B_LCD_Init();
	TIM_Config();
	ADC_Config();
	//------------------------------------------------------------------------
	
	
	//ƴ����------------------------------------------------------------------
	sprintf(showADC,"V1:%.2lfV",vol);
	sprintf(showK,"K:%.2f",k);
	sprintf(showLED,"LED:%s",led);
	sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second);
	//-------------------------------------------------------------------------
	
	
	//LCD����----------------------------------------------------------------
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);
	//-------------------------------------------------------------------------
	
  SysTick_Config(SystemCoreClock/1000);  //1ms�ж�һ��

	//ʱ���ʼ��
	second = 55;
	minute = 59;
	hour = 23;
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


void Delay_Ms(uint32_t nTime)
{
    TimingDelay = nTime;
    while(TimingDelay != 0);
}


void LED_Init(void)
{
	
	//���ֲ�ͬ�Ĳ��������� һ���ǼĴ����� һ���ǿ⺯���� 	
	RCC->APB2ENR |= 3<<4;
	GPIOD->CRL = 0x3<<2;
	GPIOC->CRH = 0x33333333;
	GPIOC->ODR = 0xff<<8;
}


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
//���ú���
void setting()
{

	int flag = 0;
	u8 key_temp;
	char showTime[20];	
	
	LCD_Clear(White);
	
	//��ʾ��Ⱦ
		sprintf(showTime,"  %d-%d-%d",hour,minute,second);
	LCD_DisplayStringLine(Line2,"   Setting");
	LCD_DisplayStringLine(Line4,showTime);
	
	//���õ�ʱ��ʧ�ܶ�ʱ��
	TIM_Cmd(TIM3, DISABLE);

	while(1)
	{
		key_temp = Key_Scan();
		switch(key_temp)
		{
			case '1':
				flag++;
				if(flag==1){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line6,"  hour");
				} 
				else if(flag == 2){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line6,"  minute");
				}
				else if(flag == 3){
					LCD_ClearLine(Line7);
					LCD_DisplayStringLine(Line6,"  second");
				}
				else if(flag == 4){
					LCD_Clear(White);
					return;
				}
				break;
			case '2':
				if(flag==0){}
				//hour
				else if(flag==1){
					hour++;
					if(hour>24){
						hour = 0;
					}
				} 
				//minute
				else if(flag == 2){
					minute++;
					if(minute >= 60){
						minute=0;
						hour++;
						if(hour>24){
							hour=1;
						}
					}
				}
				//second
				else if(flag == 3){
					second++;
					if(second >= 60){
						second=0;
						minute++;
						if(minute>=60){
							minute  = 0;
							hour++;
							if(hour>=24){
								hour = 1;
							}
						}
					}
				}
				//exit
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

//��Ⱦ����
void fflush(){
		//������Ⱦƴ��
		sprintf(showTime,"T:%2d-%2d-%2d",hour,minute,second,showTime);
		sprintf(showADC,"V1:%.2lfV",vol);
		LCD_DisplayStringLine(Line1,showADC);
		LCD_DisplayStringLine(Line3,showK);
		LCD_DisplayStringLine(Line5,showLED);
		LCD_DisplayStringLine(Line7,showTime);
}
