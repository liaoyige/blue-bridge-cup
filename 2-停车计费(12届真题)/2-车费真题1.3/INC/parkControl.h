#include "string.h"
#include "stdlib.h"
#include "usart.h"
#define  PARKS_SIZE  8


//车辆结构体
typedef struct Car {
	char type[5];
	char id[5];
}Car;
//时间结构体
typedef struct Time {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}Time;
//停车场结构体
typedef struct Parking {
	Car car;
	Time time;
	int size;
	int canPark;
}Parking;
//初始化停车场
void initParking(Parking* parking);
//初始化车辆
Car initCar(char* id, char* type);
//初始化时间
Time initTime(int year, int month, int day, int hour, int minute, int second);
//能否停车
int canPark(Parking parking);
//是否存在相同车辆  存在返回1
int isExist(Car oldCar, Car newCar);
//停车 
int park(Parking* parking, Car car, Time time);
//出库 return：金额   返回-1的话就是找不到这辆车 那么就可以停车
double goOut(Parking* parking, Car car, Time time, float cnbrPrice, float vnbrPrice);
//成功出库时的系列操作
void successGoOut(Parking* parking,int location);
//整体流程
void parkControl(Parking *parking,Car car,Time time);

