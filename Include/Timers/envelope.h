/*
 * envelope.h
 *
 *  Created on: 20Apr.,2018
 *      Author: jay
 */

#ifndef TIMERS_ENVELOPE_H_
#define TIMERS_ENVELOPE_H_

#include "cmsis_os.h"

extern osTimerId envelopeTimeoutTimerHandle;
void envelopeTimeout(void const * argument);

#endif /* TIMERS_ECT_H_ */
