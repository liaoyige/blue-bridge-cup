/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_hal.h"
#include "lcd_hal.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define k1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define k2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define k3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define k4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)
#define key k1 | (k2<<1) | (k3<<2) | (k4<<3) | 0xf0

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
_Bool ui = 0;
_Bool led1_Flag = 1;

RTC_TimeTypeDef T = {0};
RTC_DateTypeDef D = {0};

float volt = 0;
float k = 0.1;


uint8_t tag;
uint8_t cnt;
uint8_t Key3_Flag = 0;
_Bool light_Flag = 1;
uint8_t RX_pointer = 0;
uint8_t tx_Buffer[20];
uint8_t rx_Buffer[20];

uint8_t led_ctrl = 0x00;
uint8_t LED_State[4] = "OFF";
uint8_t Time_Set[3] = {0};
uint8_t Time_Set_vaild[3] = {0};
uint8_t rx_data[2];


uint32_t LCD_tick = 0;
uint32_t LED_tick = 0;
uint32_t Key_tick = 0;
uint32_t TX_tick = 0;
uint32_t RX_tick = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_Disp(uint8_t led);
void TX_Proc(void);
void RX_Proc(void);
void RTC_Proc(void);
void ADC_Proc(void);
void LCD_Proc(void);
void LED_Proc(void);
void Key_Proc(void);
void Key_Read(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
struct __FILE
{
  int handle;
};

FILE __stdout;
int fputc(int ch, FILE *f) 
{
  HAL_UART_Transmit(&huart1,(u8 *)&ch,1,50);
  return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	RX_tick = uwTick;
	
	rx_Buffer[RX_pointer++] = rx_data[0];
	HAL_UART_Receive_IT(&huart1,rx_data,1);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC2_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	I2CInit();
	LCD_Init();
	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);

	printf("12345");
	HAL_UART_Receive_IT(&huart1,rx_data,1);
//	EEPROM_Write(0x01,1);
	k = EEPROM_Read(0x01) / 10.0f;
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  LCD_Proc();
	  Key_Proc();
	  LED_Proc();
	  ADC_Proc();
	  RTC_Proc();
	  TX_Proc();
	  RX_Proc();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void RTC_Proc(void)
{
	HAL_RTC_GetTime(&hrtc,&T,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&D,RTC_FORMAT_BIN);
}

void ADC_Proc(void)
{
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2,2);
	volt = HAL_ADC_GetValue(&hadc2) * 3.3f / 4095.0f;
	HAL_ADC_Stop(&hadc2);
}

void TX_Proc(void)
{
	if(uwTick - TX_tick < 500) return;
	TX_tick = uwTick;
	if(T.Hours == Time_Set_vaild[0] && T.Minutes == Time_Set_vaild[1] && T.Seconds == Time_Set_vaild[2] )
	{
		printf("%4.2f+%3.1f+%02d%02d%02d\r\n",volt,k,Time_Set_vaild[0],Time_Set_vaild[1],Time_Set_vaild[2]);
	}
}

void RX_Proc(void)
{
	if(uwTick - RX_tick <50) return;
	RX_tick  = uwTick;
	if((rx_Buffer[0] == 'k') && (rx_Buffer[1] == '0') && (rx_Buffer[2] == '.') && (RX_pointer == 6) && (rx_Buffer[4] == '\\')&& (rx_Buffer[5] == 'n'))
	{
		
		if(rx_Buffer[3] < '9' && rx_Buffer[3] > '0')
		{
			k = (rx_Buffer[3] - '0') / 10.0f;
			printf("ok\r\n");
			EEPROM_Write(0x01,rx_Buffer[3] - '0');
		}
		else
		{
			printf("请输入正确的数字\r\n");
		}
		
	
	}
	else if(RX_pointer > 0)
	{
		printf("输入格式错误\r\n");
	}
	RX_pointer = 0;
	memset(rx_Buffer,'\0',sizeof(rx_Buffer));
}

void LCD_Proc(void)
{
	if(uwTick - LCD_tick < 200) return;
	LCD_tick = uwTick;
	
	uint8_t LCD_Buffer[20];
	if(ui == 0)
	{
		sprintf((char *)LCD_Buffer,"  V1:%4.2fV          ",volt);
		LCD_DisplayStringLine(Line1,LCD_Buffer);
		sprintf((char *)LCD_Buffer,"  k:%3.1f            ",k);
		LCD_DisplayStringLine(Line3,LCD_Buffer);
		if(led1_Flag == 1)
		{
			sprintf((char *)LED_State,"ON");
		}
		else
		{
			sprintf((char *)LED_State,"OFF");
		}
		sprintf((char *)LCD_Buffer,"  LED:%s             ",LED_State);
		LCD_DisplayStringLine(Line5,LCD_Buffer);
		sprintf((char *)LCD_Buffer,"  T:%02d-%02d-%02d           ",T.Hours,T.Minutes,T.Seconds);
		LCD_DisplayStringLine(Line7,LCD_Buffer);
		LCD_DisplayStringLine(Line9,(u8*)"                   1");
	}
	else 
	{
		LCD_DisplayStringLine(Line2,(u8 *)"       Setting      ");
		if(light_Flag)
		{
			sprintf((char *)LCD_Buffer,"      %02d-%02d-%02d       ",Time_Set[0],Time_Set[1],Time_Set[2]);
			light_Flag = !light_Flag;
		}
		else if(light_Flag == 0 && Key3_Flag == 0)
		{
			sprintf((char *)LCD_Buffer,"        -%02d-%02d       ",Time_Set[1],Time_Set[2]);
			light_Flag = !light_Flag;
		}
		else if(light_Flag == 0 && Key3_Flag == 1)
		{
			sprintf((char *)LCD_Buffer,"      %02d-  -%02d       ",Time_Set[0],Time_Set[2]);
			light_Flag = !light_Flag;
		}
		else if(light_Flag == 0 && Key3_Flag == 2)
		{
			sprintf((char *)LCD_Buffer,"      %02d-%02d-         ",Time_Set[0],Time_Set[1]);
			light_Flag = !light_Flag;
		}
//		sprintf((char *)LCD_Buffer,"      %02d-%02d-%02d       ",Time_Set[0],Time_Set[1],Time_Set[2]);
		LCD_DisplayStringLine(Line5,LCD_Buffer);
		LCD_DisplayStringLine(Line9,(u8 *)"                   2");
	}
}

void Key_Read(void)
{	
	uint8_t Key_val = (key) ^ 0xff;
	tag = Key_val & (Key_val ^ cnt);
	cnt = Key_val;
}

void Key_Proc(void)
{
	if(uwTick - Key_tick < 20) return;
	Key_tick = uwTick;
	Key_Read();
	
	if(tag & 0x01)
	{
		led1_Flag = !led1_Flag;
	}
	if(tag & 0x02 )
	{
		ui = !ui;
		LCD_Clear(Black);
		Time_Set_vaild[0] = Time_Set[0];
		Time_Set_vaild[1] = Time_Set[1];
		Time_Set_vaild[2] = Time_Set[2];
	}
	if(tag & 0x04 && ui)
	{
		Key3_Flag = (Key3_Flag + 1) % 3;
	}
	if(tag & 0x08 && ui)
	{
		Time_Set[Key3_Flag]++;
			if(Time_Set[0] == 24)
				Time_Set[0] = 0;
			if(Time_Set[1] == 60)
				Time_Set[1] = 0;
			if(Time_Set[2] == 60)
				Time_Set[2] = 0;
	}
}

void LED_Disp(uint8_t led)
{
	
	HAL_GPIO_WritePin(GPIOC,0xff00,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,led<<8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}

void LED_Proc(void)
{
	if(uwTick - LED_tick < 200)  return;
	LED_tick = uwTick;
	if((volt > 3.3f * k) && led1_Flag)
	{
		led_ctrl ^=  0x01; 
	}
	else
	{
		led_ctrl = 0x00;
	}
	LED_Disp(led_ctrl);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
