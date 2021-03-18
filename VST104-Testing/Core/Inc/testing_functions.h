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


/* CONTROL FUNCTIONS */

	int flash_processCommand(QSPI_HandleTypeDef *handler, uint8_t command, uint8_t *buff, int size, int device);

	int fram_processCommand(SPI_HandleTypeDef *handler, uint8_t *command, uint8_t *buff, int size, int device);


/* TESTING FUNCTIONS */

	void flash_manufacData(QSPI_HandleTypeDef *handler, int device);

	void fram_manufacData(SPI_HandleTypeDef *handler, int device);


#endif /* _TESTING_FUNCTIONS_H_ */

