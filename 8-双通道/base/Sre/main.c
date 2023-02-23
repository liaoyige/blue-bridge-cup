#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "adc.h"
#include "stdio.h"
#include "usart.h"
#include "stdlib.h"
#include "tim.h"
#include "iic.h"
extern char USART_RXBUF[30];

extern float DutyCycle;
extern float Frequency;
extern uint8_t RXOVER;
extern int CollectFlag;

extern PA1_PWM_SET;
extern PA2_PWM_SET;

uint8_t Read_Data(uint8_t address);
void Write_Data(unsigned char address,unsigned char info);

int PA1_Frep = 1000;
int PA2_Frep = 1000;
int PA6_Multiple = 2;
int PA7_Multiple = 3;

//本地设定时为0 串口设定为1；
int setting_flag = 0;
//PA6通道为1 PA7通道为0
int current_channel = 1;
char disp[20];
int TimingDelay;

void Show_Gui();
void Key_Control();
void Control_Multi_Add(int mode);
void Control_Multi_Sub(int mode);
void Save_data();
void Init_Data();
void receiveData();
void Led_Control();
void Data_Analysis(char * data);

extern float PA1_DutyCycle;
extern float PA1_Frequency;
extern float PA2_DutyCycle;
extern float PA2_Frequency;

int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	ADC_Config();
	KEY_Init();
	STM3210B_LCD_Init();
	LED_Init();
	i2c_init();

//	TIM2_Init();
//	TIM3_Init();
	USART_Config();//串口配置
	NVIC_Configuration();//中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //接收中断使能
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	LED_Control(LEDALL,0);
	
	//Init_Data();
	while(1)
	{

		Show_Gui();
		Key_Control();
		receiveData();
		Led_Control();
	}

}
void Show_Gui()
{
	sprintf(disp,"Channel(1):%2d",PA1_Frep);
	LCD_DisplayStringLine(Line1,disp);
	sprintf(disp,"N(1):%2d",PA6_Multiple);
	LCD_DisplayStringLine(Line3,disp);
	sprintf(disp,"Channel(2):%2d",PA2_Frep);
	LCD_DisplayStringLine(Line5,disp);
	sprintf(disp,"N(2):%2d",PA7_Multiple);
	LCD_DisplayStringLine(Line7,disp);
}

void Key_Control()
{
	char key = KEY_Scan();
	switch(key){
		case '1':
			setting_flag = !setting_flag;	
			LED_Control(LED3,setting_flag);
			break;
		case '2':
			if(setting_flag==1)return;
			current_channel = !current_channel;
			break;
		case '3':
			if(setting_flag==1)return;
			Control_Multi_Add(current_channel);
			break;
		case '4':
			if(setting_flag==1)return;
			Control_Multi_Sub(current_channel);
			break;
	}
}

void Control_Multi_Add(int mode)
{
	if(mode==1)
	{
		if(PA6_Multiple == 10)return;
		PA6_Multiple+=1;
	}
	if(mode==0)
	{
		if(PA7_Multiple == 10)return;
		PA7_Multiple+=1;
	}
	Save_data();
}
void Control_Multi_Sub(int mode)
{
	if(mode==1)
	{
		if(PA6_Multiple == 1)return;
		PA6_Multiple-=1;
	}
	if(mode==0)
	{
		if(PA7_Multiple == 1)return;
		PA7_Multiple-=1;
	}
	Save_data();
}
void Save_data()
{
	Write_Data(0x1f,PA6_Multiple);
	Delay_Ms(10);
	Write_Data(0x2f,PA7_Multiple);
	Delay_Ms(10);
}
void Init_Data()
{
	PA6_Multiple = (int)Read_Data(0x1f);
	PA6_Multiple = (int)Read_Data(0x2f);
}

void receiveData()
{
		if(RXOVER == 1)
		{
			
			//do someting
			if(USART_RXBUF[0] == 'S' && USART_RXBUF[1] == 'E' && USART_RXBUF[2] == 'T' &&
			   USART_RXBUF[3] == ':' && USART_RXBUF[5] == ':')
			{
				if(USART_RXBUF[4]=='1' || USART_RXBUF[4]=='2')
				Data_Analysis(USART_RXBUF);
			}
			
			RXOVER = 0;
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//开中断
			return;
		}
		return;
}
void Data_Analysis(char data[15])
{
	int num;
	if(!(data[6] < '9' && data[6] > '1'))
	{
		return;
	}
	num = data[6] - '0';
	if(num==1 && data[7]-'0' == 0){
			num = 10;
	}
	if(num < 2)
	{
		return;
	}
	if(data[4] == '1'){
		PA6_Multiple = num;
	}
	if(data[4] == '2'){
		PA7_Multiple = num;
	}
}
void Led_Control()
{
		if(PA1_PWM_SET==0)
		{
			LED_Control(LED1,	0);
		}
		if(PA2_PWM_SET==0)
		{
			LED_Control(LED2,	0);
		}
		if(PA1_PWM_SET!=0)
		{
			LED_Control(LED1,	1);
		}
		if(PA2_PWM_SET!=0)
		{
			LED_Control(LED2,	1);
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


