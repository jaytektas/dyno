/*
 * analog.c
 *
 *  Created on: 16Jan.,2020
 *      Author: jay
 */
#include "analog.h"
#include "stdint.h"
#include "adc.h"
#include "status.h"
#include "config.h"
#include "cmsis_os.h"

__inline__ uint16_t filterADC(uint16_t old, uint16_t new, uint16_t filter)
{
int32_t t;

	t = (new - old) * filter;
	return (uint16_t) old + t / 100;
}

void analogTask(void * argument)
{
uint16_t i;

	for(;;)
	{
		for( i = 0; i < ANALOG_CHANNEL_COUNT; i++)
			status.analog[i] = filterADC(status.analog[i], ADCBuffer[i], config.analog.ADCFilter[i]);
		osThreadYield();
	}
}




