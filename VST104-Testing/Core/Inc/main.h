/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define QSPI_BK1_IO3_Pin GPIO_PIN_6
#define QSPI_BK1_IO3_GPIO_Port GPIOF
#define QSPI_BK1_IO2_Pin GPIO_PIN_7
#define QSPI_BK1_IO2_GPIO_Port GPIOF
#define QSPI_BK1_IO0_Pin GPIO_PIN_8
#define QSPI_BK1_IO0_GPIO_Port GPIOF
#define QSPI_BK1_I1_Pin GPIO_PIN_9
#define QSPI_BK1_I1_GPIO_Port GPIOF
#define QSPI_CLK_Pin GPIO_PIN_10
#define QSPI_CLK_GPIO_Port GPIOF
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define OSC_EN_Pin GPIO_PIN_1
#define OSC_EN_GPIO_Port GPIOH
#define SPI1_HOLD_Pin GPIO_PIN_0
#define SPI1_HOLD_GPIO_Port GPIOC
#define SP1_WP_Pin GPIO_PIN_1
#define SP1_WP_GPIO_Port GPIOC
#define EF2_CUR_Pin GPIO_PIN_2
#define EF2_CUR_GPIO_Port GPIOC
#define EF1_CUR_Pin GPIO_PIN_3
#define EF1_CUR_GPIO_Port GPIOC
#define QSPI_CS1_Pin GPIO_PIN_0
#define QSPI_CS1_GPIO_Port GPIOA
#define QSPI_CS2_Pin GPIO_PIN_1
#define QSPI_CS2_GPIO_Port GPIOA
#define QSPI_CS3_Pin GPIO_PIN_2
#define QSPI_CS3_GPIO_Port GPIOA
#define SPI1_CS3_Pin GPIO_PIN_3
#define SPI1_CS3_GPIO_Port GPIOA
#define SPI1_CS2_Pin GPIO_PIN_4
#define SPI1_CS2_GPIO_Port GPIOA
#define SPI1_CS1_Pin GPIO_PIN_4
#define SPI1_CS1_GPIO_Port GPIOC
#define I2C4_EN_Pin GPIO_PIN_12
#define I2C4_EN_GPIO_Port GPIOB
#define UART3_TX_Pin GPIO_PIN_8
#define UART3_TX_GPIO_Port GPIOD
#define UART3_RX_Pin GPIO_PIN_9
#define UART3_RX_GPIO_Port GPIOD
#define UART3_EN_Pin GPIO_PIN_10
#define UART3_EN_GPIO_Port GPIOD
#define I2C3_EN_Pin GPIO_PIN_14
#define I2C3_EN_GPIO_Port GPIOD
#define UART2_EN_Pin GPIO_PIN_2
#define UART2_EN_GPIO_Port GPIOG
#define TEST2_Pin GPIO_PIN_9
#define TEST2_GPIO_Port GPIOA
#define TEST1_Pin GPIO_PIN_10
#define TEST1_GPIO_Port GPIOA
#define SW_DIO_Pin GPIO_PIN_13
#define SW_DIO_GPIO_Port GPIOA
#define SW_CLK_Pin GPIO_PIN_14
#define SW_CLK_GPIO_Port GPIOA
#define UART2_TX_Pin GPIO_PIN_5
#define UART2_TX_GPIO_Port GPIOD
#define UART2_RX_Pin GPIO_PIN_6
#define UART2_RX_GPIO_Port GPIOD
#define I2C2_EN_Pin GPIO_PIN_7
#define I2C2_EN_GPIO_Port GPIOD
#define SW_0_Pin GPIO_PIN_3
#define SW_0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define UART_TIMEOUT	50
#define I2C_TIMEOUT		100
#define QSPI_TIMEOUT	100

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
