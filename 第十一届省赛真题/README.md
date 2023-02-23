# 蓝桥杯嵌入式第11届省赛真题

[TOC]



## 1.题目分析

这题总体来说，逻辑性比12届停车题的难度低一点，但是串口需要分别的类型比12届多。以及那个LED变成16进制的解法，我个人的解法比较笨，不知道各位有没有更好的办法。**源码结尾自取！！！完全免费！！！**

读题后我的思路是这样的

串口需要分辨**ADC，LD，B这三种父模式**。LD的模式里还有0，1，2三种子模式，B里面有P和R两种子模式，ADC只需要返回对应值

所以串口方面我的结构如下：

1 ：int analysis(char *data);**分析函数**，专门解析数据，返回是哪种父模式

2：名称即注释，大家直接读就能发现，这是**串口控制按键和串口控制LED部分的子函数**，Button_Usart_Control(int mode)  ；  Led_Usart_Control(int led,int mode); 用于判断父模式后，再进一步判断子模式。ADC因为只需要返回对应值，没有多余情况，我就没有再写函数了

3：Usart_Control(); **总的串口控制函数**。将前面说的三个函数集成于其中，让他们各自配合运作即可

其他的其实就没太多好说的了。**源码结尾自取！！！完全免费！！！**

## 2.赛题

**基本功能**
通过竞赛板上电位器R37输出模拟电压信号，经微控制器内部AD采集处理后，通过液晶屏实时显示。
通过串口接收上位机指令，执行指令，并返回数据。
支持按键扫描功能，可识别当前各个按键状态。
LED亮灭受控。
**初始化状态说明**
指示灯LD1-LD8全部处于熄灭状态。
**串口通信功能**
使用竞赛板USART2完成全部串口通信功能.
通信波特率配置为9600bps。
**LED亮灭控制指令**
指令格式:“LDn:0”、“LDn:1”或“LDn:2”
指令解析：编号为n的LED指示灯点亮或熄灭，n的范围是1-8。0表示熄灭，1表示点亮，2控制指示灯状态翻转。
指令举例：
“LD1:0”，控制指示灯LD1熄灭。
“LD1:1”，控制指示灯LD1点亮。
“LD2:2”,控制指示灯LD2亮灭状态翻转。
指令回复：
本条指令不需要回复任何内容。

**按键状态查询**
指令格式：“Bn?”
指令解析：查询编号为n的按键状态。n的范围是1-4；
指令举例：“B1?”
指令回复：
“B1:P”或“B1:R”，其中P表示B1按键处于按下的状态，R表示B1按键处于释放的状态。
**模拟电压查询指令**
指令格式：“ADC?”
指令解析：查询当前微控制器采集到的实时电压值。
指令举例：“ADC?”
指令回复：
“ADC：3.02V”，表示当前采集到的电压值为3.02V，电压值保留小数点后两位有效数字。
未知指令
当设备收到收到未知的错误指令时，返回“error”。
通信指令要求
请严格按照上述1-5条中要求设计串口交互过程，注意指令格式、大小写等设计细节。
**液晶显示功能**
显示信息项
通过液晶屏幕显示按键状态、LED指示灯状态和ADC采集数据3个信息项，显示格式与位置如下图所示

[![oFpaL9.png](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/oFpaL9.png)](https://imgtu.com/i/oFpaL9)

显示格式要求
显示背景色(BackColor)：黑色。
显示前景色(TextColor)：白色。
请严格按照图示要求设计各个信息项的名称和行列位置。
**LED指示灯状态以一个16进制编码的数字表示（A、B、C、D、E、F字符大写，高位为0时显示），图示中的给出0A表示板上指示灯从LD8到LD1的状态为：灭-灭-灭-灭-亮-灭-亮-灭。**
按键状态用P或R表示，P表示按键处于按下状态，R表示处于释放状态。如果
ADC采集数值需要保留小数点后2位有效数字，显示单位为伏特（V）。

**按键功能**

1. 按键B1短按键操作：所有LED指示灯熄灭。
2. 按键B1长按键操作（按下时长超过800ms）：所有LED指示灯状态翻转。



## 3.项目结构

### 3.1全局变量

```c
char *Button_Status[2] = {"P","R"};
int B1_Status = 1;
int B2_Status = 1;
int B3_Status = 1;
int B4_Status = 1;
```

```c
//数组下标位0的位置是故意不取的，因为串口发送的是LD1开始的，这个1刚好对应数组里的1下标
//这是我想到的led状态转化16进制显示的，我觉得很笨，但是我想不到好方法了
int Hex_Led[9] = {-1,1,2,4,8,16,32,64,128};
//记录每个灯的状态
int Led_Status[9] = {-1,0,0,0,0,0,0,0,0};
//点灯的数组里面是宏定义的具体的板载LED
int Led_Arr[9] = {-1,LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8};
//对应后面的16进制数组，	对应下标:Led_Sum=Led_Status[i]*Hex_Led[i]
int Led_Sum = 0;
//LCD 16进制显示部分
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
```

### 3.2串口部分

```c
//串口控制部分
int analysis(char *data);
void Usart_Control();
void Button_Usart_Control(int mode);
void Led_Usart_Control(int led,int mode);
```

**串口总控**

```c
void Usart_Control()
{	
	int mode;
	char key = KEY_Scan();
	//获取ADC
	Adc_Temp = Get_Adc();
	//更新按键状态 这里其实应该分函数 
	if(key=='1') {B1_Status=0; } else {B1_Status=1;}
	if(key=='2') {B2_Status=0; } else {B2_Status=1;}
	if(key=='3') {B3_Status=0; } else {B3_Status=1;}
	if(key=='4') {B4_Status=0; } else {B4_Status=1;}
	//接收数据
	if(RXOVER == 1)
	{
		RXOVER = 0;
		//解析父模式
		mode = analysis(USART_RXBUF);
	}
	switch(mode)
	{
		case 1:
			//情况1 是led模式，调用led模式函数 传参:哪个led，哪个模式
			Led_Usart_Control(USART_RXBUF[2]-'0',USART_RXBUF[4]-'0');
			break;
		case 2:
			//情况2 是button模式，调用button模式函数 传参:具体哪个按键
			Button_Usart_Control(USART_RXBUF[1]-'0');
			break;
		case 3: 
			//发送回ADC参数值
			sprintf(ADC_Show,"ADC: %.2f V\n",Adc_Temp);
			USART_SendString(ADC_Show);
			break;
		case 4:
			USART_SendString("error\n");
			break;
		default:
			break;
	}
    //清除缓冲区
	memset(USART_RXBUF,0,sizeof(USART_RXBUF)  );
	USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);//¿ªÖÐ¶Ï
	//我不知道为什么这里只能这样 不能用for循环遍历
	Led_Sum = Led_Status[1]*Hex_Led[1] + Led_Status[2]*Hex_Led[2] + 
        	  Led_Status[3]*Hex_Led[3] + Led_Status[4]*Hex_Led[4] + 
        	  Led_Status[5]*Hex_Led[5] + Led_Status[6]*Hex_Led[6] + 
              Led_Status[7]*Hex_Led[7] + Led_Status[8]*Hex_Led[8];
	return;
}
```

**Button串口控制**

```c
void Button_Usart_Control(int mode)
{
    //数据不正确
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
```

**LED串口控制**

```c
void Led_Usart_Control(int led,int mode)
{
    //接收数据不正确
	if(led>8 || led < 1 || mode>2 || mode <0)
	{
		USART_SendString("error\n");
		return;
	}
	switch(mode)
	{
		case 0:
            //0模式 关灯 从从数组中取出对应的宏定义的LED
			LED_Control(Led_Arr[led],0);
            //该灯对应位置的状态置为0。
		 	Led_Status[led]=0;
			break;
		case 1:
            //1模式 开灯 从从数组中取出对应的宏定义的LED
			LED_Control(Led_Arr[led],1);
            //该灯对应位置的状态置为0。
			Led_Status[led]=1;
			break;		
		case 2:
            //2模式 反转 从从数组中取出对应的宏定义的LED
            //先翻转该led的状态
			Led_Status[led]= !Led_Status[led];
            //反转后将其状态作为控制变量输入LED的控制函数
			LED_Control(Led_Arr[led],Led_Status[led]);
		break;
	}
}
```

**数据解析函数**

```c
int analysis(char *data)
 {
    //由于c的拷贝函数存在问题，所以要初始化为0
	char LED_Mode[10]={'\0'};
	char Button_Mode[10]={'\0'};
	char ADC_Mode[10]={'\0'};
    //对数据进行切割拷贝
	strncpy(LED_Mode,data,2);
	strncpy(Button_Mode,data,1);
	strncpy(ADC_Mode,data,4);
	//如果满足LED模式
	if(strcmp("LD",LED_Mode)==0 && strcmp(':',data[3])==0)
	{
        //格式有误
		if(strlen(data)!=6)
		{
			return 4;
		}
		return 1;
	}
    //如果满足Button模式
	else if(strcmp("B",Button_Mode)==0 && strcmp('?',data[2])==0)
	{
        //格式有误
		if(strlen(data)!=4)
		{
			return 4;
		}
		return 2;
	}
    //如果满足ADC模式
	else if(strcmp("ADC?",ADC_Mode)==0)
	{
        //格式有误
		if(strlen(data)!=5)
		{
			return 4;
		}
		return 3;
	}
    //格式有误
	else
	{
		return 4;
	}
}
```

至此串口控制部分结束。可以看出**利用：数组封装状态 和 函数一起的配合。解决了很多问题**

### 3.3 按键长短按控制

```c
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
					//要想象按键按下的时候是一直触发这个while的
                    //所以主函数这时一直卡在这，那么如果我们还要正常的运行
                    //一定需要将LCD的实时显示和串口的控制在这里也写上
                    B1_Status=0;
					Usart_Control();
                    LCD_Show();
					flag++;
				}
                //长按关全部灯
				if(flag>1000)
				{
					LED_Control(LEDALL,0);
                    //全部灯状态置0，其实可以用memset函数看起来简洁一点
					for(i=1;i<9;i++)
					{
						Led_Status[i]= 0;
					}
					flag=0;
				}
                //短按翻转所有灯
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
```

### 3.4主函数

```c
//省略初始化部分
	while(1)
	{
		Button_Led_Control();
		LCD_Show();
		Usart_Control();
	}

```

总结：本次题目还是见证了我的进步的，例如

**从第12届的省赛题目里**，我学到了在串口控制中，先写一个analysis的解析函数，对收到数据进行相关解析处理。然后在里面可以对需要的变量赋值，或者返回对应模式。解析之后再专门写那些对应的控制函数什么的，让代码做到轻耦合，比较容易查错什么的。

**从第10届省赛题里**，学到了利用封装数组，在很多地方直接调用数组配合下标，可以很方便的解决一些问题。例如封装LED数组，这样就可以利用一个1下标，对应属于LED1的所有东西，例如LED1当前状态，LED1的宏定义数组，LED1对应的二进制值(为了转换16进制)

但是我觉得写代码的过程中，我有很多地方需要完善，欢迎大家的指正

https://gitee.com/jason0131czy/embedded-blue-bridge-cup