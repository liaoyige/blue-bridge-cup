#include "stm32f10x.h"
#include "lcd.h"
#include "pwm.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
u32 TimingDelay = 0;
extern char USART_RXBUF[10];
extern uint8_t RXOVER ;

double PA1_Duty_Cycle;
double PA2_Duty_Cycle;

int Channel=1;
int PA1_Compare; //等于500*0.01*PA1_Duty_Cycle
int PA2_Compare;
int PA1_Mode=1;
int PA2_Mode=1;

int Command_Flag=0;
int PWM_Usart_Flag=0;
int Setting_Low_Flagl=0;
int hour=23;
int minute=59;
int second=50;

int Setting_Hour;
int Setting_Minute;
int Setting_Second;
int Setting_Channel;
int Setting_Low;

char PA1_Show[20];
char PA2_Show[20];
char Time_Show[20];
char Channel_Show[20];
char Command_Show[20];

char key;
//Main Body
void Delay_Ms(u32 nTime);


void Time_Control();
void LCD_Show();
void Setting();
void PWM_Control(int Channel,int mode);
void Mode_Control(int Channel,int mode);
void Usart_Control();
void Init_Data();
void Save_Data();
uint8_t Read_Data(uint8_t address);
void Write_Data(unsigned char address,unsigned char info);
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	i2c_init();
	//这样才能在pwm初始化的时候就有波形
	//到时候要换成EEPROM读取
	Init_Data();
	Delay_Ms(200);

		

	USART_Config();
	NVIC_Configuration();
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//关中断
	TIM_Config();

	LED_Init();
	Key_Init();
	
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	PA1_Compare=1000*0.01*PA1_Duty_Cycle;
	PA2_Compare=1000*0.01*PA2_Duty_Cycle;
	TIM_SetCompare2(TIM2,PA1_Compare);
	TIM_SetCompare3(TIM2,PA2_Compare);
	while(1)
	{
		LCD_Show();
		Time_Control();
		Setting();
		Usart_Control();
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

void LCD_Show()
{
	sprintf(PA1_Show,"PWM-PA1:%.0lf",PA1_Duty_Cycle);
	sprintf(PA2_Show,"PWM-PA2:%.0lf",PA2_Duty_Cycle);
	sprintf(Time_Show,"Time:%2d:%2d:%2d",hour,minute,second);
	sprintf(Channel_Show,"Channel:PA%d",Channel);
	if(Command_Flag==0)
	{
		
		sprintf(Command_Show,"        %s","None");
	}
	LCD_DisplayStringLine(Line2,PA1_Show);
	LCD_DisplayStringLine(Line3,PA2_Show);
	LCD_DisplayStringLine(Line4,Time_Show);
	LCD_DisplayStringLine(Line5,Channel_Show);
	LCD_DisplayStringLine(Line6,"Command:");
	LCD_DisplayStringLine(Line7,Command_Show);
}
void Time_Control()
{
	if(second>=60)
	{
		second=0;minute++;
	}
	if(minute>=60)
	{
		minute=0;hour++;
	}
	if(hour>=24)
	{
		hour=0;
	}
}
void Setting()
{
	key = Key_Scan();
	LED_Control(LED1,PA1_Mode);
	LED_Control(LED2,PA2_Mode);

	switch(key)
	{
		case '1':
			Channel=1;
			PA1_Mode=!PA1_Mode;
			Mode_Control(Channel,PA1_Mode);
			break;
		case '2':
			PWM_Control(Channel,PA1_Mode);
			break;
		case '3':
			Channel=2;
			PA2_Mode=!PA2_Mode;
			Mode_Control(Channel,PA2_Mode);
			break;
		case '4':
			PWM_Control(Channel,PA2_Mode);
			break;
		default:
			break;
	}
}
void PWM_Control(int Channel,int mode)
{
	
	if(Channel==1)
	{
		PA1_Duty_Cycle+=10;
		if(PA1_Duty_Cycle>100)
		{
			LCD_DisplayStringLine(Line2,"                          ");
			PA1_Duty_Cycle=10;
		}
		if(mode==1)
		{
			PA1_Compare=1000*0.01*PA1_Duty_Cycle;
			TIM_SetCompare2(TIM2,PA1_Compare);
		}
		Save_Data();
	}
	if(Channel==2)
	{
		PA2_Duty_Cycle+=10;
		if(PA2_Duty_Cycle>100)
		{
			LCD_DisplayStringLine(Line3,"                          ");
			PA2_Duty_Cycle=10;
		}
		if(mode==1)
		{
			PA2_Compare=1000*0.01*PA2_Duty_Cycle;
			TIM_SetCompare3(TIM2,PA2_Compare);
		}
		Save_Data();
	}
}
void Mode_Control(int Channel,int mode)
{
	
	if(Channel==1)
	{
		if(mode==1)
		{
			PA1_Compare=1000*0.01*PA1_Duty_Cycle;
			TIM_SetCompare2(TIM2,PA1_Compare);
			Save_Data();
		}
		else
		{	
			TIM_SetCompare2(TIM2,0);
		}
	}
	//虽然这个代码很阴间 但请不要改动 后果自负
	if(Channel==2)
	{
		if(mode==1)
		{
			USART_Cmd(USART2, DISABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);//串口时钟 注意串口2的时钟在APB1下面
			USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
			TIM_Config();
			PA2_Compare=1000*0.01*PA2_Duty_Cycle;
			TIM_SetCompare3(TIM2,PA2_Compare);
			Save_Data();
		}
		else
		{	
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//串口时钟 注意串口2的时钟在APB1下面
			USART_Cmd(USART2, ENABLE);
			USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
		}
	}
}


void Usart_Control()
{
	char hour1[4]={'\0'};
	char minute1[4]={'\0'};
	char second1[4]={'\0'};
	if(RXOVER==1)
	{
		Command_Flag=1;
		sprintf(Command_Show,"   %s",USART_RXBUF);
		LCD_DisplayStringLine(Line7,Command_Show);
		strncpy(hour1,USART_RXBUF,2);
		strncpy(minute1,USART_RXBUF+2,2);
		strncpy(second1,USART_RXBUF+6,2);
		
		Setting_Hour = atoi(hour1);
		Setting_Minute = atoi(minute1);
		Setting_Second = atoi(second1);
		Setting_Channel = USART_RXBUF[11] - '0';
		Setting_Low =  USART_RXBUF[13] - '0';
		RXOVER=0;
	}
	if(Setting_Hour==hour && Setting_Minute==minute && Setting_Second==second )
	{
		Setting_Low_Flagl=1;
	}
	if(Setting_Low_Flagl==1)
	{
		if(second-Setting_Second>5)
		{
			LCD_DisplayStringLine(Line7,"                     ");
			Command_Flag=0;
			if(Setting_Channel==1)
			{
				PA1_Mode = !PA1_Mode;
			}	
			Mode_Control(Setting_Channel,PA1_Mode);
			Setting_Low_Flagl=0;
		}
	}
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//关中断
	memset(USART_RXBUF,0,sizeof(USART_RXBUF));
}
uint8_t Read_Data(uint8_t address)
{
	unsigned char val;
	
	I2CStart(); 
	I2CSendByte(0xa0);
	I2CWaitAck(); 
	
	I2CSendByte(address);
	I2CWaitAck(); 
	
	I2CStart();
	I2CSendByte(0xa1); 
	I2CWaitAck();
	val = I2CReceiveByte(); 
	I2CWaitAck();
	I2CStop();
	
	return(val);
}

void Write_Data(unsigned char address,unsigned char info)
{
	I2CStart(); 
	I2CSendByte(0xa0); 
	I2CWaitAck(); 
	
	I2CSendByte(address);	
	I2CWaitAck(); 
	I2CSendByte(info); 
	I2CWaitAck(); 
	I2CStop();
}

void Init_Data()
{
	PA1_Compare = Read_Data(0x1f);
	Delay_Ms(2);
	PA2_Compare = Read_Data(0x2f);
	Delay_Ms(2);
	PA1_Duty_Cycle = Read_Data(0x3f);
	Delay_Ms(2);
	PA2_Duty_Cycle = Read_Data(0x4a);
	Delay_Ms(2);
}
void Save_Data()
{
	Write_Data(0x1f,PA1_Compare);
	Delay_Ms(2);
	Write_Data(0x2f,PA2_Compare);
	Delay_Ms(2);
	Write_Data(0x3f,PA1_Duty_Cycle);
	Delay_Ms(2);
	Write_Data(0x4a,PA2_Duty_Cycle);
	Delay_Ms(2);
}
