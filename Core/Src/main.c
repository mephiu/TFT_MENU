/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "rgb565.h"
#include "font6x9.h"
#include "hagl.h"
#include "menu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile int16_t currentMenuIndex = 0;
volatile int16_t menuMaxIndex = 0;
//int16_t lastMenuPos = 0;
//int16_t menu_pos[10];
//int16_t counterIndex;
//uint16_t tim_data[10];

volatile uint8_t activeScreen = 0;
volatile uint8_t sensorWindowFlag = 0;
volatile uint8_t menuWindowFlag = 0;
uint16_t sensor_data[8];
int8_t activeChannels = 4;
uint8_t oversamplingPrescaler = 1;

volatile uint16_t lastEncoderValue;
char debug_text[25];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) sensor_data, sizeof(sensor_data) / sizeof(int16_t));
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	//start LCD
	lcd_init();
	show_sensor_window();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (activeScreen == 0) {
			if (sensorWindowFlag == 0){
				sensorWindowFlag = 1;
				show_sensor_window();
			}
			show_sensor_data(sensor_data);
			HAL_Delay(500);
		}
		if (activeScreen == 1) {
			menuMaxIndex = 5;
			if (menuWindowFlag == 0){
				menuWindowFlag = 1;
				htim2.Instance->CNT = 1000;
				show_menu_window();
			}
			currentMenuIndex = (htim2.Instance->CNT / 4) % menuMaxIndex;
			select_item(currentMenuIndex);
			lcd_copy();
			HAL_Delay(50);
			deselect_item(currentMenuIndex);
		}
		if (activeScreen == 11){
//			if (lastEncoderValue != htim2.Instance->CNT){
//				activeChannels += (lastEncoderValue - htim2.Instance->CNT)/2;
//			}
			update_channels_value(activeChannels, rgb565(120, 120, 120));
//			lastEncoderValue = htim2.Instance->CNT;
		}
		snprintf(debug_text, 25, "MENU:%u | active:%d", currentMenuIndex, activeScreen);
		hagl_put_text(debug_text, 10, 145, rgb565(245, 245, 255), font6x9);
		lcd_copy();
		HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//			snprintf(tim_data, 10, "Tim:%u", htim2.Instance ->CNT);
		//			hagl_put_text(tim_data, 80, 50, rgb565(65, 95, 175), font6x9);
		//			snprintf(menu_pos, 10, "Tim:%d", (htim2.Instance->CNT / 2) % menuMaxIndex );
		//			hagl_put_text(menu_pos, 80, 50, rgb565(65, 95, 175), font6x9);
		//			if (lastMenuPos != currentMenuIndex) deselect_item(lastMenuPos);
		//			select_item(currentMenuIndex);
		//			lcd_copy();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi == &hspi1) {
		lcd_transfer_done();
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == ENC_BTN_Pin) {
		while (HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin)== GPIO_PIN_RESET){

		}
		HAL_Delay(50); //basic debounce
		switch (activeScreen) {
			//sensor window is active, gonna change to menu
			case 0:
				//reset counter
				menuWindowFlag = 0;
				activeScreen = 1;
				break;
			case 1:
				//menu window is active, gonna change to sensor
				if (currentMenuIndex == menuMaxIndex - 1) {
					sensorWindowFlag = 0;
					activeScreen = 0;
				}
				if (currentMenuIndex == 0){
//					htim2.Instance->CNT = 0;
					activeScreen = 11;
				}
				break;
			case 11:
				htim2.Instance->CNT = 1000;
				lastEncoderValue = htim2.Instance->CNT;
				activeScreen = 1;
			default:
				break;
		}
//		//sensor window is active, gonna change to menu
//		if (activeScreen == 0) {
//			//reset counter
//			menuWindowFlag = 0;
//			activeScreen = 1;
//
//		} else if (activeScreen == 1) {
//			//menu window is active, gonna change to sensor
//			if (currentMenuIndex == menuMaxIndex - 1) {
//				sensorWindowFlag = 0;
//				activeScreen = 0;
//			}
//
//
//		}
	}
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
	__disable_irq();
	while (1) {
	}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
