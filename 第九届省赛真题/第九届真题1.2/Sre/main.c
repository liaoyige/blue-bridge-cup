#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "iic.h"
u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

int hour = 0;
int minute = 1;
int second = 5;
int storage_flag=1;

char *status[4] = {"Running","Pause","Standby","Setting"};
int status_flag = 2;
int pause_start_flag = 1;
int is_standby = 1;
int is_setting = 0;
int is_start = 0;
int is_pause = 0;
int alarm_flag;
int led1_flag;
char NO_Disp[26];
char Time_Disp[26];
char Status_Disp[26];

int button2_delay=0;
int button3_delay=0;
int button4_delay=0;
int set_time_mode=0;

void Countdown_Display();
void Countdown_Time_Control();
void Key_Control();
void Setting();

void Draw_Under_line(int mode);
void Time_Setting(int mode);
void Timer_State_Control();
void Led_Control();
void Status_Control(int status_flag);
uint8_t Read_Data(uint8_t address);
void Write_Data(unsigned char address,unsigned char info);
void Read_Time(int storage_flag);
void Storage_Time(int storage_flag);
//Main Body
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);

	Delay_Ms(200);
	STM3210B_LCD_Init();
	KEY_Init();
	LED_Init();
	i2c_init();
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	LED_Control(LEDALL,0);
	Read_Time(storage_flag);
	while(1)
	{
		Led_Control();
		Key_Control();
		Countdown_Display();
		Countdown_Time_Control();
	}
}


void Countdown_Display()
{
	sprintf(NO_Disp," NO %d",storage_flag);
	sprintf(Time_Disp,"     %2d:%2d:%2d",hour,minute,second);
	sprintf(Status_Disp,"       %s    ",status[status_flag]);
	LCD_DisplayStringLine(Line2,NO_Disp);
	LCD_DisplayStringLine(Line4,Time_Disp);
	LCD_DisplayStringLine(Line7,Status_Disp);
}

void Countdown_Time_Control()
{
	if(is_setting==1)
	{
		if(hour>24){hour=0;}
		if(minute>60) {minute=0;}
		if(second>60) {minute=0;}
	}
	if(is_setting==0)
	{
		if(second==0)
		{
			if(minute>0)
			{
				minute-=1;
				second = 60;
			}
			else
			{
				second=0;
			}
		}
		if(minute==0)
		{
			if(hour>1)
			{
				hour-=1;
				minute = 60;
			}
			else
			{
				minute=0;
			}
		}
	}
	if(minute==0 && second == 0 && hour==0)
	{
		alarm_flag = 1;
		if(is_setting==0)
		{
			status_flag = 2;
			Status_Control(status_flag);
		}
	}
}
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
void Key_Control()
{
	char key = KEY_Scan();
	switch(key)
	{
		case '1':
			storage_flag++;
			if(storage_flag>5){  storage_flag=1;  }
			//Status_Control(storage_flag);
			Read_Time(storage_flag);
			break;
		case '2':
			Setting();
			break;
		case '3':
			Time_Setting(set_time_mode);
			break;
		case '4':
			Timer_State_Control();
			break;
	}
}
void Setting()
{

	char key = KEY_Scan();
	status_flag=3;
	Status_Control(status_flag);

	if(KEY2==0)
	{
		button2_delay++;
	
		if(button2_delay==1)
		{
			alarm_flag = 0;
			set_time_mode++;
			if(set_time_mode>3)
			{
				set_time_mode=1;
			}
			Draw_Under_line(set_time_mode);
		}	
		if(button2_delay>=4)
		{
			while(KEY2==0)
			//存储当前数据在当前位置
			alarm_flag = 0;
			Storage_Time(storage_flag);
			LCD_SetTextColor(White);
			LCD_DrawLine(120,280,250,Horizontal);
			LCD_SetTextColor(Black);
			status_flag=2;
			Status_Control(status_flag);
			return;
		}
	}
	else
	{
			button2_delay=0;
	}

}

void Draw_Under_line(int mode)
{
	if(mode==1)
	{
		LCD_SetTextColor(White);
		LCD_DrawLine(120,280,250,Horizontal);
		LCD_SetTextColor(Red);
		LCD_DrawLine(120,230,30,Horizontal);
	}
	if(mode==2)
	{
		LCD_SetTextColor(White);
		LCD_DrawLine(120,280,250,Horizontal);
		LCD_SetTextColor(Red);
		LCD_DrawLine(120,185,30,Horizontal);
	}
	if(mode==3)
	{
		LCD_SetTextColor(White);
		LCD_DrawLine(120,280,250,Horizontal);
		LCD_SetTextColor(Red);
		LCD_DrawLine(120,140,30,Horizontal);
	}
	LCD_SetTextColor(Black);
}
void Time_Setting(int mode)
{
	if(KEY3==0 && is_setting)
	{
		button3_delay++;
		//短按
		if(button3_delay==1)
		{
			if(mode==1)
			{
				hour++;
			}
			if(mode==2)
			{
				minute++;
			}
			if(mode==3)
			{
				second++;
			}
		}
		//长按
		if(button3_delay>=10)
		{
			if(mode==1)
			{
				hour++;
			
			}
			if(mode==2)
			{
				minute++;
			}
			if(mode==3)
			{
				second++;
			}
		}
	}
	else
	{
		button3_delay=0;
	}
	
}

void Timer_State_Control()
{
	LCD_SetTextColor(White);
	LCD_DrawLine(120,280,250,Horizontal);
	LCD_SetTextColor(Black);
	alarm_flag = 0;
	if(KEY4==0)
	{
		button4_delay++;
		if(button4_delay==1)
		{
			if(pause_start_flag==1)
			{
				status_flag=0;
				Status_Control(status_flag);
				pause_start_flag=!pause_start_flag;
			}
			else if(pause_start_flag==0)
			{
				status_flag=1;
				Status_Control(status_flag);
				pause_start_flag=!pause_start_flag;
			}
		}
		if(button4_delay==10)
		{
			status_flag=2;
			Status_Control(status_flag);
		}
	}
	else
	{
		button4_delay=0;
	}
}

void Led_Control()
{
	if(is_start==1)
	{
		LED_Control(LED1,led1_flag);
	}
	else
	{
		LED_Control(LED1,0);
	}
}
void Status_Control(int status_flag)
{
	if(status_flag==0)
	{
		is_standby = 0;
		is_setting = 0;
		is_start = 1;
		is_pause = 0;
	}
	if(status_flag==1)
	{
		is_standby = 0;
		is_setting = 0;
		is_start = 0;
		is_pause = 1;
	}
	if(status_flag==2)
	{
		is_standby = 1;
		is_setting = 0;
		is_start = 0;
		is_pause = 0;
	}
	if(status_flag==3)
	{
		is_standby = 0;
		is_setting = 1;
		is_start = 0;
		is_pause = 0;
	}
}
void Storage_Time(int storage_flag)
{
	if(storage_flag==1)
	{
		Write_Data(0x10,hour);    Delay_Ms(2);
		Write_Data(0x11,minute);  Delay_Ms(2);
		Write_Data(0x12,second);  Delay_Ms(2);
	}
	if(storage_flag==2)
	{
		Write_Data(0x20,hour);    Delay_Ms(2);
		Write_Data(0x21,minute);  Delay_Ms(2);
		Write_Data(0x22,second);  Delay_Ms(2);
	}
	if(storage_flag==3)
	{
		Write_Data(0x30,hour);    Delay_Ms(2);
		Write_Data(0x31,minute);  Delay_Ms(2);    
		Write_Data(0x32,second);  Delay_Ms(2);
	}
	if(storage_flag==4)
	{
		Write_Data(0x40,hour);    Delay_Ms(2);
		Write_Data(0x41,minute);  Delay_Ms(2);
		Write_Data(0x42,second);  Delay_Ms(2);
	}
	if(storage_flag==5)
	{
		Write_Data(0x50,hour);    Delay_Ms(2);
		Write_Data(0x51,minute);  Delay_Ms(2);
		Write_Data(0x52,second);  Delay_Ms(2);
	}
}
void Read_Time(int storage_flag)
{
	if(storage_flag==1)
	{
		hour = Read_Data(0x10);    Delay_Ms(2);
		minute = Read_Data(0x11);  Delay_Ms(2);
		second = Read_Data(0x12);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=1; }
		if(second>60 || second<0){ second=10; }
	}
	if(storage_flag==2)
	{
		hour = Read_Data(0x20);    Delay_Ms(2);
		minute = Read_Data(0x21);  Delay_Ms(2);
		second = Read_Data(0x22);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=0; }
		if(second>60 || second<0){ second=30; }
	}
	if(storage_flag==3)
	{
		hour = Read_Data(0x30);    Delay_Ms(2);
		minute = Read_Data(0x31);  Delay_Ms(2);
		second = Read_Data(0x32);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=1; }
		if(second>60 || second<0){ second=10; }
	}
	if(storage_flag==4)
	{
		hour = Read_Data(0x40);    Delay_Ms(2);
		minute = Read_Data(0x41);  Delay_Ms(2);
		second = Read_Data(0x42);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=1; }
		if(minute>60 || minute<0){ minute=2; }
		if(second>60 || second<0){ second=30; }
	}
	if(storage_flag==5)
	{
		hour = Read_Data(0x50);    Delay_Ms(2);
		minute = Read_Data(0x51);  Delay_Ms(2);
		second = Read_Data(0x52);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=5; }
		if(second>60 || second<0){ second=20; }
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
