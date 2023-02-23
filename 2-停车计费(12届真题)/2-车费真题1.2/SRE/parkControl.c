#include "parkControl.h"

int parkTime;
double sum;

extern int remainParkSize ;
extern int cnbrSize ;
extern int vnbrSize ;

//��ʼ��ͣ����
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

//��ʼ������
Car initCar(char* id, char* type)
{
	Car car;
	strcpy(car.id, id);
	strcpy(car.type, type);
	return car;
}

//��ʼ��ʱ��
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

//�ܷ�ͣ�����ܷ���1
int canPark(Parking parking)
{
	if (parking.canPark == 1)
	{
		return 1;
	}
	return 0;
}

//�Ƿ����ͬ���� �ǵĻ�����1
int isExist(Car oldCar, Car newCar)
{
	if (strcmp(oldCar.type, newCar.type) == 0 && strcmp(oldCar.id, newCar.id) == 0)
	{
		return 1;
	}
	return 0;
}

//ͣ��
int park(Parking* parking, Car car, Time time)
{
	int i = 0;
	int typeFlag;
	if(remainParkSize<=0)
	{
			return 0;
	}
	//�жϳ�������
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
		//�жϸ�λ���Ƿ���ͣ��
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
			//��Ϊ����ͣ��
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
	//�жϳ�������
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
		//�жϸ�λ���Ƿ��г���û���Ļ�ֱ�Ӳ���� ���������λ
		if (parking[i].canPark == 1)
		{
			continue;
		}
		//�ж�ͬ�������Ƿ����
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
			//ȫ�ֱ�������ͣ��Сʱ�� ����extern�����������͸���λ�� 
			//�����parkTIme�Ѿ��ܹ��Զ��ж�������Сʱ��bug �����޷��жϷֺ��� �������Ǽ����ж�
			parkTime = (time.year - oldTime.year) * 365 * 24 +
								 (time.month - oldTime.month) * 30 * 24 +
								 (time.day - oldTime.day) * 24 +
								 (time.hour - oldTime.hour);
			//ʱ��Ƿ�
			if(parkTime<0)
			{
				parkTime=0;
				return -2;
			}
			//������ʱ ��ȵ�ʱ��
			else if(parkTime==0)
			{
				//ʱ��Ƿ�
				if((time.minute*60+time.second)-(oldTime.minute*60+time.second) < 0)
				{
					return -2;
				}
				//δ��һСʱ
				else
				{
					//������ոó�λ��Ϣ
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
			//������ոó�λ��Ϣ
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

