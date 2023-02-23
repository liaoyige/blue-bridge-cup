# 第十二届蓝桥杯嵌入式-停车计费

[TOC]

## 1.题目分析

刚看到题目时 大致看了一眼后，第一眼发现是串口部分比较不容易，其他的模块都很简单。串口的逻辑是主要难点，所以我首先先构想了一下串口部分的项目组成逻辑。然后才开始一步步做题。

![](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/20210506121738955.jpg)

![20210506121755650](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/20210506121755650.jpg)

![20210506121800805](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/20210506121800805.jpg)

![20210506121805398](C:\Users\czy\Desktop\21蓝桥杯\培训\2-停车计费(12届真题)\20210506121805398.jpg)

![20210506121810349](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/20210506121810349.jpg)

## 2.项目结构

基本的逻辑页面切换等这里就不多赘述主要是停车计费部分逻辑

### 2.1停车部分整体流程

我的整体思路如下：

 	1. 每过来一辆车 就先判断能不能 “出库”。如果不能出库的车，那么才有可能停的进车库
 	2. 如果这辆车能够出库，也就是当前车库已经存在这辆车，那么直接出库即可
 	3. 如果当前车辆不能出库，那么执行停车函数，判断有没有可以停车的车位进行停车即可

## 2.2串口数据解析流程

自己写了一个根据索引和长度截取字符串的函数

配合strcpy，atoi可以轻松的截取解析字符串

## 2.3细节部分

	1. 出库的车的时间，不能比入库时早
	2. 车辆类型只有CNBR和VNBR
	3. 时间的格式必须要正确，比如小时必须小于24，分钟必须小于等于60这种的
	4. 不满一小时按照一小时算

## 3.代码结构

### 3.1停车部分

结构如下 我采取结构体数组的方法模拟停车场，并分出时间结构体和停车结构体

![](https://coalball-code-image.oss-cn-chengdu.aliyuncs.com/%E8%93%9D%E6%A1%A5%E6%9D%AF/%E5%81%9C%E8%BD%A6%E5%A4%B4%E6%96%87%E4%BB%B6.png)

整理出其中的关键代码进行讲解

停车 park.c

```c
//停车
int park(Parking* parking, Car car, Time time)
{
	int i = 0;
	int typeFlag;
	//没位置了
	if(remainParkSize<=0)
	{
			return 0;
	}
	//判断车辆类型
	if (strcmp(car.type, "CNBR")==0)
	{
	  	typeFlag=1;
	}
		if (strcmp(car.type, "VNBR")==0) 
	{
			typeFlag=2;
	}
	for (i = 0; i < 8; i++)
	{
		//判断能否停车
		if (canPark(parking[i]))
		{
			//CNBR
			if(typeFlag==1)
			{
				remainParkSize--;
				cnbrSize++;
			}
			//VNBR
			if(typeFlag==2)
			{
				
				remainParkSize--;
				vnbrSize++;
			}
			//车位不够的时候关灯
			if(remainParkSize<=0)
			{
					LED_Control(LEDALL,0);
			}
			//到这就是成功停车了 结构体内赋值
			strcpy((parking[i].car).id, car.id);
			strcpy((parking[i].car).type, car.type);
			parking[i].time.year = time.year;
			parking[i].time.month = time.month;
			parking[i].time.day = time.day;
			parking[i].time.hour = time.hour;
			parking[i].time.minute = time.minute;
			parking[i].time.second = time.second;
			//该车位置为不能停车
			parking[i].canPark = 0;
			return 1;
		}
	}
	return 0;
}
```

出库 goOut.c

```c
double goOut(Parking* parking, Car car, Time time, float cnbrPrice, float vnbrPrice)
{
	int i = 0;
	int typeFlag = 0;
	double price = 0;
	Car oldCar;
	Time oldTime;
	//判断车辆类型
	if (strcmp(car.type, "CNBR")==0)
	{
		price = cnbrPrice;
	  	typeFlag=1;
	}
	if (strcmp(car.type, "VNBR")==0)
	{
		price = vnbrPrice;
		typeFlag=2;
	}
	for (i = 0; i < 8; i++)
	{
		oldCar = parking[i].car;
		oldTime = parking[i].time;
		//如果该位置可以停车，那肯定和出库没有关系 直接不进行下一步判断
		if (parking[i].canPark == 1)
		{
			continue;
		}
		//如果存在相同的车辆 那么可以停车
		if (isExist(oldCar, car))
		{
			//CNBR
			if(typeFlag==1)
			{
				remainParkSize++;
				cnbrSize--;
			}
			//VNBR
			if(typeFlag==2)
			{
				remainParkSize++;
				vnbrSize--;
			}
			//计算停车的小时数
            //这里已经能够判断 年月日小时，分和秒要继续判断
			parkTime = (time.year - oldTime.year) * 365 * 24 +
								 (time.month - oldTime.month) * 30 * 24 +
								 (time.day - oldTime.day) * 24 +
								 (time.hour - oldTime.hour);
			//时间非法
			if(parkTime<0)
			{
				parkTime=0;
				return -2;
			}
			//年月日小时相等
			else if(parkTime==0)
			{
				//时间非法
				if((time.minute*60+time.second)-(oldTime.minute*60+time.second) < 0)
				{
					return -2;
				}
				//未满一小时
				else
				{
					successGoOut(parking,i);
					return price;
				}
			}
			else
			{
				sum = price * (float)parkTime;
			}	
            //成功出库
			successGoOut(parking,i);
			return sum;
		}
	}
	return -1;
}
```

综合以上两个函数，即可进行总的逻辑控制，里面的小函数，像判断车的存在啊，成功出库后的车位清0操作，开灯，以及判断同名车辆什么的都很简单，各位在我后续发的文件里自取即可。

这里我们来看看逻辑组合部分

parkControl.c

```c
//停车控制
void parkControl(Parking *parking,Car car,Time time)
{
	sum = goOut(parking,car,time,cnbrPrice,vnbrPrice);
	//车库中没找到这辆车 尝试停车
	if(sum==-1.0)
	{
		if(park(parking,car,time))
		{
				USART_SendString("Success parking car!\n");
		}
		else
		{
				USART_SendString("Error parking car!\n");
		}
	}
	//停车时长错误
	if(sum==-2)
	{
		USART_SendString("Error ParkTime\n");
	}
	//找到该车，出库
	if(sum>=0)
	{
			sprintf(sendComputer,"%s:%s:%dhour:%.2f$\n",type,id,parkTime,sum);
			USART_SendString(sendComputer);
	}
	return;
}
```

到这我们已经实现了停车逻辑的控制。也就是逻辑部分已经完成

### 3.2主函数部分

接下来大家来看看主函数的控制和串口解析，函数结构如下

```c
//刷新显示区
void fflushData();
void fflushPara();
void updateAll();
void show();

//串口解析
int analysis(char* data);
//接收数据操作
void receiveData();
//切割字符串
char* substring(char* ch,int pos,int length);
```

**首先最重要的先来看切割字符串**,其实就是利用指针的操作赋值。

这样后续我们切割串口接收的数据时就非常的轻松并且明了。

参数1是待切割的串，参数2是切割起始位置，参数3是切割长度

```c
char* substring(char* ch,int pos,int length)  
{  
    char* des=ch;  
    char* subch;  
    int i;  
    des=des+pos;  
    for(i=0;i<length;i++)  
    {  
        subch[i]=*(des++);  
    }  
    subch[length]='\0';
    return subch; 
} 
```

数据解析 

```c
int analysis(char* data)
{
    	//配合切割函数简单完成
		strcpy(type,substring(data,0,4));
		strcpy(id,substring(data,5,4));
		year = atoi(substring(data,10,2));
		month = atoi(substring(data,12,2));
		day = atoi(substring(data,14,2));
		hour = atoi(substring(data,16,2));
		minute = atoi(substring(data,18,2));
		second = atoi(substring(data,20,2));

		//清空接收区
		memset(USART_RXBUF,0,sizeof(USART_RXBUF));
		
    	//车辆类型限定
		if(strcmp(type,"VNBR")!=0 && strcmp(type,"CNBR")!=0)
		{
			USART_SendString("Error Type Format\n");
			return 0;
		}
    	//时间bug处理
		else if(month>12 || day>31 || hour>24 || minute>60 || second>60 )
		{
			USART_SendString("Error Time Format\n");
			return 0;
		}
}
```

利用这两个函数十分轻易的就做到了数据的解析和保存

这里最后我们只需要配合接收数据时的操作，就可以基本完成停车部分了

接收数据的操作如下

```c
//接收数据开始逻辑
void receiveData()
{
		if(RXOVER == 1)
		{
			RXOVER = 0;
			//调用解析数据
			if(analysis(USART_RXBUF))
			{
				//初始化车和时间
				car = initCar(id,type);
				time = initTime(year,month,day,hour,minute,second);
				//停车流程控制
				parkControl(parking,car,time);
			}
			//这里是为了保证我们在显示车位界面时的实时刷新(小处理 问题不大)
            //因为接收数据是无论在哪个界面时都要接收的，为了保证数据实时的刷新显示
            //利用这个标志位解决
			if(showFlag==0)
			{
				fflushData();
			}
			USART_ITConfig(USART2	,USART_IT_RXNE,ENABLE);
			return;
		}
		return;
}

```

界面刷新函数那些的，难度不是很大，到时候大家自取文章后的代码就可以

界面切换函数

```c

void show()
{
	LCD_Clear(White);
	fflushPara();
	while(1)
	{
		keyTemp=Key_Scan();
		switch(keyTemp)
		{
				//车位显示和费率设置的切换
				case '1':
					showFlag=!showFlag;
					LCD_Clear(White);
					return;
				//进到函数里就保证了题目要求的按键2和按键3只在设置界面有效
				case '2':
					vnbrPrice+=0.5;
					cnbrPrice+=0.5;
					fflushPara();
					break;	
				case '3':
					vnbrPrice-=0.5;
					cnbrPrice-=0.5;
					fflushPara();
					break;
		}
		//切换界面后也要照样接收数据进行处理，否则破坏了数据一致性
		receiveData();
	}
}
```

最后主函数如下

```c
while(1)
	{		
		keyTemp=Key_Scan();
		switch(keyTemp)
		{
			case '1':
				showFlag=!showFlag;
				show();
				break;
			case '4':
				pwmFlag = !pwmFlag;
  				TIM_Cmd(TIM3, pwmFlag);
				LED_Control(LED2,pwmFlag);
				break;
		}
		fflushData();
		//接收数据开始处理
		receiveData();
	}
```


https://gitee.com/jason0131czy/embedded-blue-bridge-cup
