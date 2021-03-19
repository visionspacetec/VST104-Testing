/*
 *	Author: Filip Geib
 *	(c) 2021
 */

#ifndef _TESTING_TOOLS_H_
#define _TESTING_TOOLS_H_


/* INCLUDES */

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "stm32l4xx_hal.h"


/* DEFINITIONS */

// artificial abbreviations index
#define	HAL_I2C		4
#define HAL_QSPI	5


/* FUNCTIONS */

void scanI2Caddr(I2C_HandleTypeDef *handler);

void addr2str(uint8_t *addr, int addr_len, char *str);


/* LOG FUNCTIONS */

void log_assignHuart(UART_HandleTypeDef *hand1, UART_HandleTypeDef *hand2);

void log_enable(void);

void log_send(char *buff, int lenght);

void log_info(char *identity, int device);

void log_error(char *identity, int device, int fcnId, HAL_StatusTypeDef errId);

void log_data(char *identity, int device, char *valType, char *valStr, float valFlt);


#endif /* _TESTING_TOOLS_H_ */
