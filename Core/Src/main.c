/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h> //printf

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define VREFINT_CAL *((uint16_t*) ((uint32_t) 0x1FFF75AA)) //calibrated at 3.3V +-10mV   @ 30C +/- 5C

#define CUR_GAIN 0.000052 //E-Fuse datasheet
#define CUR_IOS 0.0000008 //E-Fuse datasheet
#define CUR_RMON 75000.0 //schematics

#define MCP_REG_TEMP (0x05) //temperature register
#define MCP_REG_CONF (0x01) //configuration register

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c2;

QSPI_HandleTypeDef hqspi;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_I2C2_Init(void);
static void MX_QUADSPI_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//SWD printf workaround
int _write(int file, char *ptr, int len) {
  /* Implement your write code here, this is used by puts and printf for example */
  int i=0;
  for(i=0 ; i<len ; i++)
    ITM_SendChar((*ptr++));
  return len;
}

int QSPI_read_data(QSPI_HandleTypeDef *hqspi, int device, uint8_t command, uint8_t *buff, int size){
	HAL_StatusTypeDef ret;
	QSPI_CommandTypeDef sCommand;

	sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	sCommand.Instruction       = command;
	sCommand.AddressMode       = QSPI_ADDRESS_NONE;
	sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;	//OK
	sCommand.DataMode          = QSPI_DATA_1_LINE;			//OK
	sCommand.NbData            = size;
	sCommand.DummyCycles       = 0;							//when 8 then 1 bit left shift
	sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;		//OK
	sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;	//OK

	//reset all devices
	HAL_GPIO_WritePin(GPIOA, QSPI_CS1_Pin | QSPI_CS2_Pin , GPIO_PIN_SET);
	HAL_GPIO_WritePin(QSPI_CS3_GPIO_Port, QSPI_CS3_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	//select appropriate memory
	if(device == 1) HAL_GPIO_WritePin(GPIOA, QSPI_CS1_Pin, GPIO_PIN_RESET);
	else if(device ==2 ) HAL_GPIO_WritePin(GPIOA, QSPI_CS2_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(QSPI_CS3_GPIO_Port, QSPI_CS3_Pin, GPIO_PIN_RESET);

	//send command
	ret = HAL_QSPI_Command(hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
	if(ret != HAL_OK) return 1;

	//receive answer
	ret = HAL_QSPI_Receive(hqspi, buff, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
	if(ret != HAL_OK) return 2;

	return 0;
}

int SPI_read_data(SPI_HandleTypeDef *hspi, uint8_t *command, uint8_t *buff, int size){
	HAL_StatusTypeDef ret;

	//toggle NCS pin
	HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_RESET);

	ret = HAL_SPI_Transmit(hspi, command, 1, 100);
	if(ret != HAL_OK) return 1;

	ret = HAL_SPI_Receive(hspi, buff, size, 100);
	if(ret != HAL_OK) return 2;

	//power off device
	HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_SET);

	return 0;
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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_I2C2_Init();
  MX_QUADSPI_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */


	//test SDW printf
  	HAL_Delay(1500);
	printf("\n\nSDW printf() initialized\n\n");

	//open USER1 isolator and turn on green LED
	printf("Opening LED isolator & turning on green\n\n");
	HAL_GPIO_WritePin(USER1_EN_GPIO_Port, USER1_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);

	//initialize I2C MCP9804 temperature sensors
	printf("Initializing MCP9804 temp. sensors:\n");

	//power on temperature sensors
	HAL_GPIO_WritePin(I2C2_EN_GPIO_Port, I2C2_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	//list of temp. sensors I2C addresses
	static const uint8_t MCP_add[] = {0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1E, 0x1F};

	//prepare I2C variables
	HAL_StatusTypeDef MCP_ret;
	uint8_t MCP_buff[2];

	//configure sensor: thermostat mode, interrupt not used
	MCP_buff[0] = 0x00; MCP_buff[1] = 0x08;
	for(int i=0; i<sizeof MCP_add; i++) {
		MCP_ret = HAL_I2C_Mem_Write(&hi2c2, MCP_add[i] << 1, MCP_REG_CONF, 1, MCP_buff, 2, HAL_MAX_DELAY);
		if(MCP_ret == HAL_OK) printf("\t- device 0x%02X configured\n", MCP_add[i]);
		else printf("\t- device %02X failed\n", MCP_add[i]);
	}

	HAL_Delay(500);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	//greetings and blink
	printf("\nBoard Sierra test iteration:\n");

	//LED GPIO demonstration ############################################################################################################
	printf("\n(1) Blinking red LED\n");

	//blink with red LED
	for(int i=0; i<4; i++) {
		HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
		HAL_Delay(200);
	}

	//clock source checking ##############################################################################################################
	printf("\n(2) Clock freq. and source demonstration:\n");

	//read and print SYSCLK, HCLK, PCLK1 and PCLK2 frequencies
	printf("\t- SYSCLK freq.: %lu [Hz]\n", HAL_RCC_GetSysClockFreq());
	printf("\t- HCLK freq.:   %lu [Hz]\n", HAL_RCC_GetHCLKFreq());
	printf("\t- PCLK1 freq.:  %lu [Hz]\n", HAL_RCC_GetPCLK1Freq());
	printf("\t- PCLK2 freq.:  %lu [Hz]\n", HAL_RCC_GetPCLK2Freq());

	//current reading testing ############################################################################################################
	printf("\n(3) Power management current readings:\n");

	//prepare ADC variables
	uint16_t ADC_vals[2];

	//start ADC conversion for EF1
	HAL_ADC_Start(&hadc1);
	//poll ADC peripheral
	HAL_ADC_PollForConversion(&hadc1, 1);
	//read ADC conversion result
	ADC_vals[0] = HAL_ADC_GetValue(&hadc1);
	//stop ADC conversion for EF1
	HAL_ADC_Stop(&hadc1);

	//start ADC conversion for EF2
	HAL_ADC_Start(&hadc2);
	//poll ADC peripheral
	HAL_ADC_PollForConversion(&hadc2, 1);
	//read ADC conversion result
	ADC_vals[1] = HAL_ADC_GetValue(&hadc2);
	//stop ADC conversion for EF2
	HAL_ADC_Stop(&hadc2);

	//compute current consumption from ADC values in [mA]
	float CUR_vals[2];
	for(int i=0; i<2; i++) {
		CUR_vals[i] = (3000 * (float)VREFINT_CAL / (float)ADC_vals[i]) / 1000;
		CUR_vals[i] = (CUR_vals[i] - CUR_IOS * CUR_RMON) / (CUR_GAIN * CUR_RMON);
	}

	// print results
	printf("\t- 3V3 channel: %f [mA]\n", CUR_vals[0]);
	printf("\t- 5V channel:  %f [mA]\n", CUR_vals[1]);

	//temperature reading testing #########################################################################################################
	printf("\n(4) Temperature sensors readings:\n");

	//loop and read & process data from all sensors
	for(int i=0; i<sizeof MCP_add; i++) {
		//request Ta data
		MCP_buff[0] = 0x00; MCP_buff[1] = 0x00;
		MCP_ret = HAL_I2C_Mem_Write(&hi2c2, MCP_add[i] << 1, MCP_REG_TEMP, 1, MCP_buff, 2, HAL_MAX_DELAY);
		if(MCP_ret != HAL_OK) printf("\t- device 0x%02X failed at writing\n", MCP_add[i]);

		//read Ta data
		MCP_ret = HAL_I2C_Mem_Read(&hi2c2, MCP_add[i] << 1, MCP_REG_TEMP, 1, MCP_buff, 2, HAL_MAX_DELAY);
		if(MCP_ret != HAL_OK) printf("\t- device 0x%02X failed at reading\n", MCP_add[i]);

		//convert Ta value to Celsius temperature
		float MCP_temp;
		//clear flag bite
		if(MCP_buff[0] & 0xE0)
			MCP_buff[0] = MCP_buff[0] & 0x1F;

		if((MCP_buff[0] & 0x10) == 0x10){
			MCP_buff[0] = MCP_buff[0] & 0x0F;
			MCP_temp = (256 - (MCP_buff[0] << 4) + (MCP_buff[1] >> 4)) * -1;
		}
		else
			MCP_temp = (float)MCP_buff[0] * 16 + (float)MCP_buff[1] / 16;

		//print results
		printf("\t- device 0x%02X temperature: %.3f [%cC]\n", MCP_add[i], MCP_temp, 176);
	}

	//QSPI flash memories detection ##########################################################################################################
	printf("\n(5) NOR-Flash QSPI manufacturer data request:\n");

	//prepare variables
	int QSPI_ret; uint8_t QSPI_buff[8];

	//loop thru memories
	for(int i=1; i<4; i++) {
		//request and read manufacturer device type
		QSPI_ret = QSPI_read_data(&hqspi, i, 0x9F, QSPI_buff, 3);
		if(QSPI_ret != 0) printf("\t- memory %d manufacturer type failed at %d\n", i, QSPI_ret);
		else {
			printf("\t- memory %d dev. type: ", i);
			for(int j=0; j<3; j++) printf("0x%02X ", QSPI_buff[j]);
			printf("\n");
		}
	}

	//loop thru memories
	for(int i=1; i<4; i++) {
		//request and read unique device ID
		QSPI_ret = QSPI_read_data(&hqspi, i, 0x4B, QSPI_buff, 8);
		if(QSPI_ret != 0) printf("\t- memory %d unique ID failed at %d\n", i, QSPI_ret);
		else {
			printf("\t- memory %d unique ID: ", i);
			for(int j=0; j<8; j++) printf("0x%02X ", QSPI_buff[j]);
			printf("\n");
		}
	}

	//QSPI flash memories detection ##########################################################################################################
	printf("\n(6) F-RAM SPI manufacturer data request:\n");

	//prepare variables
	int SPI_ret; uint8_t SPI_buff[9];

	//request and read device ID
	uint8_t SPI_command[] = {0x9F};
	SPI_ret = SPI_read_data(&hspi1, SPI_command, SPI_buff, 9);
	if(SPI_ret != 0) printf("\t- memory ID failed at %d\n", SPI_ret);
	else {
		printf("\t- memory dev. type and unique ID: ");
		for(int j=0; j<9; j++) printf("0x%02X ", SPI_buff[j]);
		printf("\n");
	}

	//new line needed
	printf("\n");

	//wait
	HAL_Delay(2000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

  while(1){

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
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_ADC;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 2;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();
  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_LSE;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,32768);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.NbrOfDiscConversion = 1;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.NbrOfDiscConversion = 1;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00C0FEFF;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief QUADSPI Initialization Function
  * @param None
  * @retval None
  */
static void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 0;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 7;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OSC_EN_GPIO_Port, OSC_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SPI1_HD_Pin|SPI1_WP_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, QSPI_CS1_Pin|QSPI_CS2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, QSPI_CS3_Pin|SPI1_CS3_Pin|SPI1_CS2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin|USER1_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2C2_EN_GPIO_Port, I2C2_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : OSC_EN_Pin */
  GPIO_InitStruct.Pin = OSC_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OSC_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_HD_Pin SPI1_WP_Pin */
  GPIO_InitStruct.Pin = SPI1_HD_Pin|SPI1_WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : QSPI_CS1_Pin QSPI_CS2_Pin */
  GPIO_InitStruct.Pin = QSPI_CS1_Pin|QSPI_CS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : QSPI_CS3_Pin SPI1_CS3_Pin SPI1_CS2_Pin */
  GPIO_InitStruct.Pin = QSPI_CS3_Pin|SPI1_CS3_Pin|SPI1_CS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_CS1_Pin USER1_EN_Pin */
  GPIO_InitStruct.Pin = SPI1_CS1_Pin|USER1_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : I2C2_EN_Pin */
  GPIO_InitStruct.Pin = I2C2_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(I2C2_EN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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