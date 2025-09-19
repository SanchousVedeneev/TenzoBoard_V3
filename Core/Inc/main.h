/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32g4xx_hal.h"

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
#define TIM1_Pulse ((TIM1_ARR+1 ) / 2)
#define TIM6_Presc (1680-1)
#define TIM7_ARR (1000)
#define TIM6_ARR (30000)
#define TIM7_Presc (16800-1)
#define TIM1_ARR (2734-1)
#define TIM1_Presc (2-1)
#define DO_LED_1_Pin GPIO_PIN_13
#define DO_LED_1_GPIO_Port GPIOC
#define DO_OUT_1_Pin GPIO_PIN_1
#define DO_OUT_1_GPIO_Port GPIOC
#define DI_IN_1_Pin GPIO_PIN_2
#define DI_IN_1_GPIO_Port GPIOC
#define AI_NTC_Pin GPIO_PIN_0
#define AI_NTC_GPIO_Port GPIOA
#define DO_LED_2_Pin GPIO_PIN_2
#define DO_LED_2_GPIO_Port GPIOA
#define DO_SLEEP_Pin GPIO_PIN_4
#define DO_SLEEP_GPIO_Port GPIOC
#define DI_ADR_5_Pin GPIO_PIN_10
#define DI_ADR_5_GPIO_Port GPIOC
#define DI_ADR_4_Pin GPIO_PIN_11
#define DI_ADR_4_GPIO_Port GPIOC
#define DI_ADR_3_Pin GPIO_PIN_12
#define DI_ADR_3_GPIO_Port GPIOC
#define DI_ADR_2_Pin GPIO_PIN_2
#define DI_ADR_2_GPIO_Port GPIOD
#define DI_ADR_1_Pin GPIO_PIN_3
#define DI_ADR_1_GPIO_Port GPIOB
#define DI_ADR_0_Pin GPIO_PIN_4
#define DI_ADR_0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
