/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "hub.h"
#include "heartbeat.h"
#include "analog.h"
#include "adc.h"
#include "stepper.h"
#include "bmp280.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Default */
osThreadId_t DefaultHandle;
const osThreadAttr_t Default_attributes = {
  .name = "Default",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};
/* Definitions for analog */
osThreadId_t analogHandle;
const osThreadAttr_t analog_attributes = {
  .name = "analog",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};
/* Definitions for communication */
osThreadId_t communicationHandle;
const osThreadAttr_t communication_attributes = {
  .name = "communication",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512
};
/* Definitions for heartBeatTimer */
osTimerId_t heartBeatTimerHandle;
const osTimerAttr_t heartBeatTimer_attributes = {
  .name = "heartBeatTimer"
};
/* Definitions for envelopeTimeoutTimer */
osTimerId_t envelopeTimeoutTimerHandle;
const osTimerAttr_t envelopeTimeoutTimer_attributes = {
  .name = "envelopeTimeoutTimer"
};
/* Definitions for hubWatchDogTimer */
osTimerId_t hubWatchDogTimerHandle;
const osTimerAttr_t hubWatchDogTimer_attributes = {
  .name = "hubWatchDogTimer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void analogTask(void *argument);
extern void communicationTask(void *argument);
extern void heartBeat(void *argument);
extern void envelopeTimeout(void *argument);
extern void hubWatchDog(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of heartBeatTimer */
  heartBeatTimerHandle = osTimerNew(heartBeat, osTimerPeriodic, NULL, &heartBeatTimer_attributes);

  /* creation of envelopeTimeoutTimer */
  envelopeTimeoutTimerHandle = osTimerNew(envelopeTimeout, osTimerOnce, NULL, &envelopeTimeoutTimer_attributes);

  /* creation of hubWatchDogTimer */
  hubWatchDogTimerHandle = osTimerNew(hubWatchDog, osTimerPeriodic, NULL, &hubWatchDogTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Default */
  DefaultHandle = osThreadNew(StartDefaultTask, NULL, &Default_attributes);

  /* creation of analog */
  analogHandle = osThreadNew(analogTask, NULL, &analog_attributes);

  /* creation of communication */
  communicationHandle = osThreadNew(communicationTask, NULL, &communication_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the Default thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

	bmp280_init_default_params(&bmp280.params);
	bmp280.addr = BMP280_I2C_ADDRESS_0;
	bmp280.i2c = &hi2c1;

	if(!bmp280_init(&bmp280, &bmp280.params))
	{
		__asm__("bkpt");
	}

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) ADCBuffer, 12);
	osTimerStart(heartBeatTimerHandle, 1000U);
	osTimerStart(hubWatchDogTimerHandle, HUB_WATCH_DOG_INTERVAL_MS);

	TIM1->SR &= ~0;
	TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E);
	TIM1->DIER |= (TIM_DIER_CC1IE | TIM_DIER_CC2IE | TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_UIE);
	TIM1->CR1 |= TIM_CR1_CEN;

	TIM2->CR1 |= TIM_CR1_CEN;
	TIM5->CR1 |= TIM_CR1_CEN;
	TIM8->CR1 |= TIM_CR1_CEN;

	stepperRel(0, 100, 10000 * ALPHA * 100, 10000 * ALPHA * 100, 1000 * ALPHA * 100);

  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
