/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HUB_TRIG_1_Pin GPIO_PIN_9
#define HUB_TRIG_1_GPIO_Port GPIOE
#define HUB_TRIG_2_Pin GPIO_PIN_11
#define HUB_TRIG_2_GPIO_Port GPIOE
#define HUB_TRIG_3_Pin GPIO_PIN_13
#define HUB_TRIG_3_GPIO_Port GPIOE
#define HUB_TRIG_4_Pin GPIO_PIN_14
#define HUB_TRIG_4_GPIO_Port GPIOE
#define HX711_3_DO_Pin GPIO_PIN_8
#define HX711_3_DO_GPIO_Port GPIOD
#define HX711_3_DO_EXTI_IRQn EXTI9_5_IRQn
#define HX711_4_DO_Pin GPIO_PIN_9
#define HX711_4_DO_GPIO_Port GPIOD
#define HX711_4_DO_EXTI_IRQn EXTI9_5_IRQn
#define HX711_1_SCK_Pin GPIO_PIN_2
#define HX711_1_SCK_GPIO_Port GPIOD
#define HX711_2_SCK_Pin GPIO_PIN_3
#define HX711_2_SCK_GPIO_Port GPIOD
#define HX711_3_SCK_Pin GPIO_PIN_4
#define HX711_3_SCK_GPIO_Port GPIOD
#define HX711_4_SCK_Pin GPIO_PIN_5
#define HX711_4_SCK_GPIO_Port GPIOD
#define HX711_1_DO_Pin GPIO_PIN_6
#define HX711_1_DO_GPIO_Port GPIOD
#define HX711_1_DO_EXTI_IRQn EXTI9_5_IRQn
#define HX711_2_DO_Pin GPIO_PIN_7
#define HX711_2_DO_GPIO_Port GPIOD
#define HX711_2_DO_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
