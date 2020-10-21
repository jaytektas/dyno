#include "stm32f4xx_hal.h"
#include "hub.h"
#include "bmp280.h"
#include "i2c.h"
#include "hx711.h"
#include "status.h"

void heartBeat(void *arg)
{
uint8_t i, hubTurningFlag = 0;

	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	// see if any rollers are moving and turn on blue LED
	for (i=0; i < HUBS; i++)
		if (hub[i].valid)
			hubTurningFlag = 1;

	if (hubTurningFlag)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

	//	bool bme280p = bmp280.id == BME280_CHIP_ID;
	// update weather station
	if(!bmp280_read_fixed(&bmp280, &status.weather.temperature, &status.weather.pressure, &status.weather.humidity))
	{
		__asm__("bkpt");
	}
}
