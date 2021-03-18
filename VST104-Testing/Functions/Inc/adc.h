/*
 *	Author: Filip Geib
 *	(c) 2021
 */

#ifndef _ADC_H_
#define _ADC_H_


/* INCLUDES */

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "stm32l4xx_hal.h"

#include "testing_tools.h"


/* FUNCTIONS */

void curr_readData(ADC_HandleTypeDef *hand, int dev);


#endif /* _ADC_H_ */
