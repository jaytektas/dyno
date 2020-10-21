#include "hub.h"
#include "status.h"
#include "config.h"
#include "stm32f4xx_hal.h"

hub_t hub[HUBS];
uint16_t msw;

// called from freeRTOS as a timer every HUB_WATCH_DOG_INTERVAL_MS
void hubWatchDog(void *argument)
{
uint8_t i;

	for (i = 0; i < HUBS; i++)
	{
		if (hub[i].watchDogEnabled)
		{
			hub[i].watchDogCount += HUB_WATCH_DOG_INTERVAL_MS;
			if (hub[i].watchDogCount >= HUB_WATCH_DOG_TIMEOUT_MS)
				hubTimeout(i);
		}
	}
}

// called when a hub times out between trigger intervals so we consider it as stalled
void hubTimeout(uint8_t hubIndex)
{
	hub[hubIndex].valid = 0;
	hub[hubIndex].lastTimeStamp = 0;
	hub[hubIndex].thisTimeStamp = 0;
	status.hub[hubIndex].interval = 0;
	hub[hubIndex].watchDogEnabled = 0;

	// turn watch dog off for this roller
}

// when a trigger pulse comes in this routine is called to update the hub details
void hubUpdate(uint8_t hubIndex, uint32_t now)
{
	hub[hubIndex].thisTimeStamp = now;
	if (hub[hubIndex].valid)
		status.hub[hubIndex].interval = PERIOD(hub[hubIndex].thisTimeStamp, hub[hubIndex].lastTimeStamp);

	hub[hubIndex].lastTimeStamp = hub[hubIndex].thisTimeStamp;
	hub[hubIndex].valid = 1;

	// reset watch dog
	hub[hubIndex].watchDogCount = 0;
	hub[hubIndex].watchDogEnabled = 1;
}

void TIM1_UP_TIM10_IRQHandler(void)
{
	TIM1->SR &= ~TIM_SR_UIF;
	msw++;
}

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
void TIM1_CC_IRQHandler(void)
{
uint16_t ccr;

	// capture overflow?
	if ((TIM1->SR & TIM_SR_CC1OF) != 0)
		TIM1->SR = 0;

	if ((TIM1->SR & TIM_SR_CC1IF) != 0)
	{
		ccr = TIM1->CCR1;
		if (TIM1->SR & TIM_SR_UIF)
			if (ccr >= 0)
			{
				msw++;
				TIM1->SR &= ~TIM_SR_UIF;
			}
		hubUpdate(0, (msw << 16) + ccr);
	}

	if ((TIM1->SR & TIM_SR_CC2IF) != 0)
	{
		ccr = TIM1->CCR2;
		if (TIM1->SR & TIM_SR_UIF)
			if (ccr < 0x8000)
			{
				msw++;
				TIM1->SR &= ~TIM_SR_UIF;
			}
		hubUpdate(1, ((msw << 16) + ccr));
	}

	if ((TIM1->SR & TIM_SR_CC3IF) != 0)
	{
		ccr = TIM1->CCR3;
		if (TIM1->SR & TIM_SR_UIF)
			if (ccr < 0x8000)
			{
				msw++;
				TIM1->SR &= ~TIM_SR_UIF;
			}
		hubUpdate(2, ((msw << 16) + ccr));
	}

	if ((TIM1->SR & TIM_SR_CC4IF) != 0)
	{
		ccr = TIM1->CCR4;
		if (TIM1->SR & TIM_SR_UIF)
			if (ccr < 0x8000)
			{
				msw++;
				TIM1->SR &= ~TIM_SR_UIF;
			}
		hubUpdate(3, ((msw << 16) + ccr));
	}
}


