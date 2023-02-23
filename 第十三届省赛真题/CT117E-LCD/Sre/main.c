#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "stdio.h"
extern uint8_t RXOVER;
extern char USART_RXBUF[30];

char dispBuf[20];
//密码相关变量
int b1=1,b2=2,b3=3;
int password1 = 1;
int password2 = 2;
int password3 = 3;
int error_record = 0;
int error_flag = 0;
//LED相关变量
int led2_flag = 0;
//PWM相关变量
int frequency = 2000;
int duty = 10;
int PWM_Mode = 0;
//显示相关变量
int lcd_mode = 1;
int init_flag=1;

//显示函数
void LCD_Show_PSD();
void LCD_Show_STA();
void LCD_Show();
//按键函数
void KEY_Control();
//密码函数
void Input_Password(char flag);
void Commit_Password();
//LED函数
void Led_Control();
//串口数据处理函数
void Data_Process();
int Check_Usart_Data();
//PWM控制函数
void PWM_Control();


void TIM2_Init(int arr,int psr);
//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	
	STM3210B_LCD_Init();
	LED_Init();
	KEY_Init();
	USART_Config();
	NVIC_Configuration();
	TIM2_Init(999,71);
	LCD_Clear(White);
	LCD_SetTextColor(Black);
	LCD_SetBackColor(White);
	
	LED_Control(LEDALL,0);

	while(1)
	{	
		LCD_Show();
		KEY_Control();
		Led_Control();
		Data_Process();
		PWM_Control();
	}
}

void LCD_Show_PSD()
{
	LCD_DisplayStringLine(Line1,"         PSD");
	if(init_flag==1)
	{
		sprintf(dispBuf,"   B1:@     ");
		LCD_DisplayStringLine(Line3,dispBuf);
		sprintf(dispBuf,"   B2:@     ");
		LCD_DisplayStringLine(Line4,dispBuf);
		sprintf(dispBuf,"   B3:@     ");
		LCD_DisplayStringLine(Line5,dispBuf);
	}
	else
	{
		sprintf(dispBuf,"   B1:%d       ",b1);
		LCD_DisplayStringLine(Line3,dispBuf);
		sprintf(dispBuf,"   B2:%d       ",b2);
		LCD_DisplayStringLine(Line4,dispBuf);
		sprintf(dispBuf,"   B3:%d       ",b3);
		LCD_DisplayStringLine(Line5,dispBuf);
	}
}
void LCD_Show_STA()
{
	LCD_DisplayStringLine(Line1,"         PSD");
	
	sprintf(dispBuf,"   F:%4dHZ      ",frequency);
	LCD_DisplayStringLine(Line3,dispBuf);
	
	sprintf(dispBuf,"   D:%d%%    ",duty);
	LCD_DisplayStringLine(Line4,dispBuf);
}
void LCD_Show()
{
	if(lcd_mode==1)
	{
		LCD_Show_PSD();
	}
	if(lcd_mode==2)
	{
		LCD_Show_STA();
	}
}
void KEY_Control()
{
	char key = KEY_Scan();
	//如果键值为1，2，3并且处于输入密码界面
	if((key=='1' || key=='2' || key=='3')&&lcd_mode==1)
	{
		init_flag=0;
		Input_Password(key);
	}
	if(key=='4')
	{
		Commit_Password();
	}
}
void Input_Password(char flag)
{
	if(flag=='1'){
		b1++;
		if(b1>9){b1=0;}
	}
	if(flag=='2'){
		b2++;
		if(b2>9){b2=0;}
	}
	if(flag=='3'){
		b3++;
		if(b3>9){b3=0;}
	}
}
void Commit_Password()
{
	//如果密码正确
	if(b1 == password1 && b2 == password2 && b3 == password3)
	{	
		LED_Control(LED1,1);
		LCD_Clear(White);
		lcd_mode = 2;
		init_flag = 1;
		error_record = 0;
		error_flag = 0;
		TIM2_Init(500,71);
		TIM_SetCompare2(TIM2,50);
	}
	else
	{
		error_record++;
		if(error_record>=3)
		{
			error_flag = 1;
		}
	}
}

void Led_Control()
{
	if(lcd_mode==1)
	{
		LED_Control(LED1,0);
	}
	LED_Control(LED2,led2_flag);
}

void Data_Process()
{
	if(RXOVER==1)
	{
		if(Check_Usart_Data())
		{
			password1 = USART_RXBUF[4] - '0';
			password2 = USART_RXBUF[5] - '0';
			password3 = USART_RXBUF[6] - '0';
		}
		RXOVER = 0;
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	}
}
void PWM_Control()
{
	if(PWM_Mode==1)
	{
		TIM2_Init(999,71);
		TIM_SetCompare2(TIM2,500);
	}
	PWM_Mode = 0;
}
int Check_Usart_Data()
{
	if(USART_RXBUF[0] - '0' == password1 && 
		 USART_RXBUF[1] - '0' == password2 && 
		 USART_RXBUF[2] - '0' == password3 && 
		 USART_RXBUF[3] == '-')
	{
		return 1;
	}
	return 0;
}
void TIM2_Init(int psr,int arr)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psr;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 500;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}
