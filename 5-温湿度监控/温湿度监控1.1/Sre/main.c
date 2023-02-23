#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "adc.h"
#include "iic.h"
#include "stdio.h"
u32 TimingDelay = 0;

//第二天需要加的：高亮显示，和第一个界面的显示

//以后这些还是用一个disp替代吧，不然可读性高但是太浪费时间和内存
char current_temperature_disp[20];
char current_humidity_disp[20];
char current_time_disp[20];
char current_record_count_disp[20];
char humidity_limit_disp[20];
char temperature_limit_disp[20];
char test_signal_disp[20];
char sampling_interval_disp[20];

double test_signal = 1.5;
int temperature_limit = 40;
int humidity_limit = 80;
int sampaling_interval = 1;

double current_temperature;
int current_humidity=60;
int current_hour=12;
int current_minute=50;
int current_second=0;
int current_record_count=0;
double temp_array[60];
int Disp_Flag = 1;
int Setting_Flag = 1;

void Choose_Disp();
void Delay_Ms(u32 nTime);
void Data_Gui();
void Setting_Gui(int flag);

void Key_Control();
void Change_Temp(int flag);
void Change_Humidity(int flag);
void Change_Interval(int flag);
void Choose_Setting(int setting_flag,int control_flag);
void HighLight(int flag);
void Get_Temp();
//Main Body
int main(void)
{
//	SysTick_Config(SystemCoreClock/1000);
//	Delay_Ms(200);
//	STM3210B_LCD_Init();
//	KEY_Init();
//	LED_Init();
//	i2c_init();
//	ADC_Config();
//	LCD_Clear(White);
//	LCD_SetBackColor(White);
//	LCD_SetTextColor(Black);
//	LED_Control(LEDALL,0);
	while(1)
	{
//		Get_Temp();
//		Choose_Disp();
//		Key_Control();
	}
}

void Get_Temp()
{
	double adc_temp;
	adc_temp = Get_Adc();
	current_temperature = (double)(adc_temp*24.4 - 20);
	if(current_temperature>60)
		current_temperature = 60.0;
}

void Data_Gui()
{
	sprintf(current_temperature_disp,"Temperature:%2.2f",current_temperature);
	sprintf(current_humidity_disp,"Humidity:%2d%%",current_humidity);
	sprintf(current_time_disp,"Clock:%2d-%2d-%2d",current_hour,current_minute,current_second);
	sprintf(current_record_count_disp,"       Count: %2d",current_record_count);
	
	LCD_DisplayStringLine(Line1,"   Current Data");
	LCD_DisplayStringLine(Line3,current_temperature_disp);
	LCD_DisplayStringLine(Line5,current_humidity_disp);
	LCD_DisplayStringLine(Line7,current_time_disp);
	LCD_DisplayStringLine(Line9,current_record_count_disp);
}

void Setting_Gui(int flag)
{
	sprintf(temperature_limit_disp,"Temp Limit:%2dC",temperature_limit);
	sprintf(humidity_limit_disp,"Humidity Limit:%2d%%",humidity_limit);
	sprintf(sampling_interval_disp,"Interval:%2dS",sampaling_interval);
	sprintf(test_signal_disp,"Test Signal: %2.1fKHz",test_signal);
	
	if(flag==1)
	{
		LCD_SetTextColor(Black);
		LCD_DisplayStringLine(Line1,"   Param Setting");
		LCD_DisplayStringLine(Line5,humidity_limit_disp);
		LCD_DisplayStringLine(Line7,sampling_interval_disp);
		LCD_DisplayStringLine(Line9,test_signal_disp);
		LCD_SetTextColor(Green);
		LCD_DisplayStringLine(Line3,temperature_limit_disp);
	}
	if(flag==2)
	{
		LCD_SetTextColor(Black);
		LCD_DisplayStringLine(Line1,"   Param Setting");
		LCD_DisplayStringLine(Line3,temperature_limit_disp);
		LCD_DisplayStringLine(Line7,sampling_interval_disp);
		LCD_DisplayStringLine(Line9,test_signal_disp);
		LCD_SetTextColor(Green);
		LCD_DisplayStringLine(Line5,humidity_limit_disp);
	}
	if(flag==3)
	{
		LCD_SetTextColor(Black);
		LCD_DisplayStringLine(Line1,"   Param Setting");
		LCD_DisplayStringLine(Line3,temperature_limit_disp);
		LCD_DisplayStringLine(Line5,humidity_limit_disp);
		LCD_DisplayStringLine(Line9,test_signal_disp);
		LCD_SetTextColor(Green);
		LCD_DisplayStringLine(Line7,sampling_interval_disp);

	}
	if(flag==4)
	{
		LCD_SetTextColor(Black);
		LCD_DisplayStringLine(Line1,"   Param Setting");
		LCD_DisplayStringLine(Line3,temperature_limit_disp);
		LCD_DisplayStringLine(Line7,sampling_interval_disp);
		LCD_DisplayStringLine(Line5,humidity_limit_disp);
		LCD_SetTextColor(Green);
		LCD_DisplayStringLine(Line9,test_signal_disp);
	}
}
void Choose_Disp()
{
	char key = KEY_Scan();
	if(key=='1')
	{
		LCD_Clear(White);
		Disp_Flag = !Disp_Flag;
	}
	if(Disp_Flag){
		LCD_SetTextColor(Black);
		Data_Gui();
		return;
	}
	Setting_Gui(Setting_Flag);
}
void Change_Temp(int flag)
{
	if(flag==1){
		if(temperature_limit+1>60)
		{
			temperature_limit = -20;
			return;
		}
		temperature_limit++;
	}
	else{
		if(temperature_limit-1<-20)
		{
			temperature_limit = 60;
			return;
		}
		temperature_limit--;
	}
}

void Change_Humidity(int flag)
{
	if(flag==1){
		if(humidity_limit+5>90)
		{
			humidity_limit = 10;
			return;
		}
		humidity_limit+=5;
	}
	else
	{
		if(humidity_limit-5<10)
		{
			humidity_limit = 90;
			return;
		}
		humidity_limit-=5;
	}
}

void Change_Interval(int flag)
{
	if(flag==1){
		if(sampaling_interval+1>5)
		{
			sampaling_interval = 1;
			return;
		}
		sampaling_interval++;
		return;
	}
	else{
		if(sampaling_interval-1<1)
		{
			sampaling_interval = 5;
			return;
		}
		sampaling_interval--;
	}
}
void Change_Signal(int flag)
{
	if(flag==1){
		test_signal+=0.5;
		return;
	}
	test_signal-=0.5;
}
void Key_Control()
{
	char key;
	if(Disp_Flag == 1)
		return;
	key = KEY_Scan();
	switch(key)
	{
		case '2':
			Setting_Flag++;
			if(Setting_Flag==5)
			{
				Setting_Flag=1;
			}
			break;
		case '3':
			Choose_Setting(Setting_Flag,1);
			break;
		case '4':
			Choose_Setting(Setting_Flag,-1);
			break;
	}
}
void Choose_Setting(int setting_flag,int control_flag)
{
	if(setting_flag==1)
	{
		Change_Temp(control_flag);
	}
	if(setting_flag==2)
	{
		Change_Humidity(control_flag);
	}
	if(setting_flag==3)
	{
		Change_Interval(control_flag);
	}
	if(setting_flag==4)
	{
		Change_Signal(control_flag);
	}
}

void HighLight(int flag)
{

}
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
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
