/*
 * testing_tools.h
 *
 *  Created on: Mar 13, 2021
 *      Author: Filip Geib
 */

#ifndef _TESTING_TOOLS_H_
#define _TESTING_TOOLS_H_


/* INCLUDES */

	#include <stdio.h>
	#include <string.h>
	#include <math.h>

	#include "main.h"
	#include "stm32l4xx_hal.h"


/* FUNCTIONS */

void addr2str(uint8_t *addr, int addr_len, char *str);

void scanI2Caddr(I2C_HandleTypeDef *handler);

void log_send(int type, char *identity, int device, char *data_str, float data_flt);


#endif /* _TESTING_TOOLS_H_ */
