#ifndef STATUS_H
#define STATUS_H

#include "stdint.h"
#include "hub.h"

// this is the structure to to be transmitted to the external application
typedef struct
{
	int32_t temperature;
	uint32_t pressure;
	uint32_t humidity;
} weather_t;

typedef struct
{
	uint16_t position;					// current position of stepper motor
	uint8_t state;						// what is the stepper motor doing at the moment
} statusStepper_t;

typedef struct hub
{
	uint32_t interval;					// current tooth interval
	uint32_t loadCell;					// 24 bit ADC HX711 load cell value
	statusStepper_t stepper;
} statusHub_t;

typedef struct
{
	weather_t weather;					// weather observations for correction
	statusHub_t hub[HUBS];				// up to this many hubs
	uint16_t analog[12];				// analog channel values after filter has been applied
} status_t;

extern status_t status;

#endif


