
#ifndef STEPPER_H_
#define STEPPER_H_

#include "stdint.h"

#define CW 0
#define CCW 1

typedef enum
{
	STOP,
	ACCEL,
	RUN,
	DECEL
} e_run_state_t;

typedef struct
{
	uint32_t min_delay;
	int32_t step_delay;
	int32_t new_step_delay;
	uint32_t last_accel_delay;

	uint32_t rest;
	uint32_t n;

	uint8_t dir;

	int32_t step_count;
	int32_t accel_count;
	int32_t decel_start;
	int32_t decel_val;
	uint32_t max_s_lim;
	uint32_t accel_lim;
} stepper_t;

typedef struct {
  //! What part of the speed ramp we are in.
  unsigned char run_state : 3;
  //! Direction stepper motor should move.
  unsigned char dir : 1;
  //! Peroid of next timer delay. At start this value set the accelration rate.
  unsigned int step_delay;
  //! What step_pos to start decelaration
  unsigned int decel_start;
  //! Sets deceleration rate.
  signed int decel_val;
  //! Minimum time delay (max speed)
  signed int min_delay;
  //! Counter used when accelerateing/decelerateing to calculate step_delay.
  signed int accel_count;
} speedRampData;

/*! \Brief Frequency of timer1 in [Hz].
 *
 * Modify this according to frequency used. Because of the prescaler setting,
 * the timer1 frequency is the clock frequency divided by 8.
 */
// Timer/Counter 1 running on 3,686MHz / 8 = 460,75kHz (2,17uS). (T1-FREQ 460750)
#define T5_FREQ 10500000

//! Number of (full)steps per round on stepper motor in use.
#define SPR 200

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T5_FREQ*100))     // (ALPHA / T5_FREQ)*100
#define T5_FREQ_148 ((int)((T5_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000)         // ALPHA*2*10000000000
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

//! Global status flags
extern struct GLOBAL_FLAGS stepper_status;
extern void stepperDelay(uint8_t hubIndex);
extern void stepperRel(uint8_t hubIndex, int16_t step, uint32_t accel, uint32_t decel, uint32_t speed);
extern void stepperHome(uint8_t hubIndex);

#endif /* STEPPER_H_ */
