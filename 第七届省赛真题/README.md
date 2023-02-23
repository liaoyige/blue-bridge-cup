# 蓝桥杯嵌入式第七届省赛真题

**代码结尾自取 完全免费！！！！！！**

**代码结尾自取 完全免费！！！！！！**

**代码结尾自取 完全免费！！！！！！**

## 1.题目

​        “模拟液位检测告警系统”通过采集模拟电压信号计算液位高度，并根据用户设定的液位阈值执行报警动作，在液位等级发生变化时，通过串行通讯接口将液位信息发送到 PC 机。                                   

设计任务及要求

1. 液位检测

   通过电位器 R37 模拟液位传感器输出电压信号，设备以 1 秒为间隔采集 R37 输出电压，并与用户设定的液位阈值进行比较。假定液位高度与 R37 输出电压之间具有正比例关系：H = VR37*K，当 VR37=3.3V 时，对应液位高度为 100cm。通过液晶显示当前的液位高度、传感器（R37）输出状态和液位等级，液位检测显示界面如图 1 所示 

      [![](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/oA6NPf.png)](https://imgtu.com/i/oA6NPf)

   AD 采集得到的结果应经过软件滤波算法处理，显示结果保留小数点后两位有效数字

2. 液位阈值设定

   设备可设定三个液位阈值，对应四个液位等级，阈值由用户通过按键输入，设备保存阈值，并根据此阈值判断液位等级，假        定用户输入的三个液位阈值为 10cm、20cm 和 30cm，液位高度与液位等级的对应关系如下：
    2.1 液位高度≤10cm 时，液位等级为 0；
    2.2 10cm<液位高度≤20cm 时，液位等级为 1；
    2.3 20cm<液位高度≤30cm 时，液位等级为 2；
    2.4 液位高度＞30cm 时，液位等级为 3。
    设备初始液位阈值分别为 30cm、50cm 和 70cm，用户修改阈值后，设备应将此参数保存在 E2PROM 中，当设备重新上电时，可从 E2PROM 中获取。

3. 液位阈值设定

   B1 按键：“设置”按键，按下后进入阈值设定界面（如图 2 所示），再次按下 B1 按键时退出设置界面，保存用户设定的结果到  E2PROM，并返回图 1 所示的液位检测界面。    
   B2 按键：切换选择 3 个待修改的阈值，被选中的阈值应突出显示。
   B3 按键：“加”按键，按下后，被选择的阈值增加 5cm，增加到 95cm 为止。
   B4 按键：“减”按键，按下后，被选择的阈值减少 5cm，减少到 5cm 为止。


   [<img src="https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/oA6YIP.png" alt="oA6YIP.png"  />](https://imgtu.com/i/oA6YIP)

4. 串口查询与输出功能

    使用 STM32 USART2 完成以下串口功能，波特率设置为 9600。

4.1 查询
        通过 PC 机向设备发送字符‘C’，设备返回当前液位高度和液位等级；
        通过 PC 机向设备发送字符‘S’，设备返回当前设定的三个阈值。
        液位高度和等级返回数据格式举例： “C:H55+L2\r\n 解析：应答高度、等级查询，液位高度为 55cm，液位等级为 2。
        阈值返回数据格式举例：“S:TL30+TM50+TH70\r\n”  解析：应答阈值查询，设备内保存的三个阈值分别为 30cm、50cm 和 70cm。
4.2 输出
        当液位等级发生变化时，设备自动向 PC 机发送当前液位等级、液位高度和液位变化趋势（上升或下降）。
        输出数据格式举例：
                “A:H55+L2+D\r\n”
                解析：液位变化自动发送，液位高度 55cm，液位等级为 2，变化趋势下降。
                “A:H55+L2+U\r\n”
                解析：液位变化自动发送，液位高度 55cm，液位等级为 2，变化趋势上升。

5.状态指示

​	  LED 指示灯功能定义如下：
​	  LD1：运行状态指示灯，以 1 秒为间隔亮灭闪烁；
​	  LD2：液位等级变化指示灯，当液位等级发生变化时，LD2 以 0.2 秒为间隔闪烁 5 次；
​	  LD3：通讯状态指示灯，当设备接收到查询指令时，LD3 以 0.2 秒为间隔闪烁 5 次

## 2.题目分析

本次的题目难度没有很突出，但是有的地方挺有意思的，特别是那个监测液位等级改变时自动发送数据什么的。一开始我还出了个小bug没解决

其实大部分难度都不大，这里我就讲一下那个自动监测等级改变时发送数据的方法吧，后面大家直接看我的注释应该就看得懂了，

**高光部分的数据控制有的人可能写的很麻烦，这里我用了我前面 第十届蓝桥杯嵌入式真题 的一个解决办法，大家可以去看一眼**

[第十届蓝桥杯真题]: https://blog.csdn.net/qq_51663309/article/details/121460829

- 如果要做到监测等级的改变，那么我们就需要一个Old_Level变量，来保存上一次的旧数据，以及一个Liquid_Level来储存当前液位等级

- 在While(1)主函数之前就要先放一个初始化Old_Level的函数，不然如果我们给Old_Level手动指定默认值的话，假设默认值是0，但是液位等级是1，那么我们一上电，LD2状态指示灯就会闪烁五次，那么这就是我们不愿意看到的。所以在进入主循环之前就要先初始化

- 后面对比的时候，利用液位的高低变量Liquid_Height对比上下限，进而改变Liquid_Level，当对比成功后，先对Liquid_Level赋值，注意，假设我们是上电后第一次改变了液位的等级，那么此时Old_Value里存储的就是我们上电的时候初始的液位等级，那Liquid_Level此时被赋值后和Old_Value的大小就是不一样的了，通过比较他们的大小我们就可以分析出变化趋势然后发送串口数据

  下面开始上大部分控制逻辑代码

## 3.项目结构

### **3.1 函数结构和全局变量**

```c
extern char USART_RXBUF[10];
extern uint8_t RXOVER;

int TimingDelay;
//显示部分
char Height_Show[20];
char ADC_Show[20];
char Liquid_Level_Show[20];
char Threshold1_Show[20];
char Threshold2_Show[20];
char Threshold3_Show[20];
//如英语翻译所示...
double Adc_Temp;
int Liquid_Height;
int Liquid_Level;
int Old_Level;
//三个阈值
int Threshold1=30;
int Threshold2=50;
int Threshold3=70;
//高亮部分的标志位
int High_Light_flag = 1;
//K1按键模式
int mode=1;
char key;

//灯相关控制	
int Led1_Flag=1;
int Led2_Flag=0;
int Led3_Flag=0;
int Led3_Happen=0;
int Led2_Happen=0;

//显示界面函数
void Liquid_Level_Display();
void Parameter_Setup_Display(int High_Light_flag);
//设置函数
void Setting();
//K3,K4按键控制函数
void Key3_Control(int flag);
void Key4_Control(int flag);
//串口模式分析
int analysis(char * data);
//液位等级控制
void Level_Control();
//串口总控制
void Usart_Control();
//Led控制
void Led_Control();
//初始化液位等级
void Level_Init();
//初始化阈值数据(EEROM)
void Init_Data();
//保存阈值数据(EEROM)
void Save_Data();
```



### 3.2 界面

**Setting设置函数**

```c
void Setting()
{
    //计算液位高度
	Adc_Temp = Get_Adc();
	Liquid_Height=Adc_Temp*30.3;
	key = KEY_Scan();
	switch(key)
	{
		case '1':
            //主界面和设置界面切换标志
			LCD_Clear(White);
            //这个在后续的switch里用
			mode = !mode;
			break;
		case '2':
			High_Light_flag++;
            //高光部分标志位控制 这个做法在我博客的第十届省赛题里就用过
			if(High_Light_flag>3) { High_Light_flag = 1; }
			break;			
		case '3':
            //直接将前面的高光标志位传入 这样按键就能执行高光那行的正确逻辑
			Key3_Control(High_Light_flag);
			break;
		case '4':
			Key4_Control(High_Light_flag);
			break;
	}
    //两个界面的切换显示
	switch(mode)
	{
		case 1:
			Liquid_Level_Display();
			break;		
		case 0:
            //同理传入高光标志位,做到 Key3，Key4，和对应的显示相配合
			Parameter_Setup_Display(High_Light_flag);
			break;
	}
}
```

**void Parameter_Setup_Display(int High_Light_flag) 高光界面函数**

这里我只想到这么写，想不到怎么精简了

```c
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
```

剩下的就是一些数据刷新什么的函数，这里我觉得没啥价值就不写啦，说白了就是Sprintf拼接然后LCD显示就可以了

### 3.3 K3，K4按键控制

特别简单，举一个K3的例子就行了，K4就是反过来

```c
void Key3_Control(int flag)
{
    //可以看出这里我们是完全配合高光显示界面做的
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
```

### 3.4 串口控制

**Usart_Control()串口总控**

```c
void Usart_Control()
{
	int mode;
	char data[20];

	if(RXOVER==1)
	{
		RXOVER = 0;
		//解析数据 判断我们需要返回哪种模式的数据
        //其实这题的逻辑简单，可以直接全写这，但我习惯分模块了
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
```

**int analysis(char * data) 数据解析函数**

```c
int analysis(char * data)
{
	if(data[0]=='C')
	{
        //接收到数据 Led3_Happen标志位置1，触发systick中断里的Led3部分
		Led3_Happen=1;
		return 1;
	}
	else if(data[0]=='S')
	{
        //接收到数据 Led3_Happen标志位置1，触发systick中断里的Led3部分
		Led3_Happen=1;
		return 2;
	}
}
```

### 3.5 液位控制

**Level_Control() 液位控制**

```c
void Level_Control()
{

	char data[20];
	//0等级
	if(Liquid_Height>0 && Liquid_Height < Threshold1)
	{
        //先对当前液位等级变量赋值
		Liquid_Level=0;
        //这里只需要判断一次上升，下降我们交给1等级取判断
		if(Old_Level>Liquid_Level)
		{
            //状态改变，Led2_Happen标志位置1，触发systick中断里的Led2的部分
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+D\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
  		//判断完了后再对这个旧等级变量赋值
		Old_Level=0;
	}
    //1等级
	else if(Liquid_Height > Threshold1 && Liquid_Height < Threshold2)
	{
        //同理
		Liquid_Level=1;
        //旧值比新值大，那么就是下降
		if(Old_Level>Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+D\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
        //旧值比新值小，那么就是上升
		if(Old_Level<Liquid_Level)
		{
			Led2_Happen=1;
			sprintf(data,"H%d+L%d+U\r\n",Liquid_Height,Liquid_Level);
			USART_SendString(data);
		}
        //同理
		Old_Level=1;
	}
    //后续同理
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
```

这里是前面提到的 **等级初始化函数void Level_Init()**

```c
void Level_Init()
{
	int i;
	char data[20];
	//刚上电的时候ADC读取的数据是错误的，所以这里先让他读十次
	for(i=0 ;i<10;i++){  	Adc_Temp = Get_Adc();  }
	Liquid_Height=Adc_Temp*30.3;
	//判断初始化的状态赋值
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
```

### 3.6 Led控制

**亮灭Led_Control(主要通过systick中断来改变标志位)**

```c
void Led_Control()
{
	LED_Control(LED1,Led1_Flag);
	LED_Control(LED3,Led3_Flag);
	LED_Control(LED2,Led2_Flag);
}
```

**中断**

```c
extern int Led1_Flag;
extern int Led2_Flag;
extern int Led3_Flag;

extern int Led3_Happen;
extern int Led2_Happen;
int Led1_Delay=0;
int Led2_Delay=0;
int Led3_Delay=0;
int Led3_Count=0;
int Led2_Count=0;
void SysTick_Handler(void)
{
	TimingDelay--;
    //正常工作 1s闪烁一次
	if(++Led1_Delay==1000)
	{
		Led1_Delay=0;
		Led1_Flag=!Led1_Flag;
	}
    //状态发生了改变
	if(Led2_Happen)
	{
		if(++Led2_Delay==200)
		{
			Led2_Count++;
			Led2_Delay=0;
			Led2_Flag=!Led2_Flag;
		}
        //因为亮灭交替，所以是>10，  1，3，5，7，9亮灯
		if(Led2_Count>10)
		{
			Led2_Happen=0;
			Led2_Count=0;
		}
	}
    //接收到了数据
	if(Led3_Happen)
	{
		if(++Led3_Delay==200)
		{
			Led3_Count++;
			Led3_Delay=0;
			Led3_Flag=!Led3_Flag;
		}
		if(Led3_Count>10)
		{
			Led3_Happen=0;
			Led3_Count=0;
		}
	}
}
```

### 3.7主函数

```c
//...省略大部分初始化
	//这是之前说的先初始化液位等级
	Level_Init();
	Init_Data();
	while(1)
	{
        //分模块编程 所以主函数非常简洁
		Setting();
		Level_Control();
		Usart_Control();
		Led_Control();
	}
```

基本代码都已经在这了，论什么是真正的保姆级教学哈哈哈哈 ，就剩下一些界面刷新啊，EEPROM的存和读数据的，非常非常基础的代码。我觉得没必要再放了。

祝大家的编程水平蒸蒸日上哈。本次的结构我个人还算是比较满意的

源码链接：https://gitee.com/jason0131czy/embedded-blue-bridge-cup

