/*
 *	Author: Filip Geib
 *	(c) 2021
 */

#ifndef _MCP9804_H_
#define _MCP9804_H_


/* INCLUDES */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "main.h"
#include "stm32l4xx_hal.h"

#include "testing_tools.h"


/* SUPPORT FUNCTIONS */

void mcp9804_powerReset(void);


/* MAIN FUNCTIONS */

void mcp9804_configure(I2C_HandleTypeDef *hand, int dev);

void mcp9884_readManufac(I2C_HandleTypeDef *hand, int dev);

void mcp9884_readData(I2C_HandleTypeDef *hand, int dev);


#endif /* _MCP9804_H_ */
