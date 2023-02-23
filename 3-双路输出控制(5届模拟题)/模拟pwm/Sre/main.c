#include "stm32f10x.h"
#include "lcd.h"
#include "pwm.h"
#include "usart.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
u32 TimingDelay = 0;
extern char USART_RXBUF[28];
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

int time=0;
char key;
//Main Body
void Delay_Ms(u32 nTime);

void PWM_Usart();
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
void Analog_PA2_PWM();
int main(void)
{
	SysTick_Config(SystemCoreClock/10000);

	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	i2c_init();
	//这样才能在pwm初始化的时候就有波形
	//到时候要换成EEPROM读取
	Init_Data();
	Delay_Ms(200);

	TIM_Config();

	USART_Config();
	LED_Init();
	Key_Init();

	NVIC_Configuration();
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//关中断
	
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	PA1_Compare=1000*0.01*PA1_Duty_Cycle;

	TIM_SetCompare2(TIM2,PA1_Compare);


	while(1)
	{
		LCD_Show();
		Time_Control();
		Setting();
		PWM_Usart();
		Usart_Control();
		Analog_PA2_PWM();
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
	if(hour>24)
	{
		hour=1;
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
			PA2_Mode=!PA2_Mode;
			Channel=2;
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
	if(Channel==2)
	{
		if(mode==1)
		{
			PA2_Compare=1000*0.01*PA2_Duty_Cycle;
			TIM_SetCompare3(TIM2,PA2_Compare);
			Save_Data();
		}
		else{
			TIM_SetCompare3(TIM2,0);
		}
	}
}
void PWM_Usart()
{
//	if(PWM_Usart_Flag==1)
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, DISABLE);//串口时钟 注意串口2的时钟在APB1下面
//	}
//	else
//	{
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, DISABLE);	
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//串口时钟 注意串口2的时钟在APB1下面
//	}
}

void Usart_Control()
{
	if(RXOVER==1)
	{
		USART_SendString("receive");
	}
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
	PA2_Duty_Cycle = Read_Data(0x4f);
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
	Write_Data(0x4f,PA2_Duty_Cycle);
	Delay_Ms(2);
}
int flag1=1;
int flag2=1;
void Analog_PA2_PWM()
{
			//配置USART1 TX引脚工作模式
			if(flag1)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_2);
			}
			if(flag2)
			{
				GPIO_ResetBits(GPIOA,GPIO_Pin_2);
			}
}