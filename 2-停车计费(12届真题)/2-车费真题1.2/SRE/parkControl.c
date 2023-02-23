#include "parkControl.h"

int parkTime;
double sum;

extern int remainParkSize ;
extern int cnbrSize ;
extern int vnbrSize ;

//初始化停车场
void initParking(Parking* parking)
{	
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		strcpy(parking[i].car.id, "");
		strcpy(parking[i].car.type, "");
		parking[i].size = 0;
		parking[i].time.year = 0;
		parking[i].time.month = 0;
		parking[i].time.day = 0;
		parking[i].time.hour = 0;
		parking[i].time.minute = 0;
		parking[i].time.second = 0;
		parking[i].canPark = 1;
	}
	
}

//初始化车辆
Car initCar(char* id, char* type)
{
	Car car;
	strcpy(car.id, id);
	strcpy(car.type, type);
	return car;
}

//初始化时间
Time initTime(int year, int month, int day, int hour, int minute, int second)
{
	Time time;
	time.year = year;
	time.month = month;
	time.day = day;
	time.hour = hour;
	time.minute = minute;
	time.second = second;
	return time;
}

//能否停车？能返回1
int canPark(Parking parking)
{
	if (parking.canPark == 1)
	{
		return 1;
	}
	return 0;
}

//是否存在同车辆 是的话返回1
int isExist(Car oldCar, Car newCar)
{
	if (strcmp(oldCar.type, newCar.type) == 0 && strcmp(oldCar.id, newCar.id) == 0)
	{
		return 1;
	}
	return 0;
}

//停车
int park(Parking* parking, Car car, Time time)
{
	int i = 0;
	int typeFlag;
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
		//判断该位置是否能停车
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
			strcpy((parking[i].car).id, car.id);
			strcpy((parking[i].car).type, car.type);
			parking[i].time.year = time.year;
			parking[i].time.month = time.month;
			parking[i].time.day = time.day;
			parking[i].time.hour = time.hour;
			parking[i].time.minute = time.minute;
			parking[i].time.second = time.second;
			//置为不能停车
			parking[i].canPark = 0;
			return 1;
		}
	}
	return 0;
}

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
		//判断该位置是否有车，没车的话直接不检查 跳过这个车位
		if (parking[i].canPark == 1)
		{
			continue;
		}
		//判断同名车辆是否存在
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
			//全局变量计算停车小时数 用于extern到主函数发送给上位机 
			//这里的parkTIme已经能够自动判断年月日小时的bug 但是无法判断分和秒 所以我们继续判断
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
			//年月日时 相等的时候
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
					//出库清空该车位信息
					strcpy(parking[i].car.id, "");
					strcpy(parking[i].car.type, "");
					parking[i].time.year = 0;
					parking[i].time.month = 0;
					parking[i].time.day = 0;
					parking[i].time.hour = 0;
					parking[i].time.minute = 0;
					parking[i].time.second = 0;
					parking[i].canPark = 1;
					return price;
				}
			}
			else
			{
				sum = price * (float)parkTime;
			}
			//出库清空该车位信息
			strcpy(parking[i].car.id, "");
			strcpy(parking[i].car.type, "");
			parking[i].time.year = 0;
			parking[i].time.month = 0;
			parking[i].time.day = 0;
			parking[i].time.hour = 0;
			parking[i].time.minute = 0;
			parking[i].time.second = 0;
			parking[i].canPark = 1;
			return sum;
		}
	}
	return -1;
}

