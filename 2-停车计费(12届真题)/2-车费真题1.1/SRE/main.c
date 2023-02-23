/*
1:构造一个停车场结构体：车类型；车编号；年; 月; 日; 时; 分; 秒 ; 可否停车
2:串口解析部分 
			接收数据  (0-3)->车类型 (5-8)->车编号 (10,11)->年 (12,13)->月 (14,15)->日 (16,17)->时 (18,19)->分 (20,21)->分

3：思路
		初始化停车场结构体8个"可否停车"都为1 表示每个位置都能停车，
		当串口接收到一串数据的时候 先进行健壮性判断，看看有无格式问题。
		然后先解析车的编号和类型，判断每个"不能停车"的车位中，有无该车编号和类型匹配的
		如果有的话就继续解析年月日时分秒，车辆类型等等 存储到临时变量中
		这时利用时间和费率计算停车费，然后拼接字符串输出给上位机return该函数；
		
		如果没有的话，就重新循环(这里可以优化)判断有无空车位，解析全部信息填入到结构体中存入数组，可否停车置为0

*/

/*******************************************************************************  
* 文件名称：USART2接收及发送实验 
* 实验目的：掌握STM32 USART的操作与配置方法
* 程序说明：1.通过串口调试工具显示连接PC机与串口调试软件，将波特率设定为9600
*           2.发送字符串"asdx",在LCD第7行显示asd,以'x'位结束标志
      现象：串口2 默认连接下载串口的，不需要额外的串口线，下载线充当串口线
			      选择对应的端口和波特率即可以实现串口通信
* 日期版本：2015-NUAA
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
  * @说明     主函数
  * @参数     None 
  * @返回值   None
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
	
	//LCD工作模式配置
	STM3210B_LCD_Init();
	
	
	initParking(parking);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White); 
  SysTick_Config(SystemCoreClock/1000);  //1ms中断一次
	
	USART_Config();//串口配置
	NVIC_Configuration();//中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //接收中断使能
	LCD_DisplayStringLine(Line1,"Receive:");
	//USART_SendString("Hello Word!");
	
  while(1)
	{		
		//收到数据
		if(RXOVER == 1)
		{
			//解析------(0-3)->车类型 (5-8)->车编号 (10,11)->年 (12,13)->月 (14,15)->日 (16,17)->时 (18,19)->分 (20,21)->分
			strcpy(type,substring(USART_RXBUF,0,4));
			strcpy(id,substring(USART_RXBUF,5,4));
			year = atoi(substring(USART_RXBUF,10,2));
			month = atoi(substring(USART_RXBUF,12,2));
			day = atoi(substring(USART_RXBUF,14,2));
			hour = atoi(substring(USART_RXBUF,16,2));
			minute = atoi(substring(USART_RXBUF,18,2));
			second = atoi(substring(USART_RXBUF,20,2));
			
			//初始化车和时间
			car = initCar(id,type);
			time = initTime(year,month,day,hour,minute,second);
			parkControl(parking,car,time);
			sum = goOut(parking,car,time,cnbrPrice,vnbrPrice);
			//车库中没找到这辆车 准备停车
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
			//存在该车 那么出库
			else
			{
				//发送
				sprintf(sendComputer,"%s:%s:%d:%.2f",type,id,parkTime,sum);
				USART_SendString(sendComputer);
			}
//      测试部分
//			sprintf(showDetail,"%s:%s:%d-%d-%d-%d-%d-%d",type,id,year,month,day,hour,minute,second);
//			USART_SendString(showDetail);
			for(i=0;i<28;i++)
			{
				USART_RXBUF[i] = 0;  //清空接收区
			}
			RXOVER = 0;
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//开中断
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
  * @说明     USART1相关GPIO和工作模式配置
  * @参数     None 
  * @返回值   None
  */
void USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//引脚时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//串口时钟 注意串口2的时钟在APB1下面

    //配置USART1 TX引脚工作模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //配置USART1 RX引脚工作模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //串口1工作模式配置
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
  * @说明     USART1字符串发送函数
  * @参数     str: 指向字符串的指针
  * @返回值   None
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
  * @说明     配置中断向量控制器
  * @参数     None
  * @返回值   None
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
  * @说明     延时函数
  * @参数     nTime: 延时时间
  * @返回值   None
  */
void Delay_Ms(uint32_t nTime)
{
    TimingDelay = nTime;
    while(TimingDelay != 0);
}
/******************************************END OF FILE*************************/