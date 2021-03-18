/*
 * testing_tools.cpp
 *
 *  Created on: Mar 13, 2021
 *      Author: Filip Geib
 */

#include "testing_tools.h"


/* VARIABLES */

UART_HandleTypeDef *huart_A;
UART_HandleTypeDef *huart_B;


/* DEFINITIONS AND CONSTANTS */

// HAL_Status abbreviations
static const char STATUS[5][12] = {"HAL_OK", "HAL_ERROR", "HAL_BUSY", "HAL_TIMEOUT", "I2C_MEASURE"};


/* FUNCTIONS */

void scanI2Caddr(I2C_HandleTypeDef *handler) {
	printf("Scanning I2C bus:\n");
	HAL_StatusTypeDef ret;

	// check all possible addresses
	for(int i=1; i<128; i++) {
	  ret = HAL_I2C_IsDeviceReady(handler, (uint16_t)(i<<1), 2, 10);
	  if (ret == HAL_OK)
		  printf("0x%02X\n", i);
	}
	printf("Scanning completed\n");
}


void addr2str(uint8_t *addr, int addr_len, char *str) {
	int index = 0;
	for(int i=0; i<addr_len; i++)
	   index += sprintf(&str[index], "%02X", addr[i]);
}


/* LOG FUNCTIONS */

void log_assignHuart(UART_HandleTypeDef* hand1, UART_HandleTypeDef* hand2) {
	huart_A = hand1;
	huart_B = hand2;
}


void log_enable() {
	HAL_GPIO_WritePin(GPIOG, UART2_EN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, UART3_EN_Pin, GPIO_PIN_SET);
}


void log_send(char *buff, int lenght) {
	HAL_StatusTypeDef ret;

	ret = HAL_UART_Transmit(huart_A, (uint8_t *)buff, lenght, UART_TIMEOUT);
	if(ret != HAL_OK) printf("ERROR: UART-A %s\n", STATUS[ret]);

	ret = HAL_UART_Transmit(huart_B, (uint8_t *)buff, lenght, UART_TIMEOUT);
	if(ret != HAL_OK) printf("ERROR: UART-B %s\n", STATUS[ret]);
}


void log_info(char *identity, int device) {
	char buff[256];
	int lenght = sprintf(buff, "INF -i %s -d %d\n", identity, device);
	log_send(buff, lenght);
}


void log_error(char *identity, int device, int fcnId, HAL_StatusTypeDef errId) {
	char buff[256];
	int lenght = sprintf(buff, "ERR -i %s -d %d -f %d -h %s\n", identity, device, fcnId, STATUS[errId]);
	log_send(buff, lenght);
}


void log_data(char *identity, int device, char *valType, char *valStr, float valFlt) {
	char buff[256]; int lenght;

	// print string or float value
	if(strcmp(valStr, "NA"))
		lenght = sprintf(buff, "DAT -i %s -d %d -t %s -s %s\n", identity, device, valType, valStr);
	else
		lenght = sprintf(buff, "DAT -i %s -d %d -t %s -f %f\n", identity, device, valType, valFlt);

	// send to UART
	log_send(buff, lenght);
}
