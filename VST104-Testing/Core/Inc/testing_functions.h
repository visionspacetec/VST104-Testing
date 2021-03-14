/*
 * testing_functions.h
 *
 *  Created on: Mar 13, 2021
 *      Author: Filip Geib
 */

#ifndef _TESTING_FUNCTIONS_H_
#define _TESTING_FUNCTIONS_H_


/* INCLUDES */

	#include <stdio.h>
	#include <string.h>
	#include <math.h>

	#include "main.h"
	#include "stm32l4xx_hal.h"

	#include "testing_tools.h"


/* DEFINITIONS AND CONSTANTS */

	// MCU ADC calibration
	#define VREFINT_CAL *((uint16_t*) ((uint32_t) 0x1FFF75AA)) //calibrated at 3.3V +-10mV   @ 30C +/- 5C

	// E-Fuse current monitoring parameters
	#define CUR_GAIN 0.000052 //E-Fuse data-sheet
	#define CUR_IOS 0.0000008 //E-Fuse data-sheet
	#define CUR_RMON 75000.0  //schematic


/* CONFIGURATION FUNCTIONS */

	void temp_powerReset(void);

	void temp_configure(I2C_HandleTypeDef *handler, int MCP_id);

	void magneto_powerReset(void);


/* CONTROL FUNCTIONS */

	int flash_processCommand(QSPI_HandleTypeDef *handler, uint8_t command, uint8_t *buff, int size, int device);

	int fram_processCommand(SPI_HandleTypeDef *handler, uint8_t *command, uint8_t *buff, int size, int device);


/* TESTING FUNCTIONS */

	void curr_readData(ADC_HandleTypeDef *handler, int device);

	void temp_readData(I2C_HandleTypeDef *handler, int device);

	void flash_manufacData(QSPI_HandleTypeDef *handler, int device);

	void fram_manufacData(SPI_HandleTypeDef *handler, int device);


#endif /* _TESTING_FUNCTIONS_H_ */

