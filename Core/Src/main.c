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
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "rgb565.h"
#include "font6x9.h"
#include "hagl.h"
#include "menu.h"
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
#include "sdcard.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int __io_putchar(int ch) {
	if (ch == '\n') {
		__io_putchar('\r');
	}

	HAL_UART_Transmit(&huart6, (uint8_t*) &ch, 1, HAL_MAX_DELAY);

	return 1;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//volatile int16_t currentMenuIndex = 0;
volatile int16_t menuMaxIndex = MAX_MAIN_MENU_INDEX; //max number of selectable items on screen - menu
volatile uint8_t encoderBtnFlag;	//flag for button interrupt
uint8_t previousItem;		//needed for correct display of selected menu item
uint8_t currentItem;

volatile uint8_t activeScreen = 0;	//curent screen that is being shown

uint8_t activeChannels = 4;
uint8_t oversamplingPrescaler = 1;

char debug_text[25]; //bottom line text, just for debug
uint16_t sensor_data[8];			//raw adc data
uint16_t summed_data[8];			//accu for adc data, needed for oversampling
uint16_t averaged_data[8];			//averaged adc data

uint8_t datetime[12] = { 2, 0, 2, 1, 1, 2, 1, 5, 1, 7, 5, 9 }; //template datetime
uint8_t datetimeIndex;
uint8_t mounted = 0;
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
  MX_USART6_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	printf("Starting..\n");

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) sensor_data,
			sizeof(sensor_data) / sizeof(int16_t));
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	lcd_init();
//	FATFS fs;
//	FRESULT fresult;
//	fresult = f_mount(&fs, "/", 1);
//	if (fresult != FR_OK) printf("ERROR!!! in mounting SD CARD...\n\n");
//	else printf("SD CARD mounted successfully...\n\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		//************************CHANGING SCREENS************************
		switch (activeScreen) {
		case 0:  //ADC data screen
			for (int currentIter = 0; currentIter < oversamplingPrescaler;
					++currentIter) {
				for (int currentChan = 0; currentChan < activeChannels;
						++currentChan) {
					summed_data[currentChan] += sensor_data[currentChan];
					assert_param(summed_data[currentChan] <= UINT16_MAX);
				}
				HAL_Delay(50);
			}
			for (int var = 0; var < activeChannels; ++var) {
				averaged_data[var] = summed_data[var] / oversamplingPrescaler;
				summed_data[var] = 0;
			}
			show_sensor_data(averaged_data, activeChannels);
			if (mounted)
				sd_card_update_file("POMIARY.txt", averaged_data);
			HAL_Delay(1000);
			printf("Sensor data case executed.\n");
			fflush(stdout);
			break;
		case 1:  //main menu screen
			currentItem = (__HAL_TIM_GET_COUNTER(&htim2) >> 1) % menuMaxIndex;
			assert(currentItem <= menuMaxIndex - 1);
			show_main_menu();
			select_item(previousItem, currentItem);
			if (previousItem != currentItem) {
				previousItem = currentItem;
			}
			printf("Main menu case executed..\n");
			fflush(stdout);
			break;
		case 11:  //active channels selection screen
			activeChannels = __HAL_TIM_GET_COUNTER(&htim2) >> 1;
			if (activeChannels < 1) {
				activeChannels = 1;
				__HAL_TIM_SET_COUNTER(&htim2, 2);
			}
			if (activeChannels > 8) {
				activeChannels = 8;
				__HAL_TIM_SET_COUNTER(&htim2, 16);
			}
			set_channels_value(activeChannels, rgb565(200, 200, 200));
			printf("Active channels case executed..\n");
			fflush(stdout);
			break;
		case 12:  //oversampling selection screen
			oversamplingPrescaler = __HAL_TIM_GET_COUNTER(&htim2) >> 1;
			if (oversamplingPrescaler < 1) {
				oversamplingPrescaler = 1;
				__HAL_TIM_SET_COUNTER(&htim2, 2);
			}
			if (oversamplingPrescaler > 15) {
				oversamplingPrescaler = 15;
				__HAL_TIM_SET_COUNTER(&htim2, 30); //encoder counts up to 19 so 19*2 is max available value rn
			}
			set_oversampling_prescaler(oversamplingPrescaler,
					rgb565(220, 220, 220));
			printf("Oversampling case executed...\n");
			fflush(stdout);
			break;
		case 14:
			currentItem = (__HAL_TIM_GET_COUNTER(&htim2) >> 1) % menuMaxIndex;
			assert(currentItem <= menuMaxIndex - 1);
			show_misc_menu();
			select_item(previousItem, currentItem);
			if (previousItem != currentItem) {
				previousItem = currentItem;
			}
			break;
		case 41:
			currentItem = __HAL_TIM_GET_COUNTER(&htim2) >> 1;
			switch (datetimeIndex) {
			//2021 12 26 23 59
			case 0:
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex] < 1) {
					datetime[datetimeIndex] = 1;
					__HAL_TIM_GET_COUNTER(&htim2) = 1 * 2;
				}
				if (datetime[datetimeIndex] > 2) {
					datetime[datetimeIndex] = 2;
					__HAL_TIM_GET_COUNTER(&htim2) = 2 * 2;
				}
				break;
			case 4: //month1
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex] > 1) {
					datetime[datetimeIndex] = 1;
					__HAL_TIM_GET_COUNTER(&htim2) = 1 * 2;
				}
				break;
			case 5: //month2
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex - 1] == 1) {
					if (datetime[datetimeIndex] > 2) {
						datetime[datetimeIndex] = 2; //for 10,11,12
						__HAL_TIM_GET_COUNTER(&htim2) = 2 * 2;
					}
				} else if (datetime[datetimeIndex - 1] == 0) {
					if (datetime[datetimeIndex] < 1) {
						datetime[datetimeIndex] = 1; //for 01,02,03,04,05,06,07,08,09
						__HAL_TIM_GET_COUNTER(&htim2) = 1 * 2;
					}
				}
				break;
			case 6: //day1
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex] > 3) {
					datetime[datetimeIndex] = 3;
					__HAL_TIM_GET_COUNTER(&htim2) = 3 * 2;
				}
				break;
			case 7: //day2
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex - 1] == 3) {
					if (datetime[datetimeIndex] > 1) {
						datetime[datetimeIndex] = 1; //for 30,31
						__HAL_TIM_GET_COUNTER(&htim2) = 1 * 2;
					}
				} else if (datetime[datetimeIndex - 1] == 0) {
					if (datetime[datetimeIndex] < 1) {
						datetime[datetimeIndex] = 1; //for 01,02,03,04,05,06,07,08,09
						__HAL_TIM_GET_COUNTER(&htim2) = 1 * 2;
					}
				}
				break;
			case 8: //hour1
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex] > 2) {
					datetime[datetimeIndex] = 2;
					__HAL_TIM_GET_COUNTER(&htim2) = 2 * 2;
				}
				break;
			case 9: //hour2
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex - 1] == 2) {
					if (datetime[datetimeIndex] > 3) {
						datetime[datetimeIndex] = 3; //for 20,21,22,23
						__HAL_TIM_GET_COUNTER(&htim2) = 3 * 2;
					}
				}
				break;
			case 10: //minute1
				datetime[datetimeIndex] = currentItem % 10;
				if (datetime[datetimeIndex] > 5) {
					datetime[datetimeIndex] = 5;
					__HAL_TIM_GET_COUNTER(&htim2) = 5 * 2;
				}
				break;
			default:
				datetime[datetimeIndex] = currentItem % 10;
				break;
			}
			set_datetime_screen(datetimeIndex, datetime);
			break;
		case 42:
			show_sd_card_info();
			break;
		}
		//************************CHANGING SCREENS************************
		if (encoderBtnFlag) {
			encoderBtnFlag = 0;
			switch (activeScreen) {
			case 0: //sensor to menu
				__HAL_TIM_SET_COUNTER(&htim2, 0);
				activeScreen = 1;
				menuMaxIndex = MAX_MAIN_MENU_INDEX;
				break;
			case 1:	//menu to ...
				//menu to sensor
				if (currentItem == (menuMaxIndex - 1)) {
					activeScreen = 0;
					break;
				}
				//menu to activechannels
				else if (currentItem == 0) {
					__HAL_TIM_GET_COUNTER(&htim2) = activeChannels * 2; //you can't assign value to shifted value, hence no >> is used, silly me tried..
					activeScreen = 11;
					break;
				} else if (currentItem == 1) { //menu to oversampling
					__HAL_TIM_GET_COUNTER(&htim2) = oversamplingPrescaler * 2; //you can't assign value to shifted value, hence no >> is used, silly me tried..
					activeScreen = 12;
					break;
				} else if (currentItem == 3) { //menu to misc
					activeScreen = 14;
					__HAL_TIM_GET_COUNTER(&htim2) = 0;
					menuMaxIndex = MAX_MISC_MENU_INDEX;
					break;
				}
			case 11: //channels to main menu
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				activeScreen = 1;
				break;
			case 12: //sampling to main menu
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				activeScreen = 1;
				break;
			case 14: //misc menu to main menu
				if (currentItem == (menuMaxIndex - 1)) {
					menuMaxIndex = MAX_MAIN_MENU_INDEX;
					__HAL_TIM_GET_COUNTER(&htim2) = 0;
					activeScreen = 1;
					break;
				}
				if (currentItem == 0) {
					datetimeIndex = 0;
					__HAL_TIM_GET_COUNTER(&htim2) = 0;
					activeScreen = 41; //switch to datetime screen
					break;
				}
				if (currentItem == 1){
					activeScreen = 42; //switch to SD card info
					break;
				}
				if (currentItem == 2){
					sd_card_mount();
					sd_card_basic_write_to_file("POMIARY.txt");
					mounted = 1;
					break;
				}
				if (currentItem == 3){
					sd_card_unmount();
					mounted = 0;
					break;
				}
			case 41: //set datetime to misc menu
				//iterate over each symbol 2021-12-20 17:56 - 12 symbols to iterate through
				while (datetimeIndex >= 11) {
					activeScreen = 14;
					datetimeIndex = 0;
				}
				datetimeIndex++;
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				break;
			case 42:
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				activeScreen = 14;
				break;
			default:
				break;
			}
			printf("Button interrupt executed....\n");
			fflush(stdout);
		}

		snprintf(debug_text, 25, "MENU:%u | active:%d", currentItem,
				activeScreen);
		hagl_put_text(debug_text, 10, 145, rgb565(245, 245, 255), font6x9);
		lcd_copy();
		printf("Main while loop executed.............................\n");
		fflush(stdout);
		HAL_Delay(100);
	}


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

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
		HAL_Delay(50); //basic debounce
		while (HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin)
				== GPIO_PIN_RESET) {

		}
		encoderBtnFlag = 1;
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
