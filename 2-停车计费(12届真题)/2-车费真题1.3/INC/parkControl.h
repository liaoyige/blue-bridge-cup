#include "string.h"
#include "stdlib.h"
#include "usart.h"
#define  PARKS_SIZE  8


//�����ṹ��
typedef struct Car {
	char type[5];
	char id[5];
}Car;
//ʱ��ṹ��
typedef struct Time {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}Time;
//ͣ�����ṹ��
typedef struct Parking {
	Car car;
	Time time;
	int size;
	int canPark;
}Parking;
//��ʼ��ͣ����
void initParking(Parking* parking);
//��ʼ������
Car initCar(char* id, char* type);
//��ʼ��ʱ��
Time initTime(int year, int month, int day, int hour, int minute, int second);
//�ܷ�ͣ��
int canPark(Parking parking);
//�Ƿ������ͬ����  ���ڷ���1
int isExist(Car oldCar, Car newCar);
//ͣ�� 
int park(Parking* parking, Car car, Time time);
//���� return�����   ����-1�Ļ������Ҳ��������� ��ô�Ϳ���ͣ��
double goOut(Parking* parking, Car car, Time time, float cnbrPrice, float vnbrPrice);
//�ɹ�����ʱ��ϵ�в���
void successGoOut(Parking* parking,int location);
//��������
void parkControl(Parking *parking,Car car,Time time);

