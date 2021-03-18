/*
 *	Author: Filip Geib
 *	(c) 2021
 */

#ifndef _MPU6050_H_
#define _MPU6050_H_


/* INCLUDES */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "main.h"
#include "stm32l4xx_hal.h"

#include "testing_tools.h"


/* SUPPORT FUNCTIONS */

void mpu6050_powerReset(void);

int mpu6050_waitMeasure(I2C_HandleTypeDef *hand, int dev);

int16_t mpu6050_processBuff(uint8_t *buff, int data);


/* MAIN FUNCTIONS */

void mpu6050_configure(I2C_HandleTypeDef *hand, int dev);

void mpu6050_readManufac(I2C_HandleTypeDef *hand, int dev);

void mpu6050_readAccData(I2C_HandleTypeDef *hand, int dev);

void mpu6050_readGyroData(I2C_HandleTypeDef *hand, int dev);

void mpu6050_readTempData(I2C_HandleTypeDef *hand, int dev);


#endif /* _MPU6050_H_ */
