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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int __io_putchar(int ch)
{
  if (ch == '\n') {
    __io_putchar('\r');
  }

  HAL_UART_Transmit(&huart6, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

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
volatile int16_t menuMaxIndex = 5;
volatile uint8_t encoderBtnFlag;
uint8_t previousItem;
uint8_t currentItem;

volatile uint8_t activeScreen = 0;

uint8_t activeChannels = 4;
uint8_t oversamplingPrescaler = 1;

char debug_text[25];
uint16_t sensor_data[8];
uint16_t temp_data[8];
uint16_t prepared_data[8];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;  // file system
FIL fil; // File
FILINFO fno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];  // to store strings..

int i=0;

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}

void clear_buffer (void)
{
	for (int i=0; i<BUFFER_SIZE; i++) buffer[i] = '\0';
}

void send_uart (char *string)
{
	uint8_t len = strlen (string);
	HAL_UART_Transmit(&huart6, (uint8_t *) string, len, HAL_MAX_DELAY);  // transmit in blocking mode
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
//*******************************************************SD_CARD FUNCTIONS**************************************
  HAL_Delay (500);

  fresult = f_mount(&fs, "/", 1);
  	if (fresult != FR_OK) send_uart ("ERROR!!! in mounting SD CARD...\n\n");
  	else send_uart("SD CARD mounted successfully...\n\n");


  	/*************** Card capacity details ********************/

  	/* Check free space */
  	f_getfree("", &fre_clust, &pfs);

  	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
  	sprintf (buffer, "SD CARD Total Size: \t%lu\n",total);
  	send_uart(buffer);
  	clear_buffer();
  	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
  	sprintf (buffer, "SD CARD Free Space: \t%lu\n\n",free_space);
  	send_uart(buffer);
  	clear_buffer();



  	/************* The following operation is using PUTS and GETS *********************/

  	/* Open file to write/ create a file if it doesn't exist */
      fresult = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  	/* Writing text */
  	f_puts("This data is from the FILE1.txt. And it was written using ...f_puts... ", &fil);

  	/* Close file */
  	fresult = f_close(&fil);

  	if (fresult == FR_OK)send_uart ("File1.txt created and the data is written \n");

  	/* Open file to read */
  	fresult = f_open(&fil, "file1.txt", FA_READ);

  	/* Read string from the file */
  	f_gets(buffer, f_size(&fil), &fil);

  	send_uart("File1.txt is opened and it contains the data as shown below\n");
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();




  	/**************** The following operation is using f_write and f_read **************************/

  	/* Create second file with read write access and open it */
  	fresult = f_open(&fil, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE);

  	/* Writing text */
  	strcpy (buffer, "This is File2.txt, written using ...f_write... and it says Hello from Controllerstech\n");

  	fresult = f_write(&fil, buffer, bufsize(buffer), &bw);

  	send_uart ("File2.txt created and data is written\n");

  	/* Close file */
  	f_close(&fil);



  	// clearing buffer to show that result obtained is from the file
  	clear_buffer();

  	/* Open second file to read */
  	fresult = f_open(&fil, "file2.txt", FA_READ);
  	if (fresult == FR_OK)send_uart ("file2.txt is open and the data is shown below\n");

  	/* Read data from the file
  	 * Please see the function details for the arguments */
  	f_read (&fil, buffer, f_size(&fil), &br);
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();


  	/*********************UPDATING an existing file ***************************/

  	/* Open the file with write access */
  	fresult = f_open(&fil, "file2.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);

  	/* Move to offset to the end of the file */
  	fresult = f_lseek(&fil, f_size(&fil));

  	if (fresult == FR_OK)send_uart ("About to update the file2.txt\n");

  	/* write the string to the file */
  	fresult = f_puts("This is updated data and it should be in the end", &fil);

  	f_close (&fil);

  	clear_buffer();

  	/* Open to read the file */
  	fresult = f_open (&fil, "file2.txt", FA_READ);

  	/* Read string from the file */
  	fresult = f_read (&fil, buffer, f_size(&fil), &br);
  	if (fresult == FR_OK)send_uart ("Below is the data from updated file2.txt\n");
  	send_uart(buffer);
  	send_uart("\n\n");

  	/* Close file */
  	f_close(&fil);

  	clear_buffer();


  	/*************************REMOVING FILES FROM THE DIRECTORY ****************************/

  	fresult = f_unlink("/file1.txt");
  	if (fresult == FR_OK) send_uart("file1.txt removed successfully...\n");

  	fresult = f_unlink("/file2.txt");
  	if (fresult == FR_OK) send_uart("file2.txt removed successfully...\n");

  	/* Unmount SDCARD */
  	fresult = f_mount(NULL, "/", 1);
  	if (fresult == FR_OK) send_uart ("SD CARD UNMOUNTED successfully...\n");


//*******************************************************SD_CARD FUNCTIONS**************************************
  	printf("Starting..\n");
  	fflush(stdout);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) sensor_data,
			sizeof(sensor_data) / sizeof(int16_t));
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
	//start LCD
	lcd_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		switch (activeScreen) {
		case 0:
			for (int currentIter = 0; currentIter < oversamplingPrescaler; ++currentIter) {
				for (int currentChan = 0; currentChan < activeChannels; ++currentChan) {
					temp_data[currentChan] += sensor_data[currentChan];
					assert_param(temp_data[currentChan] <= UINT16_MAX);
				}
				HAL_Delay(50);
			}
			for (int var = 0; var < activeChannels; ++var) {
				prepared_data[var] = temp_data[var] / oversamplingPrescaler;
				temp_data[var] = 0;
			}
			show_sensor_data(prepared_data, activeChannels);
			HAL_Delay(1000);
			printf("Sensor data case executed.\n");
			fflush(stdout);
			break;
		case 1:
			currentItem = (__HAL_TIM_GET_COUNTER(&htim2)>>1) % menuMaxIndex;
			assert(currentItem <= menuMaxIndex-1);
			show_menu_window();
			deselect_item(previousItem);
			select_item(currentItem);
			if (previousItem != currentItem) {
				previousItem = currentItem;
			}
			printf("Main menu case executed..\n");
			fflush(stdout);
			break;
		case 2:
			activeChannels = __HAL_TIM_GET_COUNTER(&htim2)>>1;
			if (activeChannels < 1){
				activeChannels = 1;
				__HAL_TIM_SET_COUNTER(&htim2, 2);
			}
			if (activeChannels > 8) {
				activeChannels = 8;
				__HAL_TIM_SET_COUNTER(&htim2, 16);
			}
			update_channels_value(activeChannels, rgb565(200, 200, 200));
			printf("Active channels case executed..\n");
			fflush(stdout);
			break;
		case 3:
			oversamplingPrescaler = __HAL_TIM_GET_COUNTER(&htim2)>>1;
			if (oversamplingPrescaler < 1) {
				oversamplingPrescaler = 1;
				__HAL_TIM_SET_COUNTER(&htim2, 2);
			}
			if (oversamplingPrescaler > 15) {
				oversamplingPrescaler = 15;
				__HAL_TIM_SET_COUNTER(&htim2, 30); //encoder counts up to 19 so 19*2 is max available value rn
			}
			update_oversampling_prescaler(oversamplingPrescaler, rgb565(220, 220, 220));
			printf("Oversampling case executed...\n");
			fflush(stdout);
			break;
		}
		if (encoderBtnFlag) {
			encoderBtnFlag = 0;
			switch (activeScreen) {
			case 0: //sensor to menu
				__HAL_TIM_SET_COUNTER(&htim2, 0);
				activeScreen = 1;
				break;
			case 1:	//menu to ...
				//menu to sensor
				if (currentItem == (menuMaxIndex - 1)) {
					activeScreen = 0;
					break;
				}
				//menu to activechannels
				else if (currentItem == 0) {
					__HAL_TIM_GET_COUNTER(&htim2) = activeChannels*2; //you can't assign value to shifted value, hence no >> is used, silly me tried..
					activeScreen = 2;
					break;
				}
				else if (currentItem == 1) {
					__HAL_TIM_GET_COUNTER(&htim2) = oversamplingPrescaler*2; //you can't assign value to shifted value, hence no >> is used, silly me tried..
					activeScreen = 3;
					break;
				}
			case 2:
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				activeScreen = 1;
				break;
			case 3:
				__HAL_TIM_GET_COUNTER(&htim2) = 0;
				activeScreen = 1;
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
	while (HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin) == GPIO_PIN_RESET) {

	}
	HAL_Delay(50); //basic debounce
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
