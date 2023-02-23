#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "adc.h"
#include "stdio.h"
#include "usart.h"
#include "stdlib.h"

extern char USART_RXBUF[10];
extern uint8_t RXOVER;

int TimingDelay;

char Height_Show[20];
char ADC_Show[20];
char Liquid_Level_Show[20];
char Threshold1_Show[20];
char Threshold2_Show[20];
char Threshold3_Show[20];

double Adc_Temp;
int Liquid_Height;
int Liquid_Level;
int Old_Level;

int Threshold1=30;
int Threshold2=50;
int Threshold3=70;

int High_Light_flag = 1;
int mode=1;
char key;

	
int Led1_Flag=1;
int Led2_Flag=0;
int Led3_Flag=0;
int Led3_Happen=0;
int Led2_Happen=0;


void Liquid_Level_Display();
void Parameter_Setup_Display(int High_Light_flag);
void Setting();
void Key3_Control(int flag);
void Key4_Control(int flag);
int analysis(char * data);
void Level_Control();
void Usart_Control();
void Led_Control();
void Level_Init();

void Init_Data();
void Save_Data();

int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	KEY_Init();
	ADC_Config();
	STM3210B_LCD_Init();
	LED_Init();
	i2c_init();
	USART_Config();//串口配置
	NVIC_Configuration();//中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //接收中断使能
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	LED_Control(LEDALL,0);
	Level_Init();
	Init_Data();
	while(1)
	{
		Setting();
		Level_Control();
		Usart_Control();
		Led_Control();
	}

}

void Level_Init()
{
	int i;
	//ADC第一次读的数据是错误的 第二次开始才可以，为了防误差干脆多取几次
	for(i=0 ;i<10;i++)
	{
		Adc_Temp = Get_Adc();
	}
	Liquid_Height=Adc_Temp*30.3;
	if(Liquid_Height>0 && Liquid_Height < Threshold1)
	{
		Old_Level=0;
		Liquid_Level=0;
	}
	else if(Liquid_Height > Threshold1 && Liquid_Height < Threshold2)
	{
		Old_Level=1;
		Liquid_Level=1;

	}
	else if(Liquid_Height > Threshold2 && Liquid_Height < Threshold3)
	{
		Old_Level=2;
		Liquid_Level=2;
	}
	else if( Liquid_Height >= Threshold3)
	{
		Old_Level=3;
		Liquid_Level=3;
	}
}

void Liquid_Level_Display()
{
	sprintf(Height_Show,"Height: %2dcm",Liquid_Height);
	sprintf(ADC_Show,"ADC: %.2fV",Adc_Temp);
	sprintf(Liquid_Level_Show,"Level: %d",Liquid_Level);
	LCD_SetBackColor(White);
	LCD_DisplayStringLine(Line2,"   Liquid Level");
	LCD_DisplayStringLine(Line4,Height_Show);
	LCD_DisplayStringLine(Line5,ADC_Show);
	LCD_DisplayStringLine(Line6,Liquid_Level_Show);
}

void Parameter_Setup_Display(int High_Light_flag)
{
	sprintf(Threshold1_Show,"Threshold1: %2d     ",Threshold1);
	sprintf(Threshold2_Show,"Threshold2: %2d     ",Threshold2);
	sprintf(Threshold3_Show,"Threshold3: %2d     ",Threshold3);
	if(High_Light_flag==1)
	{
		LCD_SetBackColor(White);
		LCD_DisplayStringLine(Line2,"   Parameter Setup");
		LCD_DisplayStringLine(Line5,Threshold2_Show);
		LCD_DisplayStringLine(Line6,Threshold3_Show);
		LCD_SetBackColor(Yellow);
		LCD_DisplayStringLine(Line4,Threshold1_Show);
	}
	else if(High_Light_flag==2)
	{
		LCD_SetBackColor(White);
		LCD_DisplayStringLine(Line2,"   Parameter Setup");
		LCD_DisplayStringLine(Line4,Threshold1_Show);
		LCD_DisplayStringLine(Line6,Threshold3_Show);
		LCD_SetBackColor(Yellow);
		LCD_DisplayStringLine(Line5,Threshold2_Show);
	}
	else if(High_Light_flag==3)
	{
		LCD_SetBackColor(White);
		LCD_DisplayStringLine(Line2,"   Parameter Setup");
		LCD_DisplayStringLine(Line5,Threshold2_Show);
		LCD_DisplayStringLine(Line4,Threshold1_Show);
		LCD_SetBackColor(Yellow);
		LCD_DisplayStringLine(Line6,Threshold3_Show);
	}

}
void Setting()
{
	Adc_Temp = Get_Adc();
	Liquid_Height=Adc_Temp*30.3;
	key = KEY_Scan();
	switch(key)
	{
		case '1':
			LCD_Clear(White);
			mode = !mode;
			break;
		case '2':
			High_Light_flag++;
			if(High_Light_flag>3) { High_Light_flag = 1; }
			break;			
		case '3':
			Key3_Control(High_Light_flag);
			break;
		case '4':
			Key4_Control(High_Light_flag);
			break;
	}
	switch(mode)
	{
		case 1:
			Liquid_Level_Display();
			break;		
		case 0:
			Parameter_Setup_Display(High_Light_flag);
			break;
	}
}

void Key3_Control(int flag)
{
  
	if(flag==1)
	{
		if(Threshold1+5>95)
		{
      Threshold1=95;
      Save_Data();
			return;
		}
		Threshold1+=5;
		Save_Data();
	}
	else if(flag==2)
	{
		if(Threshold2+5>95)
		{
			Threshold2=95;
      Save_Data();
			return;
		}
		Threshold2+=5;
		Save_Data();
	}
	else if(flag==3)
	{
		if(Threshold3+5>95)
		{
			Threshold3=95;
      Save_Data();
			return;
		}
		Threshold3+=5;
		Save_Data();
	}
}
void Key4_Control(int flag)
{
	if(flag==1)
	{
		if(Threshold1-5<0)
		{
			Threshold1=0;
			Save_Data();
			return;
		}
		Threshold1-=5;
		Save_Data();
	}
	else if(flag==2)
	{
		if(Threshold3-5<0)
		{
			Threshold3=0;
			Save_Data();
			return;
		}
		Threshold2-=5;
		Save_Data();
	}
	else if(flag==3)
	{
		if(Threshold3-5<0)
		{
			Threshold3=0;
			Save_Data();
			return;
		}
		Threshold3-=5;
		Save_Data();
	}
}

void Level_Control()
{

	char data[20];

	if(Liquid_Height>0 && Liquid_Height < Threshold1)
	{
		Liquid_Level=0;
		if(Old_Level>Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+D\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		Old_Level=0;
	}
	else if(Liquid_Height > Threshold1 && Liquid_Height < Threshold2)
	{
		Liquid_Level=1;
		if(Old_Level>Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+D\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		if(Old_Level<Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+U\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		Old_Level=1;
	}
	else if(Liquid_Height > Threshold2 && Liquid_Height < Threshold3)
	{
		Liquid_Level=2;
		if(Old_Level>Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+D\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		if(Old_Level<Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+U\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		Old_Level=2;
	}
	else if( Liquid_Height >= Threshold3)
	{
		Liquid_Level=3;
		if(Old_Level<Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+U\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
		Old_Level=3;
	}
}

void Usart_Control()
{
	int mode;
	char data[20];

	if(RXOVER==1)
	{
		RXOVER = 0;
		//解析数据
		mode = analysis(USART_RXBUF);
		memset(USART_RXBUF,'\0',sizeof(USART_RXBUF));
		USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);
		switch(mode)
		{
			case 1:
				sprintf(data,"C:H%d+L%d\r\n",Liquid_Height,Liquid_Level);
				USART_SendString(data);
				break;
			case 2:
				sprintf(data,"S:TL%d+TM%d+TH%d\r\n",Threshold1,Threshold2,Threshold3);
				USART_SendString(data);
   			break;
			default:
			  break;
		}
	}
}
int analysis(char * data)
{
	if(data[0]=='C')
	{
		Led3_Happen=1;
		return 1;
	}
	else if(data[0]=='S')
	{
		Led3_Happen=1;
		return 2;
	}
}
void Led_Control()
{
	LED_Control(LED1,Led1_Flag);
	LED_Control(LED3,Led3_Flag);
	LED_Control(LED2,Led2_Flag);
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
	Threshold1 = Read_Data(0x1f);
	Delay_Ms(2);
	Threshold2 = Read_Data(0x2f);
	Delay_Ms(2);
	Threshold3 = Read_Data(0x3f);
	Delay_Ms(2);
}
void Save_Data()
{
	Write_Data(0x1f,Threshold1);
	Delay_Ms(2);
	Write_Data(0x2f,Threshold2);
	Delay_Ms(2);
	Write_Data(0x3f,Threshold3);
	Delay_Ms(2);
}
