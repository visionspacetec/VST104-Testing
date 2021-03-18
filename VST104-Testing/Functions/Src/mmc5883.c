/*
 *	Author: Filip Geib
 *	(c) 2021
 */


#include "mmc5883.h"


/* DEFINITIONS AND CONSTANTS */

// MMC5883 list of I2C addresses
static const uint8_t ADDR[] = {0x30, 0x30};

// measurement maximal timeout (in ms)
#define TIMEOUT		30	// 100Hz per channel

// MMC5883 significant registers
#define REG_STAT	0x07
#define REG_CTRL0	0x08
#define REG_CTRL1	0x09
#define REG_CTRL2	0x0A

#define REG_MAG		0x00
#define REG_TEMP	0x06
#define	REG_ID		0x2F

// MMC5883 significant commands
static uint8_t CMD_TM_M	= 0x01;
static uint8_t CMD_TM_T	= 0x02;

static uint8_t CMD_SET 	= 0x08;
static uint8_t CMD_RST 	= 0x10;

static uint8_t CMD_SW_RST	= 0x80;

// MMC5883 significant constants
#define RDY_TM_M	0
#define RDY_TM_T	1

#define MAG_So		0.25	// mG/LSB
#define	MAG_FS		8.0		// G
#define T_So		0.7		// LSB/degC
#define T_Off		-75.0	// degC


/* SUPPORT FUNCTIONS */

void mmc5883_powerReset() {
	// isolate both I2C busses
	HAL_GPIO_WritePin(I2C3_EN_GPIO_Port, I2C3_EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(I2C4_EN_GPIO_Port, I2C4_EN_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);

	// connect both I2C busses
	HAL_GPIO_WritePin(I2C3_EN_GPIO_Port, I2C3_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(I2C4_EN_GPIO_Port, I2C4_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	log_info("mmc5883_powerReset", -1);
}


int mmc5883_waitMeasure(I2C_HandleTypeDef *hand, int type, int dev) {
	HAL_StatusTypeDef ret; uint8_t buff;

	// variable for time management
	int cnt = 0;

	// periodically check status register
	while(cnt*5 < TIMEOUT) {
		// read status register
		ret = HAL_I2C_Mem_Read(hand, ADDR[dev] << 1, REG_STAT, 1, &buff, 1, I2C_TIMEOUT);
		if(ret != HAL_OK) log_error("mmc5883_waitMeasure", dev, 0, ret);

		// check buffer (0:mag, 1:temp)
		if(!type && (buff & (1 << RDY_TM_M))) return 0;
		else if(type && (buff & (1 << RDY_TM_T))) return 0;

		// wait 5ms and increase counter
		HAL_Delay(5);
		cnt++;
	}

	// timeout error
	return -1;
}


uint16_t mmc5883_processBuff(uint8_t *buff, int data) {
	return (uint16_t)buff[0 + 2*data] | (uint16_t)(buff[1 + 2*data] << 8);
}


/* MAIN FUNCTIONS */

void mmc5883_configure(I2C_HandleTypeDef *hand, int dev) {
	HAL_StatusTypeDef ret;

	// reset device - clear registers (5ms power on)
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL1, 1, &CMD_SW_RST, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_configure", dev, 0, ret);
	HAL_Delay(100);

	log_info("mmc5883_configure", dev);
}


void mmc5883_readManufac(I2C_HandleTypeDef *hand, int dev) {
	HAL_StatusTypeDef ret; uint8_t buff;

	// read product ID register
	ret = HAL_I2C_Mem_Read(hand, ADDR[dev] << 1, REG_ID, 1, &buff, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readManufac", dev, 0, ret);

	// convert address to string
	char tmp[3];
	addr2str(&buff, 1, tmp);
	log_data("mmc5883", dev, "dev_id", tmp, NAN);
}


void mmc5883_readMagData(I2C_HandleTypeDef *hand, int dev) {
	HAL_StatusTypeDef ret; uint8_t buff[6]; int waitRet;

	// set magnetometer
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL0, 1, &CMD_SET, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 0, ret);

	// initiate magnetic measurement
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL0, 1, &CMD_TM_M, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 1, ret);

	// wait for measurement to complete
	waitRet = mmc5883_waitMeasure(hand, 0, dev);
	if(waitRet != 0) log_error("mmc5883_readMagData", dev, 2, 4);

	// read magnetic register
	ret = HAL_I2C_Mem_Read(hand, ADDR[dev] << 1, REG_MAG, 1, buff, 6, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 3, ret);

	// process measurement 1
	uint16_t valTmp[6];
	for(int i=0; i<3; i++)
		valTmp[i] = mmc5883_processBuff(buff, i);

	// reset magnetometer
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL0, 1, &CMD_RST, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 4, ret);

	// initiate magnetic measurement
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL0, 1, &CMD_TM_M, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 5, ret);

	// wait for measurement to complete
	waitRet = mmc5883_waitMeasure(hand, 0, dev);
	if(waitRet != 0) log_error("mmc5883_readMagData", dev, 6, 4);

	// read magnetic register
	ret = HAL_I2C_Mem_Read(hand, ADDR[dev] << 1, REG_MAG, 1, buff, 6, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readMagData", dev, 7, ret);

	// process measurement 2
	for(int i=0; i<3; i++)
		valTmp[i+3] = mmc5883_processBuff(buff, i);

	// find data offset
	float valOffset[3];
	for(int i=0; i<3; i++)
		valOffset[i] = (float)(valTmp[i] + valTmp[i+3]) / 2;

	// process final data
	float val[3];
	for(int i=0; i<3; i++) {
		val[i] = (float)valTmp[i] - valOffset[i];
		val[i] = val[i] * (MAG_So / 1000) * (MAG_FS / 8);
	}

	log_data("mmc5883", dev, "mag_x", "NA", val[0]);
	log_data("mmc5883", dev, "mag_y", "NA", val[1]);
	log_data("mmc5883", dev, "mag_z", "NA", val[2]);
}


void mmc5883_readTempData(I2C_HandleTypeDef *hand, int dev) {
	HAL_StatusTypeDef ret; uint8_t buff; int waitRet;

	// initiate temperature measurement
	ret = HAL_I2C_Mem_Write(hand, ADDR[dev] << 1, REG_CTRL0, 1, &CMD_TM_T, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readTempData", dev, 0, ret);

	// wait for measurement to complete
	waitRet = mmc5883_waitMeasure(hand, 1, dev);
	if(waitRet != 0) log_error("mmc5883_readTempData", dev, 1, 4);

	// read temperature register
	ret = HAL_I2C_Mem_Read(hand, ADDR[dev] << 1, REG_TEMP, 1, &buff, 1, I2C_TIMEOUT);
	if(ret != HAL_OK) log_error("mmc5883_readTempData", dev, 2, ret);

	// process temperature value (formula from data-sheet)
	float val = T_Off + T_So*(float)buff;

	log_data("mmc5883", dev, "temp", "NA", val);
}

