/*
1:����һ��ͣ�����ṹ�壺�����ͣ�����ţ���; ��; ��; ʱ; ��; �� ; �ɷ�ͣ��
2:���ڽ������� 
			��������  (0-3)->������ (5-8)->����� (10,11)->�� (12,13)->�� (14,15)->�� (16,17)->ʱ (18,19)->�� (20,21)->��

3��˼·
		��ʼ��ͣ�����ṹ��8��"�ɷ�ͣ��"��Ϊ1 ��ʾÿ��λ�ö���ͣ����
		�����ڽ��յ�һ�����ݵ�ʱ�� �Ƚ��н�׳���жϣ��������޸�ʽ���⡣
		Ȼ���Ƚ������ı�ź����ͣ��ж�ÿ��"����ͣ��"�ĳ�λ�У����޸ó���ź�����ƥ���
		����еĻ��ͼ�������������ʱ���룬�������͵ȵ� �洢����ʱ������
		��ʱ����ʱ��ͷ��ʼ���ͣ���ѣ�Ȼ��ƴ���ַ����������λ��return�ú�����
		
		���û�еĻ���������ѭ��(��������Ż�)�ж����޿ճ�λ������ȫ����Ϣ���뵽�ṹ���д������飬�ɷ�ͣ����Ϊ0

*/

/*******************************************************************************  
* �ļ����ƣ�USART2���ռ�����ʵ�� 
* ʵ��Ŀ�ģ�����STM32 USART�Ĳ��������÷���
* ����˵����1.ͨ�����ڵ��Թ�����ʾ����PC���봮�ڵ�����������������趨Ϊ9600
*           2.�����ַ���"asdx",��LCD��7����ʾasd,��'x'λ������־
      ���󣺴���2 Ĭ���������ش��ڵģ�����Ҫ����Ĵ����ߣ������߳䵱������
			      ѡ���Ӧ�Ķ˿ںͲ����ʼ�����ʵ�ִ���ͨ��
* ���ڰ汾��2015-NUAA
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "stdio.h"
#include "parkControl.h"
#include "usart.h"
#include "pwm.h"
#include "led.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/



void Delay_Ms(uint32_t nTime);


void fflushData();
void fflushPara();
void updateAll();
void show();

void parkControl(Parking *parking,Car car,Time time);
int analysis(char* data);
void receiveData();
	
char* substring(char* ch,int pos,int length);

/**
  * @˵��     ������
  * @����     None 
  * @����ֵ   None
  */
	
	

extern uint8_t RXOVER;
extern int parkTime;
extern double sum;
extern char USART_RXBUF[50];

uint32_t TimingDelay = 0;

//չʾ
char showDetail[28];
char showCNBR[28];
char showVNBR[28];
char showCNBRprice[28];
char showVNBRprice[28];
char showIDLE[28];

//��������
char sendComputer[28];

//�ṹ��
Car car;
Time time;
Parking parking[PARKS_SIZE];
extern char id[5];
extern char type[5];
extern int year;
extern int month;
extern int day;
extern int hour;
extern int minute;
extern int second;
extern int remainParkSize;
extern int cnbrSize ;
extern int vnbrSize;
//�۸�
extern float cnbrPrice;
extern float vnbrPrice;
//��λ
extern int remainParkSize;
extern int cnbrSize ;
extern int vnbrSize ;
//����
int keyTemp;
int showFlag = 0;
int pwmFlag = 0;
int main(void)
{	


	//LCD����ģʽ����
	STM3210B_LCD_Init();
	TIM_Config();
	Key_Init();
	LED_Init();
	initParking(parking);
	
	LCD_Clear(White);
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black); 
  SysTick_Config(SystemCoreClock/1000);  //1ms�ж�һ��
	
	USART_Config();//��������
	NVIC_Configuration();//�ж�����
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�ʹ��
	LED_Control(LEDALL,0);
	LED_Control(LED1,1);
	fflushData();
  while(1)
	{		
		keyTemp = Key_Scan();
		//��λ��ʾ�ͷ������ý����л�
		switch(keyTemp)
		{
			case '1':
				showFlag=!showFlag;
				show();
				break;
			case '4':
				pwmFlag = !pwmFlag;
  		  TIM_Cmd(TIM3, pwmFlag);
				LED_Control(LED2,pwmFlag);
				break;
		}
		fflushData();
		//�յ����ݿ�ʼ�߼�����
		receiveData();
	}
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
/******************************************END OF FILE*************************/
//�и��ַ���
char* substring(char* ch,int pos,int length)  
{  
    char* des=ch;  
    char* subch;  
    int i;  
    des=des+pos;  
    for(i=0;i<length;i++)  
    {  
        subch[i]=*(des++);  
    }  
    subch[length]='\0';
    return subch; 
} 
//��������
int analysis(char* data)
{
		strcpy(type,substring(data,0,4));
		strcpy(id,substring(data,5,4));
		year = atoi(substring(data,10,2));
		month = atoi(substring(data,12,2));
		day = atoi(substring(data,14,2));
		hour = atoi(substring(data,16,2));
		minute = atoi(substring(data,18,2));
		second = atoi(substring(data,20,2));

		//��ս�����
		memset(USART_RXBUF,0,sizeof(USART_RXBUF));
		USART_SendString(data);
		
		if(!(strcmp(type,"VNBR")==0 || strcmp(type,"CNBR")==0)  )
		{
			USART_SendString("Error Type Format\n");
			return 0;
		}
		else if(month>12 || day>31 || hour>24 || minute>60 || second>60 )
		{
			USART_SendString("Error Time Format\n");
			memset(USART_RXBUF,0,sizeof(USART_RXBUF));
			return 0;
		}
}


//����ȫ������
void updateAll()
{
	sprintf(showCNBR,"  CNBR:%d",cnbrSize);
	sprintf(showVNBR,"  VNBR:%d",vnbrSize);
	sprintf(showCNBRprice,"  CNBR:%.2f",cnbrPrice);
	sprintf(showVNBRprice,"  VNBR:%.2f",vnbrPrice);
	sprintf(showVNBR,"  VNBR:%d",vnbrSize);
	sprintf(showIDLE,"  IDLE:%d",remainParkSize);
}
//��Ⱦ��λ����
void fflushData()
{
	//������Ⱦƴ��
	updateAll();
	LCD_DisplayStringLine(Line1,"         Data");
	LCD_DisplayStringLine(Line3,showCNBR);
	LCD_DisplayStringLine(Line5,showVNBR);
	LCD_DisplayStringLine(Line7,showIDLE);
}
//��Ⱦ�۸�����
void fflushPara()
{
	updateAll();
	LCD_DisplayStringLine(Line1,"         Para");
	LCD_DisplayStringLine(Line3,showCNBRprice);
	LCD_DisplayStringLine(Line5,showVNBRprice);
}
//������ʾ�л�
void show()
{
	LCD_Clear(White);
	fflushPara();
	while(1)
	{
		keyTemp=Key_Scan();
		switch(keyTemp)
		{
					//��λ��ʾ�ͷ������ý����л�
				case '1':
					showFlag=!showFlag;
					LCD_Clear(White);
					break;
				case '2':
					vnbrPrice+=0.5;
					cnbrPrice+=0.5;
					fflushPara();
					break;	
				case '3':
					vnbrPrice-=0.5;
					cnbrPrice-=0.5;
					fflushPara();
					break;
		}
		//�л������Ҳ����Ҫ�������ݽ��д��� ��������һ���Իᱻ�ƻ�
		receiveData();
	}
}
//�������ݿ�ʼ����
void receiveData()
{
		if(RXOVER == 1)
		{
			RXOVER = 0;
			//���ҽ������ݸ�ʽ��ȷ��ʱ��
			if(analysis(USART_RXBUF))
			{
				//��ʼ������ʱ��
				car = initCar(id,type);
				time = initTime(year,month,day,hour,minute,second);
				//���̿���
				parkControl(parking,car,time);
			}
			//���������ʾ��λ������ô��Ҫʵʱˢ��
			if(showFlag==0)
			{
				fflushData();
			}
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//���ж�
			return;
		}
		return;
}
