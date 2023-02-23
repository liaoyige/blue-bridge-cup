#include "volControl.h"

extern double Vol;
extern double MaxVol;
extern double MinVOl;

extern int Status_Flag;
extern uint16_t LED_Arr[9];
extern int UpperLED;
extern int LowerLED;
extern int Led_Flag;
void Status_Control()
{
	if(Vol>MaxVol)
	{
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
