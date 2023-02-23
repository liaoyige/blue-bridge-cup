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
void NVIC_Configuration(void);
void Delay_Ms(uint32_t nTime);
void USART_Config(void);
void USART_SendString(int8_t *str);

char* substring(char* ch,int pos,int length);
/**
  * @˵��     ������
  * @����     None 
  * @����ֵ   None
  */
	
	

extern uint8_t RXOVER;
extern int parkTime;

uint32_t TimingDelay = 0;
uint8_t USART_RXBUF[20];

float cnbrPrice=3.5;
float vnbrPrice=2;
float sum;
char showDetail[28];
char sendComputer[28];
char id[8];
char type[8];
int year;
int month;
int day;
int hour;
int minute;
int second;
int main(void)
{	
	uint8_t i;
	Parking parking[8];
	Car car;
	Time time;
	
	//LCD����ģʽ����
	STM3210B_LCD_Init();
	
	
	initParking(parking);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White); 
  SysTick_Config(SystemCoreClock/1000);  //1ms�ж�һ��
	
	USART_Config();//��������
	NVIC_Configuration();//�ж�����
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //�����ж�ʹ��
	LCD_DisplayStringLine(Line1,"Receive:");
	//USART_SendString("Hello Word!");
	
  while(1)
	{		
		//�յ�����
		if(RXOVER == 1)
		{
			//����------(0-3)->������ (5-8)->����� (10,11)->�� (12,13)->�� (14,15)->�� (16,17)->ʱ (18,19)->�� (20,21)->��
			strcpy(type,substring(USART_RXBUF,0,4));
			strcpy(id,substring(USART_RXBUF,5,4));
			year = atoi(substring(USART_RXBUF,10,2));
			month = atoi(substring(USART_RXBUF,12,2));
			day = atoi(substring(USART_RXBUF,14,2));
			hour = atoi(substring(USART_RXBUF,16,2));
			minute = atoi(substring(USART_RXBUF,18,2));
			second = atoi(substring(USART_RXBUF,20,2));
			
			//��ʼ������ʱ��
			car = initCar(id,type);
			time = initTime(year,month,day,hour,minute,second);
			parkControl(parking,car,time);
			sum = goOut(parking,car,time,cnbrPrice,vnbrPrice);
			//������û�ҵ������� ׼��ͣ��
			if(sum==-1)
			{
				if(park(parking,car,time))
				{
						USART_SendString("Success parking car!");
				}
				else
				{
						USART_SendString("Error parking car!");
				}
			}
			//���ڸó� ��ô����
			else
			{
				//����
				sprintf(sendComputer,"%s:%s:%d:%.2f",type,id,parkTime,sum);
				USART_SendString(sendComputer);
			}
//      ���Բ���
//			sprintf(showDetail,"%s:%s:%d-%d-%d-%d-%d-%d",type,id,year,month,day,hour,minute,second);
//			USART_SendString(showDetail);
			for(i=0;i<28;i++)
			{
				USART_RXBUF[i] = 0;  //��ս�����
			}
			RXOVER = 0;
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//���ж�
		}
	}
}
char* substring(char* ch,int pos,int length)  
{  
    char* des=ch;  
    char* subch=(char*)calloc(sizeof(char),length+1);  
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
    USART_InitStructure.USART_BaudRate = 9600;
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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