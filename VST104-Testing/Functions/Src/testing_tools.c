/*
 * testing_tools.cpp
 *
 *  Created on: Mar 13, 2021
 *      Author: Filip Geib
 */

#include "testing_tools.h"


/* VARIABLES */

UART_HandleTypeDef* huartPtrs[2];

/* DEFINITIONS AND CONSTANTS */

// LOG classes abbreviations
static const char LOG_types[3][4] = {"INF", "ERR", "DAT"};


/* FUNCTIONS */

void addr2str(uint8_t *addr, int addr_len, char *str) {
	int index = 0;
	for(int i=0; i<addr_len; i++)
	   index += sprintf(&str[index], "%02X", addr[i]);
}


void scanI2Caddr(I2C_HandleTypeDef *handler) {
	printf("Scanning I2C bus:\n");
	HAL_StatusTypeDef result;

	// check all possible addresses
	for(int i=1; i<128; i++) {
	  result = HAL_I2C_IsDeviceReady(handler, (uint16_t)(i<<1), 2, 10);
	  if (result == HAL_OK)
		  printf("0x%02X\n", i);
	}
	printf("Scanning completed\n");
}


void assign_huartPtr(UART_HandleTypeDef* hand1, UART_HandleTypeDef* hand2) {
	huartPtrs[0] = hand1;
	huartPtrs[1] = hand2;
}


void log_send(int type, char *identity, int device, char *data_str, float data_flt) {
	// process device
	char device_str[8] = "na";
	if(device >= 0) sprintf(device_str, "%d", device);

	// generate message string
	char msg[256];
	sprintf(msg, "%s -i %s -d %s -s %s -f %f\n", LOG_types[type], identity, device_str, data_str, data_flt);

	// send to SWD
	printf("%s", msg);

	// send to UARTs
	for(int i=0; i<2; i++) {
		if(HAL_UART_Transmit(huartPtrs[i], (uint8_t *)msg, strlen(msg)+1, UART_TIMEOUT) != HAL_OK)
			printf("#### UART-%d ERROR ####\n", i+1);
	}
}

