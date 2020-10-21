#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdint.h"
#include "hub.h"

#define MS_SECOND				1000

#define EDGE_FALLING			0
#define EDGE_RISING				1
#define EDGE_BOTH				2

typedef struct {
	unsigned char identity[60];
	unsigned char version[10];
	uint8_t TrigEdge[HUBS];
	uint8_t rpmEdge;
	struct
	{
		uint16_t ADCFilter[12];
	} analog;
} config_t;

extern const config_t DefaultConfig;
extern config_t config;

#endif /* CONFIG_H_ */
