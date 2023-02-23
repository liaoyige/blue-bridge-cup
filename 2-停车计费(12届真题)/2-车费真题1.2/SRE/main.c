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
#include "stdio.h"
#include "parkControl.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

#define  parkSize  8
void NVIC_Configuration(void);
void Delay_Ms(uint32_t nTime);
void USART_Config(void);
void USART_SendString(int8_t *str);

void fflushData();
void fflushPara();
void fflushAll();
void show();

void parkControl(Parking *parking,Car car,Time time);
int analysis(char* data);

char* substring(char* ch,int pos,int length);

/**
  * @˵��     ������
  * @����     None 
  * @����ֵ   None
  */
	
	

extern uint8_t RXOVER;
extern int parkTime;
extern double sum;

uint32_t TimingDelay = 0;
char USART_RXBUF[50];

//�۸�
float cnbrPrice=3.5;
float vnbrPrice=2;

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
Parking parking[parkSize];
char id[5];
char type[5];
int year;
int month;
int day;
int hour;
int minute;
int second;
int remainParkSize = 8;
int cnbrSize = 0;
int vnbrSize = 0;

//����
int keyTemp;

int showFlag = 0;
int main(void)
{	


	//LCD����ģʽ����
	STM3210B_LCD_Init();
	
	Key_Init();
	initParking(parking);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White); 
  SysTick_Config(SystemCoreClock/1000);  //1ms�ж�һ��
	
	USART_Config();//��������
	NVIC_Configuration();//�ж�����
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�ʹ��

	fflushData();
  while(1)
	{		
		keyTemp=Key_Scan();
		//��λ��ʾ�ͷ������ý����л�
		switch(keyTemp)
		{
			case '1':
				show();
				break;
		}
		fflushData();
		//�յ�����
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
		}
	}
}
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

/**
  * @˵��     USART1���GPIO�͹���ģʽ����
  * @����     None 
  * @����ֵ   None
  */
void USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//����ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//����ʱ�� ע�⴮��2��ʱ����APB1����

    //����USART1 TX���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //����USART1 RX���Ź���ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //����1����ģʽ����
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
  * @˵��     USART1�ַ������ͺ���
  * @����     str: ָ���ַ�����ָ��
  * @����ֵ   None
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
  * @˵��     �����ж�����������
  * @����     None
  * @����ֵ   None
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

//ͣ������
void parkControl(Parking *parking,Car car,Time time)
{
	sum = goOut(parking,car,time,cnbrPrice,vnbrPrice);
	//������û�ҵ������� ׼��ͣ��
	if(sum==-1.0)
	{
		if(park(parking,car,time))
		{
				USART_SendString("Success parking car!\n");
		}
		else
		{
				USART_SendString("Error parking car!\n");
		}
	}
	//ͣ��ʱ������
	if(sum==-2)
	{
		USART_SendString("Error ParkTime\n");
	}
	//���ڸó�
	if(sum>=0)
	{
			//����
			sprintf(sendComputer,"%s:%s:%dhour:%.2f$\n",type,id,parkTime,sum);
			USART_SendString(sendComputer);
	}
	
}
//��������
int analysis(char* data)
{		int i;

		USART_SendString("receive1\n");
		strcpy(type,substring(data,0,4));
	//	USART_SendString("receive2\n");
	
		strcpy(id,substring(data,5,4));
		year = atoi(substring(data,10,2));
		month = atoi(substring(data,12,2));
		day = atoi(substring(data,14,2));
		hour = atoi(substring(data,16,2));
		minute = atoi(substring(data,18,2));
		second = atoi(substring(data,20,2));

		//��ս�����
		memset(USART_RXBUF,0,sizeof(USART_RXBUF));

		for(i=0;i<50;i++)
		{
			USART_RXBUF[i] = 0;  //��ս�����
		}

		if(strcmp(type,"VNBR")!=0 && strcmp(type,"CNBR")!=0)
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


//��Ⱦ����
void fflushData()
{
	//������Ⱦƴ��
	fflushAll();
	LCD_DisplayStringLine(Line1,"         Data");
	LCD_DisplayStringLine(Line3,showCNBR);
	LCD_DisplayStringLine(Line5,showVNBR);
	LCD_DisplayStringLine(Line7,showIDLE);
}
void fflushPara()
{
	fflushAll();
	LCD_DisplayStringLine(Line1,"         Para");
	LCD_DisplayStringLine(Line3,showCNBRprice);
	LCD_DisplayStringLine(Line5,showVNBRprice);
}
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
					LCD_Clear(White);
					return;
				case '2':
					vnbrPrice+=0.5;
					cnbrPrice+=0.5;
					fflushAll();
					fflushPara();
					break;	
				case '3':
					vnbrPrice-=0.5;
					cnbrPrice-=0.5;
					fflushPara();
					break;
		}
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
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//���ж�
		}
	}
}

void fflushAll()
{
	sprintf(showCNBR,"  CNBR:%d",cnbrSize);
	sprintf(showVNBR,"  VNBR:%d",vnbrSize);
	sprintf(showCNBRprice,"  CNBR:%.2f",cnbrPrice);
	sprintf(showVNBRprice,"  VNBR:%.2f",vnbrPrice);
	sprintf(showVNBR,"  VNBR:%d",vnbrSize);
	sprintf(showIDLE,"  IDLE:%d",remainParkSize);
}
