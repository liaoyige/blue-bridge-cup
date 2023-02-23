#include "stm32f10x.h"
#include "lcd.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "adc.h"
#include "stdio.h"
#include "usart.h"
#include "stdlib.h"

extern char USART_RXBUF[20];

extern uint8_t RXOVER;

void Delay_Ms(u32 nTime);

//按键控制灯
void Button_Led_Control();
//串口部分函数
int analysis(char *data);
void Usart_Control();
void Button_Usart_Control(int mode);
void Led_Usart_Control(int led,int mode);
void LCD_Show();
char * substring(char * ch,int position,int size);

//展示部分
char Button1_Show[12];
char Button2_Show[12];
char Button3_Show[12];
char Button4_Show[12];
char ADC_Show[18];
char Led_Show[12];

//Led控制处理部分
int Hex_Led[9] = {-1,1,2,4,8,16,32,64,128};
int Led_Status[9] = {-1,0,0,0,0,0,0,0,0};
int Led_Arr[9] = {-1,LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8};
//对应后面的16进制数组 Led_Sum=Led_Status[i]*Hex_Led[i]
int Led_Sum = 0;
//这方法也太笨了
unsigned char AllLed_Status[][2] = 
{
	"00","01","02","03","04","05","06","07",
  "08","09","0A","0B","0C","0D","0E","0F",
  "10","11","12","13","14","15","16","17",
  "18","19","1A","1B","1C","1D","1E","1F",
  "20","21","22","23","24","25","26","27",
  "28","29","2A","2B","2C","2D","2E","2F",
  "30","31","32","33","34","35","36","37",
  "38","39","3A","3B","3C","3D","3E","3F",
  "40","41","42","43","44","45","46","47",
  "48","49","4A","4B","4C","4D","4E","4F",
  "50","51","52","53","54","55","56","57",
  "58","59","5A","5B","5C","5D","5E","5F",
  "60","61","62","63","64","65","66","67",
  "68","69","6A","6B","6C","6D","6E","6F",
  "70","71","72","73","74","75","76","77",
  "78","79","7A","7B","7C","7D","7E","7F",
  "80","81","82","83","84","85","86","87",
  "88","89","8A","8B","8C","8D","8E","8F",
  "90","91","92","93","94","95","96","97",
  "98","99","9A","9B","9C","9D","9E","9F",
  "A0","A1","A2","A3","A4","A5","A6","A7",
  "A8","A9","AA","AB","AC","AD","AE","AF",
  "B0","B1","B2","B3","B4","B5","B6","B7",
  "B8","B9","BA","BB","BC","BD","BE","BF",
  "C0","C1","C2","C3","C4","C5","C6","C7",
  "C8","C9","CA","CB","CC","CD","CE","CF",
  "D0","D1","D2","D3","D4","D5","D6","D7",
  "D8","D9","DA","DB","DC","DD","DE","DF",
  "E0","E1","E2","E3","E4","E5","E6","E7",
  "E8","E9","EA","EB","EC","ED","EE","EF",
  "F0","F1","F2","F3","F4","F5","F6","F7",
  "F8","F9","FA","FB","FC","FD","FE","FF",
};


//按键状态
char *Button_Status[2] = {"P","R"};
int B1_Status = 1;
int B2_Status = 1;
int B3_Status = 1;
int B4_Status = 1;
//ADC取值
double Adc_Temp;
int TimingDelay;
int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	Delay_Ms(200);
	ADC_Config();
	KEY_Init();
	STM3210B_LCD_Init();
	LED_Init();
	USART_Config();//串口配置
	NVIC_Configuration();//中断配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //接收中断使能
	LCD_Clear(White);
	LCD_SetBackColor(White);
	LCD_SetTextColor(Black);
	LED_Control(LEDALL,0);
	
	while(1)
	{
		Button_Led_Control();
		LCD_Show();
		Usart_Control();
	}

}




//切割字符串
char* substring(char* sre,int pos,int length)  
{  
		char *dst = (char*)calloc(sizeof(char),length+1);
		strncpy(dst,sre+pos,length);
		dst[length+1]='\0';
    return dst; 
} 
//串口总控制
void Usart_Control()
{	
	int mode;
	char key = KEY_Scan();
	//获取ADC的值
	Adc_Temp = Get_Adc();
	//更新按键状态
	if(key=='1') {B1_Status=0; } else {B1_Status=1;}
	if(key=='2') {B2_Status=0; } else {B2_Status=1;}
	if(key=='3') {B3_Status=0; } else {B3_Status=1;}
	if(key=='4') {B4_Status=0; } else {B4_Status=1;}
	//接收数据
	if(RXOVER == 1)
	{
		RXOVER = 0;
		//调用解析函数 解析判断三个模式
		mode = analysis(USART_RXBUF);
	}
	switch(mode)
	{
		case 1:
			//调用Led串口控制函数
			Led_Usart_Control(USART_RXBUF[2]-'0',USART_RXBUF[4]-'0');
			break;
		case 2:
			//调用按键串口控制函数
			Button_Usart_Control(USART_RXBUF[1]-'0');
			break;
		case 3: 
			//发回ADC数据
			sprintf(ADC_Show,"ADC: %.2f V\n",Adc_Temp);
			USART_SendString(ADC_Show);
			break;
		case 4:
			USART_SendString("error\n");
			break;
		default:
			break;
	}
	memset(USART_RXBUF,0,sizeof(USART_RXBUF)  );
	USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//开中断
	//这里不知道为什么用for循环会卡
	//计算16进制码的下标
	Led_Sum = Led_Status[1]*Hex_Led[1] + Led_Status[2]*Hex_Led[2]+Led_Status[3]*Hex_Led[3]+Led_Status[4]*Hex_Led[4]
				  + Led_Status[5]*Hex_Led[5] + Led_Status[6]*Hex_Led[6]+Led_Status[7]*Hex_Led[7]+Led_Status[8]*Hex_Led[8];
	return;
}
//数据解析
int analysis(char *data)
 {
	char LED_Mode[10]={'\0'};
	char Button_Mode[10]={'\0'};
	char ADC_Mode[10]={'\0'};
	strncpy(LED_Mode,data,2);
	strncpy(Button_Mode,data,1);
	strncpy(ADC_Mode,data,4);

	if(strcmp("LD",LED_Mode)==0 && strcmp(':',data[3])==0)
	{
		if(strlen(data)!=6)
		{
			return 4;
		}
		return 1;
	}
	else if(strcmp("B",Button_Mode)==0 && strcmp('?',data[2])==0)
	{
		if(strlen(data)!=4)
		{
			return 4;
		}
		return 2;
	}
	else if(strcmp("ADC?",ADC_Mode)==0)
	{
		if(strlen(data)!=5)
		{
			return 4;
		}
		return 3;
	}
	else
	{
		return 4;
	}
}
//界面展示
void LCD_Show()
{
	sprintf(ADC_Show,"ADC: %.2f V",Adc_Temp);
	sprintf(Button1_Show,"B1 : %s",Button_Status[B1_Status]);
	sprintf(Button2_Show,"B2 : %s",Button_Status[B2_Status]);
	sprintf(Button3_Show,"B3 : %s",Button_Status[B3_Status]);
	sprintf(Button4_Show,"B4 : %s",Button_Status[B4_Status]);
	sprintf(Led_Show,"LED : %c%c",AllLed_Status[Led_Sum][0],AllLed_Status[Led_Sum][1]);
	LCD_DisplayStringLine(Line1,ADC_Show);
	LCD_DisplayStringLine(Line2,Button1_Show);
	LCD_DisplayStringLine(Line3,Button2_Show);
	LCD_DisplayStringLine(Line4,Button3_Show);
	LCD_DisplayStringLine(Line5,Button4_Show);
	LCD_DisplayStringLine(Line6,Led_Show);
}


//按键串口控制
void Button_Usart_Control(int mode)
{
	if(mode>4 || mode<1)
	{	
		USART_SendString("error\n");
		return;
	}
	sprintf(Button1_Show,"B1 : %s \n",Button_Status[B1_Status]);
	sprintf(Button2_Show,"B2 : %s \n",Button_Status[B2_Status]);
	sprintf(Button3_Show,"B3 : %s \n",Button_Status[B3_Status]);
	sprintf(Button4_Show,"B4 : %s \n",Button_Status[B4_Status]);
	if(mode==1){ USART_SendString(Button1_Show); }
	if(mode==2){ USART_SendString(Button2_Show); }
	if(mode==3){ USART_SendString(Button3_Show); }
	if(mode==4){ USART_SendString(Button4_Show); }
}
//LED串口控制
void Led_Usart_Control(int led,int mode)
{
	if(led>8 || led < 1 || mode>2 || mode <0)
	{
		USART_SendString("error\n");
		return;
	}
	switch(mode)
	{
		case 0:
			LED_Control(Led_Arr[led],0);
		  Led_Status[led]=0;
			break;
		case 1:
			LED_Control(Led_Arr[led],1);
			Led_Status[led]=1;
			break;		
		case 2:
			Led_Status[led]= !Led_Status[led];
			LED_Control(Led_Arr[led],Led_Status[led]);
		break;
	}
}
//按键LED控制
void Button_Led_Control()
{
		int flag;
		int i;
		char key;
		key = KEY_Scan();
		switch(key)
		{
			case '1':
				while(KEY1==0)
				{
					//没有这句 就不能返回B1被按下的情况
					Usart_Control();
					B1_Status=0;
					LCD_Show();
					flag++;
				}
				if(flag>1000)
				{
					LED_Control(LEDALL,0);
					for(i=1;i<9;i++)
					{
						Led_Status[i]= 0;
					}
					flag=0;
				}
				else
				{
					for(i=1;i<9;i++)
					{
						Led_Status[i]= !Led_Status[i];
						LED_Control(Led_Arr[i],Led_Status[i]);
					}
				}
		}
}

