/*
 *	Author: Filip Geib
 *	(c) 2021
 */


#ifndef _S25FL256L_H_
#define _S25FL256L_H_


/* INCLUDES */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "stm32l4xx_hal.h"

#include "testing_tools.h"


/* SUPPORT FUNCTIONS */

void s25fl256l_defaultCommand(QSPI_CommandTypeDef *command);

int s25fl256l_sendCommand(QSPI_HandleTypeDef *hand, QSPI_CommandTypeDef *cmd, int dev);

void s25fl256l_writeSectorRandom(QSPI_HandleTypeDef *hand, int *dev, int devSize, int sector);


/* MAIN FUNCTIONS */

void s25fl256l_configure(QSPI_HandleTypeDef *hand, int dev);

void s25fl256l_readManufac(QSPI_HandleTypeDef *hand, int dev);


#endif /* _S25FL256L_H_ */
