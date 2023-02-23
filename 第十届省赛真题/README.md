# 蓝桥杯嵌入式第十届省赛真题

[TOC]



## 1.题目分析

总的来说这题考点特别的少，逻辑也比我之前发的12届的停车计费简单得多，还是一样 **代码结尾自取。完全免费**

相对来说能从这题学到的。对我来说我觉得是 **封装一些“状态”数组**。可以让代码的可读性和复用性高很多。

**思路其实很简单，就是切换界面和获取adc的值，并和上下限比较进行对应操作。**
源码在末尾

![IvVBp4.md.jpg](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/305c67eeddb209e1c66d3c2d2abbfc0d.png)

![IvVakT.md.jpg](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/f1ab2d14e4762271473fa4b3e284eccf-16500256574083.png)

![IvVdtU.md.jpg](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/e9130b7bcbc55ff7379942b0d5d82136.png)

![IvVwhF.md.jpg](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/08e3735fa31ac4dbe1b810a5a30edd16.png)

## 2.项目结构

变量设计：

```c
u32 TimingDelay = 0;

//切换设置函数
void setting();
void Delay_Ms(u32 nTime);
//更新
void Upate_Data();
//渲染主界面
void Fflush_Main();
//渲染设置界面
void Fflush_Setting();
//Key_4，3的对应控制这里的flag我觉得用的挺好的
//后面大家会看到 
void Key4_Control(int flag);
void Key3_Control(int flag);
//高光展示
void DymaicShow(int flag);
//控制状态
void Status_Control()

//展示数组(个人习惯)
char MaxVolDisplay[24];
char MinVolDisplay[24];
char UpperDisplay[24];
char LowerDisplay[24];
char StatusDisplay[24];
char VolDisplay[24];
//状态数组
char *Status[8]={"Normal","Upper  ","Lower  "};
//LED灯数组 这里也非常有用，可以和后面的led灯编号对照
uint16_t LED_Arr[9]={0,LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8};
//电压
double Vol = 3.22;
double MaxVol = 2.4;
double MinVOl = 1.2;
//LED编号
int UpperLED = 1;
int LowerLED = 2;
//状态值 和状态数组比较
int Status_Flag = 0;
```

### 2.1数组思路

首先先来聊聊**为什么这里的LED要利用编号** 而不是规定死字符串 “LD1”，“LD2”这样的。因为如果规定死字符串的话，到时候报警灯点亮的时候，还需要解析字符串取出最后一位的数字，麻烦

我们不如换一种思路 **规定字符串为“LD”而我们在后面动态的拼接LED的编号**。也就是这样

```c
sprintf(UpperDisplay," Upper:LD%d",UpperLED);
```

就做到了利用UpperDisplay动态展示对应的数据。并且UpperLED可以用来后面点亮灯。

这也是为什么我要把LED封装成数组。

我们可以轻易的做到   **利用UpperLED作为LED数组的下标，点亮对应的LED**    这样点亮就非常方便了，而不需要if判断。如下

(这里的flag是用来交替闪烁的，200ms的时候 flag会自己取反，做到亮灭交替)

```c
LED_Control(LED_Arr[UpperLED],Led_Flag);
```

那么前面的Status_Flag和char *Status[8]也是同理了；

这里我们就可以引出我的第一个函数

Status_Control.c

```c
void Status_Control()
{
	if(Vol>MaxVol)
	{
        //这里的标志位是到时中断里200ms取反一次的 做到间断闪烁
        LED_Control(LED_Arr[UpperLED],Led_Flag);
		Status_Flag=1;
		return;
	}
	if(Vol<MinVOl)
	{
        LED_Control(LED_Arr[LowerLED],Led_Flag);
		Status_Flag=2;
		return;
	}
	Status_Flag=0;
}
```

可以很清晰的通过代码看出。不同的Status_Flag 到时就会动态的控制char *Status[8]展示他对应的该有的变量

```c
sprintf(StatusDisplay,"  Status:%s",Status[Status_Flag]);
LCD_DisplayStringLine(Line5,StatusDisplay);
```

### 2.2Key_Flag控制对应逻辑

重点就在这四个函数

```
void Key4_Control(int flag);
void Key3_Control(int flag);
void DymaicShow(int flag);
```

分析题目我们可以读出，在Setting界面修改的时候，需要在最大电压值，最小电压值，最大电压LED编号和最小电压LED编号。这

**四个地方**切换，并通过KEY3，KEY4控制他们的加减。

他们的共性就是 4 个位置 对应四个不同的操作模式和操作逻辑。

(高光+KEY4+KEY3)*4个位置 那么有12种情况，为此写12个函数未免太过麻烦了。

我们抽象的来想，可以是   **3个操作，各自对应4个模式** 。 **把三个操作抽象成函数**。

而**三个操作里面对应的四种情况其实是平行关系**的。

这个平行关系我们就用Flag替代。例如flag为1，执行三个函数里各自的情况1，flag2执行三个函数里各自的情况2。

而按键2 就是我们用来触发Flag改变的，逻辑如下（14-30行）

如此我们的代码耦合性就会降低 ，函数各司其职。

```c
//切屏函数
void setting()
{
	char key;
	int flag=0;
	LCD_Clear(White);
	Fflush_Setting();
	
	while(1)
	{
		key = KEY_Scan();
		Vol=Get_Adc();
		Status_Control();
		DymaicShow(flag);
		switch(key)
		{
            //返回主界面
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
```

### 2.3KEY控制操作

利用前面的操作，这里我们就可以做不同对应的情况了，其实我觉得应该用switch case会更加好一点，结构更加清晰，但是当时写完懒得改了。

```c
void Key3_Control(int flag)
{
    //最大电压值
	if(flag==0)
	{
		//浮点数计算精度问题
		if(MaxVol+0.3<3.300000000000001)
		{
			MaxVol+=0.3;
			return;
		}
		MaxVol=3.3;
	}
    //最小电压值
	else if(flag==1)
	{
		if(MinVOl+0.3<3.300000000000001)
		{
			MinVOl+=0.3;
			return;
		}
		MinVOl=3.3;
	}
    //最大电压值LED编号
	else if(flag==2)
	{
        //这里要先关灯，不然警告闪灯的时候，假设LED1亮还没熄灭的时候，
        //我们按下Key3切换，那么LED2闪烁，LED1还在亮
		LED_Control(LEDALL,0);
        //最大编号
		if(UpperLED==8)
		{
			UpperLED=8;
			return;
		}
        //如果往上+1的时候，和最小电压编号的LED编号相等的时候
        //那么我们需要过这个编号，让两个编号不相等
		if(UpperLED+1==LowerLED)
		{
            //但是如果跨过LowerLED的编号后会超过8，那么也不行的哦
			if(UpperLED+1==8)
			{
				return;
			}
            //逻辑完成，直接跨越
			UpperLED+=2;
			return;
		}
        //其他情况正常+1,后续最小电压那些也是同理，我就不写那么多注释了
		UpperLED++;
	}
    //最小电压值LED编号
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
```

KEY4_Control也是这个道理。这里就不复制展示了，代码文章结尾自取



高亮部分我来求解一下，大家有没有更好的解法，我只会这种最笨的方式

```c
void DymaicShow(int flag)
{
	LCD_SetBackColor(White);
	if(flag==0)
	{
		LCD_DisplayStringLine(Line1, "      Setting");
		LCD_DisplayStringLine(Line4,MinVolDisplay);
		LCD_DisplayStringLine(Line5,UpperDisplay);
		LCD_DisplayStringLine(Line6,LowerDisplay);
        //单独某一行刷新改色
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
```

最后就是这个中断里取反的操作 很简单的

```c
void SysTick_Handler(void)
{
	TimingDelay--;
	//低于下限电压
	if(Status_Flag==2)
	{
		if(++Led_Dealy==200)
		{
			Led_Flag= !Led_Flag;
			Led_Dealy=0;
		}
	}
	//高于上限电压
	else if(Status_Flag==1)
	{
		if(++Led_Dealy==200)
		{
			Led_Flag= !Led_Flag;
			Led_Dealy=0;
		}
	}
}
```

整体和逻辑有关的代码大概就是这样了

我个人认为最好的地方就是我用了数组和对应的Flag标识，来和数组组成映射关系，在代码里的很多地方都因此受益。还是个小萌新哈，如果大家有更好的方式也欢迎大家提出。

链接 https://gitee.com/jason0131czy/embedded-blue-bridge-cup