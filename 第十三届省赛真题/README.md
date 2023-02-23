# 1 题目

![image-20220502141530953](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/image-20220502141530953.png)

![image-20220502141717367](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/image-20220502141717367.png)

![image-20220502141729800](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/image-20220502141729800.png)

![image-20220502141737807](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/image-20220502141737807.png)

# 2 分析

总的来说是一道逻辑非常简单的题目，整体的难度都比较低。看到后马上就能想到思路的那种。只有一部分的小细节需要注意

源码在结尾的链接各位可以自行下载

我的思路是 

- 把三位密码拆成三个变量，省的到时候取个数，十位数，百位数比较麻烦。

- 通过按键的输入进入不同的函数进行操作
- 通过Systick进行标志位的控制，实现题目里要求的5s的操作。

# 3 项目结构

大致就是利用这些变量完全总体的逻辑

```c
//输入的密码
int b1=1,b2=2,b3=3;
//设置的密码
int password1 = 1;
int password2 = 2;
int password3 = 3;
//错误相关记录
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
```

## 3.1 LCD显示部分

两个不同的界面展示，其中PSD界面的展示我使用了一个**init_flag来控制展示的字符。**

因为我用的b1那些的是整形，所以不想做转换什么的了。

直接用一个标志位控制显示，输入密码的时候只要 **init_flag=0**，就会切换到正常的显示数字

```c
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
```

## 3.2 按键控制部分

这里要注意这个&&lcd_mode==1，因为题目告诉我们只有处于pwd界面的时候，key1，2，3才有效。

所以我们&&上一个lcd_mode(该标志位==1)的时候显示的是界面1。

意味着如果是界面2，这个if不会成立

```c
void KEY_Control()
{
	char key = KEY_Scan();
	//如果键值为1，2，3并且处于输入密码界面
	if((key=='1' || key=='2' || key=='3')&&lcd_mode==1)
	{
		init_flag=0;
        //输入密码
		Input_Password(key);
	}
	if(key=='4')
	{
        //提交密码
		Commit_Password();
	}
}
```

## 3.3 密码部分

Input_Password() 输入密码部分没什么好说的，判断flag进行对应的++以及bug处理就好了。

Commit_Password() 的提交部分，因为如果密码正确的话要切换界面。

那么切换完我们先进行一次清屏，并且按照题目要求的

- 点亮LED1，切换屏幕,init_flag = 1（这是为了切换回去后显示出那些 ‘@’）
- 密码正确时 
  - error_record(错误次数),error_flag(错误标志)置为0
  - 输出2khz的10%占空比的方波
- 密码错误时
  - 题目要求大于等于3次要闪烁，所以我们利用error_record进行错误次数的记录。
  - 大于等于3后，错误标志位置1，systick的中断函数就会进行相关的变量控制，后面马上会提到。

```c
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
```

## 3.4 LED部分

刚刚我们提到了，**界面SDA的时候是密码正确，要开灯，那么与之对应的，在界面PWD的时。自然就要关灯**

我们可以利用显示界面的那个标志位 也就是 lcd_mode进行控制，将其关闭

这里的 **led2_flag** 其实就是为了完成密码错误三次以及以上时的**间隔100ms闪烁**

他的数值会在systick的中断函数里进行控制，3.7部分会讲到。别着急

```c
void Led_Control()
{
	if(lcd_mode==1)
	{
		LED_Control(LED1,0);
	}
	LED_Control(LED2,led2_flag);
}
```

## 3.5 数据处理部分

这个其实就没什么好说的，进行一点点简单的密码对比，然后将新密码覆盖上去就可以了

```c
void Data_Process()
{
	if(RXOVER==1)
	{
        //检验串口数据是否正确
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
int Check_Usart_Data()
{
	if(	 USART_RXBUF[0] - '0' == password1 && 
		 USART_RXBUF[1] - '0' == password2 && 
		 USART_RXBUF[2] - '0' == password3 && 
		 USART_RXBUF[3] == '-')
	{
		return 1;
	}
	return 0;
}
```

## 3.6 PWM控制部分

这里是因为，PWM波的这个TIM2_Init函数，不要反复一直在while(1)里执行，那样波形会有问题，所以我的想法就是，设置一次后，马上清零标志，这样下次就不会执行到了PWM_Mode也是在中断里赋值的，后续马上来。

```c
void PWM_Control()
{
	if(PWM_Mode==1)
	{
		TIM2_Init(999,71);
        TIM_SetCompare2(TIM2,500);
	}
	PWM_Mode = 0;
}
```

## 3.7 Systick中断部分

我就直接理由按照注释打到里面吧，不然不好描述

```c
extern int lcd_mode;
extern int led2_flag;
extern int error_record;
extern int error_flag;
extern int PWM_Mode;
//5s倒计时
int change_second = 5000;
//100ms闪烁
int error_delay = 100;
//闪烁50次,也就是闪烁5s
int led3_count = 50;
void SysTick_Handler(void)
{
   	//界面2的时候 
	if(lcd_mode==2)
	{
		change_second--;
		if(change_second==0)
		{
			change_second=5000;
            //5s到了，切换回界面1,PWM波也恢复1000，占空比百分之50
			lcd_mode = 1;
			PWM_Mode = 1;
		}
	}
    //密码输错三次或者以上时
	if(error_flag)
	{
        //100ms闪烁
		if(--error_delay==0)
		{
            //50次慢慢减
			led3_count--;
            //每间隔100ms，都会对led2_flag的标志位取反，对应前面的3.4led控制部分
			led2_flag = !led2_flag;
			error_delay = 100;
		}
        //闪烁完毕，标志位清0
		if(led3_count==0)
		{
			led3_count = 50;
			error_flag = 0;
		}
	}
	TimingDelay--;
}
```

## 3.8 主函数While(1)

```c
	while(1)
	{	
		LCD_Show();
		KEY_Control();
		Led_Control();
		Data_Process();
		PWM_Control();
	}
```



[源码链接：]: https://gitee.com/jason0131czy/embedded-blue-bridge-cup/tree/master/%E7%AC%AC%E5%8D%81%E4%B8%89%E5%B1%8A%E7%9C%81%E8%B5%9B%E7%9C%9F%E9%A2%98

