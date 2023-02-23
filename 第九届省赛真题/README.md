# 蓝桥杯第九届省赛真题



[TOC]

## 1.题目

**1、LCD 显示**

​	LCD 显示存储位置、定时时间和当前状态。系统预留 5 个存储位置用于存储常用的定时时间。当定时器停止时，当前状态为 Standby；当系统正在设置时间时，当前状态为 Setting；当定时器运行时，当前状态为 Running，定时器暂停时，当前状态为 Pause。

![](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/20190223133909127.png)



**2、按键功能**

系统使用 4 个按键，B1、B2、B3 和 B4。

   	 按键 B1 为存储位置切换键。每按一次，存储位置依次以 1、2、3、4、5循环切换，切换后定时时间设定为当前位置存储的时间。
   	
   	  按键 B2 为时间位置（时、分、秒）切换键和存储键。短按 B2 键进入时间设置状态。每次短按 B2 键，设置位置以时、分、秒循环切换，并突出显示（高亮）当前位置；设置完后，长按 B2 键（超过 0.8 秒）把设置的时间存储到当前的存储位置，并推出设置状态。如果是临时设置定时时间，则不需存储，直接按定时器启动按键。
   	
   	   按键 B3 为时、分、秒（按键 B2 确定当前位置）数字增加键。每短按B3 一次，数字递增一次；按住 B3 超过 0.8 秒，则数字快速递增，直到松开B3 按键。数字递增时，超出范围则从头循环。

​		按键 B4 为定时器启动键。短按 B4，定时器启动，开始运行；运行期间短按 B4，暂停定时器，再短按 B4，恢复定时器运行；长按 B4（超过 0.8 秒），则取消定时器运行，回到 Standby 状态。



**3、PWM 输出和 LED 显示**

定时器运行时，PA6 口输出 PWM 信号，同时 LED 灯（LD1）以 0.5 秒的频率闪烁。PWM 信号频率为 1KHz，占空比为 80%。
定时器停止或暂停时，停止输入 PWM 信号，LED 灯灭。

**4、定时时间存储**

设定好的定时时间存储在 EEPROM 中。
掉电重启后，显示存储位置 1 的定时时间。

## 2.题目分析

**源码在结尾 完全免费开源！！！**

**源码在结尾 完全免费开源！！！**

**源码在结尾 完全免费开源！！！**

这道题我乍一看 ，欸 就这？这么简单？ 但是其实实现起来的时候发现和想象中的不一样，题目里有很多小坑要去处理，不然程序到处会有奇奇怪怪的bug，虽然这些bug大部分情况下不会太影响你的程序正常运行，但我们尽量还是把程序做到完美一点点吧

要注意几个小点

- 设置时间时，时间的跳动规则不能使用我们平时正常倒计时的规则，会有小bug。也就是我们在正常倒计时的地方要加一句限定，限定只有不在设置的时候才是正常倒计时的时间规则。

- 反正挺多地方都需要加上这个设置限定的，长短按的地方也有很多小细节要注意

- 改变状态的显示可以利用我之前的文章有提到的，详情参考

  [封装数组的思想]: https://blog.csdn.net/qq_51663309/article/details/121460829

  封装数组的方法去做，顺便再写一个控制所有状态的函数，可以方便很多，这里听起来好像有点抽象，后面看我代码就知道了

  总之这题 就是**在制作一个简单的倒计时上，增加功能，注意细节减少bug就可以了**。基础功能的实现是比较容易的

## 3.项目结构

### 3.1 函数结构与全局变量

```c
//时间
int hour = 0;
int minute = 1;
int second = 5;
//存储位置标志位
int storage_flag=1;
//状态数组和数组对应标志位以及当前状态
//其实这四个状态也可以封装成数组，但是我不想弄了
char *status[4] = {"Running","Pause","Standby","Setting"};
int status_flag = 2;
int is_standby = 1;
int is_setting = 0;
int is_start = 0;
int is_pause = 0;

//暂停和启动标志位(都是短按，所以标志位控制)
int pause_start_flag = 1;
//倒计时到了标志
int alarm_flag;
//led标志
int led1_flag;
//显示区
char NO_Disp[26];
char Time_Disp[26];
char Status_Disp[26];
//按键延时标志
int button2_delay=0;
int button3_delay=0;
int button4_delay=0;
//改变时还是分还是秒
int set_time_mode=0;
//显示函数
void Countdown_Display();
//倒计时控制
void Countdown_Time_Control();
//总按键控制
void Key_Control();
//KEY2：设置选择时分秒控制
void Setting();
//下划线
void Draw_Under_line(int mode);
//KEY3：设置时间
void Time_Setting(int mode);
//KEY4：定时器状态控制
void Timer_State_Control();
//LED控制
void Led_Control();
//状态控制(主要为了方便)
void Status_Control(int status_flag);
//读和存数据的东西
uint8_t Read_Data(uint8_t address);
void Write_Data(unsigned char address,unsigned char info);
void Read_Time(int storage_flag);
void Storage_Time(int storage_flag);
```

### 3.2 倒计时基础实现

这里就是之前说的 设置模式和正常时的两种情况要区分开，你肯定不希望设置倒计时的时候被进位什么的影响吧

```c
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
            //总不能minute为0了还继续减少分钟吧哈哈
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
    //时间到！！！
	if(minute==0 && second == 0 && hour==0)
	{
		alarm_flag = 1;
		if(is_setting==0)
		{
            //2状态对应之前数组的第三个元素 Standby，在status_control里会处理
			status_flag = 2;
			Status_Control(status_flag);
		}
	}
}
```

systick中断

```c
void SysTick_Handler(void)
{
	TimingDelay--;
	//一秒倒计时
	if(++Time_Flag==1000)
	{
		//限定只有运行的时候倒计时能走
		if(alarm_flag==0 && is_setting==0 && is_standby==0 && is_pause==0)
		{
			second--;
		}
		Time_Flag=0;
	}
	if(++Led_Delay==500)
	{
		led1_flag = !led1_flag;
		Led_Delay = 0;
	}
}
```

### 3.3 方便的状态控制函数

因为我们每次切换状态的时候，都要把当前状态置1，其他置0，每次都写4行太麻烦了，这里直接封装

而在我的LCD显示函数里，显示的那个状态也会由status的数组，传入status_flag达到同步显示的效果

```c
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
```

### 3.4 按键总控结构

```c
void Key_Control()
{
	char key = KEY_Scan();
	switch(key)
	{
        //切换存储的时间
		case '1':
            //切换存储位
			storage_flag++;
			if(storage_flag>5){  storage_flag=1;  }
            //改变状态
			Status_Control(storage_flag);
            //读取该位置存储时间
			Read_Time(storage_flag);
			break;
        //设置选择的时分秒
		case '2':
			Setting();
			break;
        //改变时分秒，set_time_mode在按键2里会改变
        //后续马上让大家看
		case '3':
			Time_Setting(set_time_mode);
			break;
        //定时器的状态控制
		case '4':
			Timer_State_Control();
			break;
	}
}
```

### 3.5 KEY2的设置功能

前面大家看到了Setting函数 那么我们看看他的实现体

```c
void Setting()
{
	char key = KEY_Scan();
    //去看对应数组的参量哈
    //更新定时器状态
	status_flag=3;
	Status_Control(status_flag);
	//长短按的实现
	if(KEY2==0)
	{
		button2_delay++;
		//短按
		if(button2_delay==1)
		{
			alarm_flag = 0;
            //三个模式 其实就是时分秒，后面该变量交付给KEY3控制
			set_time_mode++;
			if(set_time_mode>3)
			{
				set_time_mode=1;
			}
            //靠下划线让我们方便识别设置的时分秒
			Draw_Under_line(set_time_mode);
		}	
        //长按
		if(button2_delay>=4)
		{
            //确认松手，这个很重要，不然可能退出了马上外面扫描到key2
            //然后又进入了设置函数，烦得很，大家想想
			while(KEY2==0)
			//存储当前数据位置 报警置0(因为有可能定时器时间到了然后我们设置时间)
			alarm_flag = 0;
            //存储
			Storage_Time(storage_flag);
            //清除所有下划线
			LCD_SetTextColor(White);
			LCD_DrawLine(120,280,250,Horizontal);
			LCD_SetTextColor(Black);
            //更新状态
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
```

下划线的绘制函数没什么好说的，慢慢试坐标就是了，然后根据传入的参数分成三个if语句就可以了，这里就不展示了

### 3.6 KEY3的设置时间功能

```c
void Time_Setting(int mode)
{
    //注意到这里哈，这就是前面说的那些小细节
    //与上is_setting，保证只有设置时间的时候key3起作用
	if(KEY3==0 && is_setting)
	{
		button3_delay++;
		//短按
		if(button3_delay==1)
		{
			if(mode==1){hour++;}
			if(mode==2){minute++;}
			if(mode==3){second++;}
		}
		//长按
		if(button3_delay>=10)
		{
			if(mode==1){hour++;}
			if(mode==2){minute++;}
			if(mode==3){second++;}
		}
	}
	else
	{
		button3_delay=0;
	}
	
}
```

### 3.7 KEY4的状态设置功能

```c
void Timer_State_Control()
{
    //清除下划线(也是小细节处理)
	LCD_SetTextColor(White);
	LCD_DrawLine(120,280,250,Horizontal);
	LCD_SetTextColor(Black);
    //报警先清0，不然会被之前状态影响
	alarm_flag = 0;
	if(KEY4==0)
	{
		button4_delay++;
        //短按
		if(button4_delay==1)
		{
            //暂停和开始的转化
			if(pause_start_flag==1)
			{
                //一如既往调用这两行更新状态
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
        //长按
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
```

LED和Pwm的控制就太简单拉， is_start这个变量为1的时候才能使用就行了。其他时候就关闭 easy~。这里也不放了

### 3.8 存储部分

其实也很简单，分五个存储时间的地方就行了，注意地址不要重复

```c
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
```

取值倒是要注意，因为可能存在不是你的板子取到垃圾值的情况，所以我们可以这么做

当 取到的值 不在规定范围内的时候，我们先往里面写入一次值就好了

```c
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
        Storage_Time(1);
	}
	if(storage_flag==2)
	{
		hour = Read_Data(0x20);    Delay_Ms(2);
		minute = Read_Data(0x21);  Delay_Ms(2);
		second = Read_Data(0x22);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=0; }
		if(second>60 || second<0){ second=30; }
         Storage_Time(2);
	}
	if(storage_flag==3)
	{
		hour = Read_Data(0x30);    Delay_Ms(2);
		minute = Read_Data(0x31);  Delay_Ms(2);
		second = Read_Data(0x32);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=1; }
		if(second>60 || second<0){ second=10; }
         Storage_Time(3);
	}
	if(storage_flag==4)
	{
		hour = Read_Data(0x40);    Delay_Ms(2);
		minute = Read_Data(0x41);  Delay_Ms(2);
		second = Read_Data(0x42);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=1; }
		if(minute>60 || minute<0){ minute=2; }
		if(second>60 || second<0){ second=30; }
        Storage_Time(4);
	}
	if(storage_flag==5)
	{
		hour = Read_Data(0x50);    Delay_Ms(2);
		minute = Read_Data(0x51);  Delay_Ms(2);
		second = Read_Data(0x52);  Delay_Ms(2);
		if(hour>24 || hour<0){ hour=0; }
		if(minute>60 || minute<0){ minute=5; }
		if(second>60 || second<0){ second=20; }
        Storage_Time(5);
	}
}

```

### 3.9 主函数

```c
	//省略初始化
	//读取值，默认storage_flag==1
	Read_Time(storage_flag);
	while(1)
	{
        //LED
		Led_Control();
        //按键控制
		Key_Control();
        //显示
		Countdown_Display();
       	//时间逻辑校验
		Countdown_Time_Control();
	}
```



细节的坑真的多，可能代码还是有小bug，但是我感觉应该大部分都还ok了。希望我和大家代码能力越来越优秀呀！！！

下题见。

[源码]: https://gitee.com/jason0131czy/embedded-blue-bridge-cup
