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
  * @说明     主函数
  * @参数     None 
  * @返回值   None
  */
	
	

extern uint8_t RXOVER;
extern int parkTime;
extern double sum;

uint32_t TimingDelay = 0;
char USART_RXBUF[50];

//价格
float cnbrPrice=3.5;
float vnbrPrice=2;

//展示
char showDetail[28];
char showCNBR[28];
char showVNBR[28];
char showCNBRprice[28];
char showVNBRprice[28];
char showIDLE[28];

//发送数据
char sendComputer[28];

//结构体
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

//按键
int keyTemp;

int showFlag = 0;
int main(void)
{	


	//LCD工作模式配置
	STM3210B_LCD_Init();
	
	Key_Init();
	initParking(parking);
	
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White); 
  SysTick_Config(SystemCoreClock/1000);  //1ms中断一次
	
	USART_Config();//串口配置
	NVIC_Configuration();//中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //接收中断使能

	fflushData();
  while(1)
	{		
		keyTemp=Key_Scan();
		//车位显示和费率设置界面切换
		switch(keyTemp)
		{
			case '1':
				show();
				break;
		}
		fflushData();
		//收到数据
		if(RXOVER == 1)
		{
			RXOVER = 0;
			//当且仅当数据格式正确的时候
			if(analysis(USART_RXBUF))
			{
				//初始化车和时间
				car = initCar(id,type);
				time = initTime(year,month,day,hour,minute,second);
				//流程控制
				parkControl(parking,car,time);
			}
			//如果是在显示车位界面那么需要实时刷新
			
			if(showFlag==0)
			{
				fflushData();
			}
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//开中断
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
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

//停车控制
void parkControl(Parking *parking,Car car,Time time)
{
	sum = goOut(parking,car,time,cnbrPrice,vnbrPrice);
	//车库中没找到这辆车 准备停车
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
	//停车时长错误
	if(sum==-2)
	{
		USART_SendString("Error ParkTime\n");
	}
	//存在该车
	if(sum>=0)
	{
			//发送
			sprintf(sendComputer,"%s:%s:%dhour:%.2f$\n",type,id,parkTime,sum);
			USART_SendString(sendComputer);
	}
	
}
//解析数据
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

		//清空接收区
		memset(USART_RXBUF,0,sizeof(USART_RXBUF));

		for(i=0;i<50;i++)
		{
			USART_RXBUF[i] = 0;  //清空接收区
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


//渲染函数
void fflushData()
{
	//重新渲染拼接
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
					//车位显示和费率设置界面切换
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
			//当且仅当数据格式正确的时候
			if(analysis(USART_RXBUF))
			{
				//初始化车和时间
				car = initCar(id,type);
				time = initTime(year,month,day,hour,minute,second);
				//流程控制
				parkControl(parking,car,time);
			}
			//如果是在显示车位界面那么需要实时刷新
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//开中断
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
