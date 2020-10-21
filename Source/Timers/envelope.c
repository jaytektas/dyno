/*
 * envelope.c
 *
 *  Created on: 20Apr.,2018
 *      Author: jay
 */

#include "Timers/envelope.h"
#include "Tasks/communication.h"
#include "usbd_cdc_if.h"

/* envelopeTimeout function */
void envelopeTimeout(void const * argument)
{
	packetReset();
	osTimerStop(envelopeTimeoutTimerHandle);
}

