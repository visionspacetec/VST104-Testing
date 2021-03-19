/*
 * testing_functions.cpp
 *
 *  Created on: Mar 13, 2021
 *      Author: Filip Geib
 */


#include "testing_functions.h"


/* CONFIGURATION FUNCTIONS */


/* CONTROL FUNCTIONS */

/*
	int fram_processCommand(SPI_HandleTypeDef *handler, uint8_t *command, uint8_t *buff, int size, int device) {
		HAL_StatusTypeDef ret;

		// set all devices
		HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, SPI1_CS2_Pin | SPI1_CS3_Pin, GPIO_PIN_SET);
		HAL_Delay(1);

		// reset appropriate memory
		if(device == 1) HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_RESET);
		else if(device ==2 ) HAL_GPIO_WritePin(GPIOA, SPI1_CS2_Pin, GPIO_PIN_RESET);
		else HAL_GPIO_WritePin(GPIOA, SPI1_CS3_Pin, GPIO_PIN_RESET);

		// send command
		int errNo = 0;
		ret = HAL_SPI_Transmit(handler, command, 1, 100);
		if(ret != HAL_OK) errNo = 1;

		// receive answer
		if(errNo == 0) {
			ret = HAL_SPI_Receive(handler, buff, size, 100);
			if(ret != HAL_OK) errNo = 2;
		}

		//set all devices
		HAL_GPIO_WritePin(GPIOC, SPI1_CS1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, SPI1_CS2_Pin | SPI1_CS3_Pin, GPIO_PIN_SET);

		return errNo;
	}

*/

/* TESTING FUNCTIONS */

/*
	void fram_manufacData(SPI_HandleTypeDef *handler, int device) {
		// prepare variables
		int ret; uint8_t buff[9]; uint8_t command[] = {0x9F};

		// request and read manufacturer device type
		ret = fram_processCommand(handler, command, buff, 9, device);
		if(ret != 0) log_send(1, "fram_manufacData", device, "na", ret);
		else {
			char tmp[19];
			addr2str(buff, 9, tmp);
			log_send(2, "flash_manufacData->type", device, tmp, NAN);
		}

	}

*/
