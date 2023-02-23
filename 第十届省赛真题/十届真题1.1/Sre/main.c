#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "adc.h"
u32 TimingDelay = 0;
void setting();
void Delay_Ms(u32 nTime);
void Upate_Data();
void Fflush_Main();
void Fflush_Setting();
void Key4_Control(int flag);
void Key3_Control(int flag);
void DymaicShow(int flag);

char MaxVolDisplay[24];
char MinVolDisplay[24];
char UpperDisplay[24];
char LowerDisplay[24];
char StatusDisplay[24];
char VolDisplay[24];

char *Status[8]={"Normal","Upper  ","Lower  "};
uint16_t LED_Arr[9]={0,LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8};

double Vol = 3.22;
double MaxVol = 2.4;
double MinVOl = 1.2;
int UpperLED = 1;
int LowerLED = 2;
int Status_Flag = 0;
int main(void)
{
	char key;
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	ADC_Config();
	KEY_Init();
	STM3210B_LCD_Init();
	LED_Init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);

	Upate_Data();
	Fflush_Main();
	while(1)
	{
		//获取电压
		Vol=Get_Adc();
		//状态控制
		Status_Control();
		key = KEY_Scan();
		switch(key)
		{
			case '1':
				setting();
				break;
		}
		Fflush_Main();
	}

}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
//设置界面切换
void setting()
{
	char key;
	int flag=0;
	LCD_Clear(White);
	Fflush_Setting();
	
	while(1)
	{
		//高亮展示
		DymaicShow(flag);
		key = KEY_Scan();
		Vol=Get_Adc();
		Status_Control();
		switch(key)
		{
			case '1':
				LCD_Clear(White);
				LCD_SetBackColor(White);
				return;
			case '2':
				flag++;
				if(flag>=4){  flag=0;   }
				break;
			case '3':
				Key3_Control(flag);
				break;
			case '4':
				Key4_Control(flag);
				break;
		}

	}

}
//按键3逻辑控制
void Key3_Control(int flag)
{
	if(flag==0)
	{
		if(MaxVol+0.3<3.300000000000001)
		{
			MaxVol+=0.3;
			return;
		}
		MaxVol=3.3;
	}
	else if(flag==1)
	{
		if(MinVOl+0.3<3.300000000000001)
		{
			MinVOl+=0.3;
			return;
		}
		MinVOl=3.3;
	}
	else if(flag==2)
	{
			LED_Control(LEDALL,0);
		if(UpperLED==8)
		{
			UpperLED=8;
			return;
		}
		if(UpperLED+1==LowerLED)
		{
			if(UpperLED+1==8)
			{
				return;
			}
			UpperLED+=2;
			return;
		}
		UpperLED++;
	}
	else if(flag==3)
	{
			LED_Control(LEDALL,0);
		if(LowerLED==8)
		{
			LowerLED=8;
			return;
		}
		if(LowerLED+1==UpperLED)
		{
			if(LowerLED+1==8)
			{
				return;
			}
			LowerLED+=2;
			return;
		}
		LowerLED++;
	}
}
//按键4逻辑控制
void Key4_Control(int flag)
{
	if(flag==0)
	{
		//这是因为小数计算不精确
		if(MaxVol-0.3>=0.00000000000000001)
		{
			MaxVol-=0.3;
			return;
		}
		MaxVol=0.0;
		return;
	}
	else if(flag==1)
	{
		if(MinVOl-0.3>=0.00000000000000001)
		{
			MinVOl-=0.3;
			return;
		}
		MinVOl=0.0;
	}
	else if(flag==2)
	{
			LED_Control(LEDALL,0);
		if(UpperLED==1)
		{
			return;
		}
		if(UpperLED-1==LowerLED)
		{
			if(UpperLED-1==1)
			{
				return;
			}
			UpperLED-=2;
			return;
		}
		UpperLED--;
	}
	else if(flag==3)
	{
			LED_Control(LEDALL,0);
		if(LowerLED==1)
		{
			return;
		}
		if(LowerLED-1==UpperLED)
		{
			if(LowerLED-1==1)
			{
				return;
			}
			LowerLED-=2;
			return;
		}
		LowerLED--;
	}
}
//全部数据更新
void Upate_Data()
{
	sprintf(MaxVolDisplay," Max Volt:%.2f",MaxVol);
	sprintf(MinVolDisplay," Min Volt:%.2f",MinVOl);
	sprintf(UpperDisplay," Upper:LD%d",UpperLED);
	sprintf(LowerDisplay," Lower:LD%d",LowerLED);
	sprintf(VolDisplay,"  Volt:%.2f",Vol);
	sprintf(StatusDisplay,"  Status:%s",Status[Status_Flag]);
}
//主页面刷新
void Fflush_Main()
{
	Upate_Data();
	LCD_DisplayStringLine(Line1, "        Main");
	LCD_DisplayStringLine(Line4,VolDisplay);
	LCD_DisplayStringLine(Line5,StatusDisplay);
}
//设置页面刷新
void Fflush_Setting()
{
	Upate_Data();
	LCD_DisplayStringLine(Line1, "      Setting");
	LCD_DisplayStringLine(Line3,MaxVolDisplay);
	LCD_DisplayStringLine(Line4,MinVolDisplay);
	LCD_DisplayStringLine(Line5,UpperDisplay);
	LCD_DisplayStringLine(Line6,LowerDisplay);
}
//高光的动态展示
void DymaicShow(int flag)
{
	LCD_SetBackColor(White);
	if(flag==0)
	{
		LCD_DisplayStringLine(Line1, "      Setting");
		LCD_DisplayStringLine(Line4,MinVolDisplay);
		LCD_DisplayStringLine(Line5,UpperDisplay);
		LCD_DisplayStringLine(Line6,LowerDisplay);
		LCD_SetBackColor(Yellow);
		sprintf(MaxVolDisplay," Max Volt:%.2f    ",MaxVol);
		LCD_DisplayStringLine(Line3,MaxVolDisplay);
		return;
	}
	if(flag==1)
	{
		LCD_DisplayStringLine(Line1, "      Setting");
		LCD_DisplayStringLine(Line3,MaxVolDisplay);
		LCD_DisplayStringLine(Line5,UpperDisplay);
		LCD_DisplayStringLine(Line6,LowerDisplay);
		LCD_SetBackColor(Yellow);
		sprintf(MinVolDisplay," Min Volt:%.2f    ",MinVOl);
		LCD_DisplayStringLine(Line4,MinVolDisplay);
		return;
	}
	if(flag==2)
	{
		LCD_DisplayStringLine(Line1, "      Setting");
		LCD_DisplayStringLine(Line3,MaxVolDisplay);
		LCD_DisplayStringLine(Line4,MinVolDisplay);
		LCD_DisplayStringLine(Line6,LowerDisplay);
		LCD_SetBackColor(Yellow);
		sprintf(UpperDisplay," Upper:LD%d        ",UpperLED);
		LCD_DisplayStringLine(Line5,UpperDisplay);
		return;
	}
	if(flag==3)
	{
		LCD_DisplayStringLine(Line1, "      Setting");
		LCD_DisplayStringLine(Line3,MaxVolDisplay);
		LCD_DisplayStringLine(Line4,MinVolDisplay);
		LCD_DisplayStringLine(Line5,UpperDisplay);
		LCD_SetBackColor(Yellow);
		sprintf(LowerDisplay," Lower:LD%d        ",LowerLED);
		LCD_DisplayStringLine(Line6,LowerDisplay);
		return;
	}
}