#ifndef HUB_H
#define HUB_H

#include "stdint.h"
#include "cmsis_os.h"
#include "timers.h"

#define HUBS 4
#define HUB_WATCH_DOG_INTERVAL_MS	10			// how often to check if the watch dog timer has expired
#define HUB_WATCH_DOG_TIMEOUT_MS	1000		// 0 - 65535ms timeout for hub trigger intervals

#define PERIOD(now, then) (now < then) ? (now + (0xffffffff - then) + 1) : (now - then)

typedef struct
{
	uint8_t watchDogEnabled:1;
	uint16_t watchDogCount;								// counter to timeout when no activity
	uint32_t lastTimeStamp;
	uint32_t thisTimeStamp;
	uint8_t valid:1;									// is the status interval valid
} hub_t;

extern void hubTimeout(uint8_t rollerIndex);		// when a roller has stalled
extern void hubWatchDog(void *argument);
extern void TIM2_IRQHandler(void);
extern hub_t hub[HUBS];

#endif
