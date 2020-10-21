/*
 * heartBeat.h
 *
 *  Created on: 13Jan.,2020
 *      Author: jay
 */

#ifndef HEARTBEAT_H_
#define HEARTBEAT_H_

#include "cmsis_os.h"

extern void heartBeat(void *arg);
extern osTimerId_t heartBeatTimerHandle;

#endif /* HEARTBEAT_H_ */
