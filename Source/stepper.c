/*
 * stepper[hubIndex].cpp
 *
 *  Created on: 24Jan.,2020
 *      Author: jay
 */

#include <math.h>

#include "status.h"
#include "stepper.h"
#include "global.h"
#include "hub.h"
#include "stm32f4xx_hal.h"

// move relative
// setup each stepper[hubIndex]
// move absolute

stepper_t stepper[HUBS];

void stepperHome(uint8_t hubIndex)
{
	status.hub[hubIndex].stepper.position = 0;
}

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
void TIM5_IRQHandler(void)
{
	// delay call back
	if (TIM5->SR & TIM_SR_CC1IF)
	{
		if (status.hub[0].stepper.state == STOP)
		{
			TIM5->CCER &= ~TIM_CCER_CC1E;
			TIM5->DIER &= ~TIM_DIER_CC1IE;
			TIM5->SR &= ~TIM_SR_CC1IF;
		}

		stepperDelay(0);	// compute new delay between steps
		TIM5->CCR1 += stepper[0].step_delay;
	}
}

void stepperAbs(uint8_t hubIndex, int16_t newPosition)
{}

// values passed here for accel / decel / speed are in radians
void stepperRel(uint8_t hubIndex, int16_t step, uint32_t accel, uint32_t decel, uint32_t speed)
{
	if(step < 0)
	{
		stepper[hubIndex].dir = CCW;
		step = -step;
	}
	else
	{
		stepper[hubIndex].dir = CW;
	}

	// If moving only 1 step.
	if(step == 1)
	{
		// Move one step...
		stepper[hubIndex].accel_count = -1;
		// ...in DECEL state.
		status.hub[hubIndex].stepper.state = DECEL;
		// Just a short delay so main() can act on 'running'.
		stepper[hubIndex].step_delay = 1000;
//		status.running = TRUE;
		//    OCR1A = 10;
		// Run Timer/Counter 1 with prescaler = 8.
		//    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
	}
	// Only move if number of steps to move is not zero.
	else
	{
		if(step != 0)
		{
			stepper[hubIndex].min_delay = A_T_x100 / speed;
			stepper[hubIndex].step_delay = (T5_FREQ_148 * sqrt(A_SQ / accel)) / 100;
			stepper[hubIndex].max_s_lim = (long)speed * speed / (long)(((long)A_x20000 * accel) / 100);

			if (stepper[hubIndex].max_s_lim == 0)
				stepper[hubIndex].max_s_lim = 1;

			stepper[hubIndex].accel_lim = ((long)step * decel) / (accel + decel);

			if (stepper[hubIndex].accel_lim == 0)
				stepper[hubIndex].accel_lim = 1;

			if (stepper[hubIndex].accel_lim <= stepper[hubIndex].max_s_lim)
				stepper[hubIndex].decel_val = stepper[hubIndex].accel_lim - step;
			else
				stepper[hubIndex].decel_val = -((long)stepper[hubIndex].max_s_lim * accel) / decel;

			if(stepper[hubIndex].decel_val == 0)
				stepper[hubIndex].decel_val = -1;

			stepper[hubIndex].decel_start = step + stepper[hubIndex].decel_val;

			if(stepper[hubIndex].step_delay <= stepper[hubIndex].min_delay)
			{
				stepper[hubIndex].step_delay = stepper[hubIndex].min_delay;
				status.hub[hubIndex].stepper.state = RUN;
			}
			else
				status.hub[hubIndex].stepper.state = ACCEL;

			stepper[hubIndex].accel_count = 0;

			if (hubIndex == 0)
			{
				TIM5->CCR1 += stepper[hubIndex].step_delay;
				TIM5->CCER |= TIM_CCER_CC1E;
				TIM5->DIER |= TIM_DIER_CC1IE;
			}


			//			status.running = TRUE;
			//    OCR1A = 10;
				// Set Timer/Counter to divide clock by 8
			//    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
		}
	}
}

void stepperDelay(uint8_t hubIndex)
{
	switch(status.hub[hubIndex].stepper.state)
	{
		case STOP:
			stepper[hubIndex].step_count = 0;
			stepper[hubIndex].rest = 0;
			// Stop Timer/Counter 1.
			//      TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
//			status.running = FALSE;
			break;

		case ACCEL:
//			sm_driver_StepCounter(srd.dir);
			stepper[hubIndex].step_count++;
			stepper[hubIndex].accel_count++;
			stepper[hubIndex].new_step_delay = stepper[hubIndex].step_delay - (((2 * (long)stepper[hubIndex].step_delay) + stepper[hubIndex].rest) / (4 * stepper[hubIndex].accel_count + 1));
			stepper[hubIndex].rest = ((2 * (long)stepper[hubIndex].step_delay) + stepper[hubIndex].rest) % (4 * stepper[hubIndex].accel_count + 1);
			// Check if we should start deceleration.
			if(stepper[hubIndex].step_count >= stepper[hubIndex].decel_start)
			{
				stepper[hubIndex].accel_count = stepper[hubIndex].decel_val;
				status.hub[hubIndex].stepper.state = DECEL;
			}
			// Check if we hit max speed.
			else
				if(stepper[hubIndex].new_step_delay <= stepper[hubIndex].min_delay)
				{
					stepper[hubIndex].last_accel_delay = stepper[hubIndex].new_step_delay;
					stepper[hubIndex].new_step_delay = stepper[hubIndex].min_delay;
					stepper[hubIndex].rest = 0;
					status.hub[hubIndex].stepper.state = RUN;
				}
			break;

		case RUN:
//			sm_driver_StepCounter(srd.dir);
			stepper[hubIndex].step_count++;
			stepper[hubIndex].new_step_delay = stepper[hubIndex].min_delay;
			// Check if we should start deceleration.
			if(stepper[hubIndex].step_count >= stepper[hubIndex].decel_start)
			{
				stepper[hubIndex].accel_count = stepper[hubIndex].decel_val;
				// Start deceleration with same delay as accel ended with.
				stepper[hubIndex].new_step_delay = stepper[hubIndex].last_accel_delay;
				status.hub[hubIndex].stepper.state = DECEL;
			}
			break;

		case DECEL:
//			sm_driver_StepCounter(srd.dir);
			stepper[hubIndex].step_count++;
			stepper[hubIndex].accel_count++;
			stepper[hubIndex].new_step_delay = stepper[hubIndex].step_delay - (((2 * (long)stepper[hubIndex].step_delay) + stepper[hubIndex].rest) / (4 * stepper[hubIndex].accel_count + 1));
			stepper[hubIndex].rest = ((2 * (long)stepper[hubIndex].step_delay) + stepper[hubIndex].rest) % (4 * stepper[hubIndex].accel_count + 1);
			// Check if we at last step
			if(stepper[hubIndex].accel_count >= 0)
				status.hub[hubIndex].stepper.state = STOP;
			break;
	}
	stepper[hubIndex].step_delay = stepper[hubIndex].new_step_delay;
}


