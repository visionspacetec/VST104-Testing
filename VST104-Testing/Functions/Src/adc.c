/*
 *	Author: Filip Geib
 *	(c) 2021
 */


#include "adc.h"


/* DEFINITIONS AND CONSTANTS */

// MCU ADC calibration: @ 3.3V +-10mV, @ 30C +/- 5C
#define VREFINT_CAL *((uint16_t*) ((uint32_t) 0x1FFF75AA))

// E-Fuse current monitoring parameters
#define CUR_GAIN	0.000052
#define CUR_IOS		0.0000008
#define CUR_RMON	75000.0


/* FUNCTIONS */

void curr_readData(ADC_HandleTypeDef *hand, int dev) {
	//start ADC conversion
	HAL_ADC_Start(hand);

	//poll ADC peripheral
	HAL_ADC_PollForConversion(hand, 1);

	//read ADC conversion result
	uint16_t raw_val = HAL_ADC_GetValue(hand);

	//stop ADC conversion
	HAL_ADC_Stop(hand);

	//compute current consumption from ADC vals in [mA]
	float val = (3000 * (float)VREFINT_CAL / (float)raw_val) / 1000;
	val = (val - CUR_IOS * CUR_RMON) / (CUR_GAIN * CUR_RMON);

	log_data("adc", dev, "curr", "NA", val);
}
