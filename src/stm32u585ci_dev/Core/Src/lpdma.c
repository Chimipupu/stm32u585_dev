/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lpdma.c
  * @brief   This file provides code for the configuration
  *          of the LPDMA instances.
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
/* Includes ------------------------------------------------------------------*/
#include "lpdma.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* LPDMA1 init function */
void MX_LPDMA1_Init(void)
{

  /* USER CODE BEGIN LPDMA1_Init 0 */

  /* USER CODE END LPDMA1_Init 0 */

  /* Peripheral clock enable */
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_LPDMA1);

  /* LPDMA1 interrupt Init */
  NVIC_SetPriority(LPDMA1_Channel0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(LPDMA1_Channel0_IRQn);

  /* USER CODE BEGIN LPDMA1_Init 1 */

  /* USER CODE END LPDMA1_Init 1 */
  /* USER CODE BEGIN LPDMA1_Init 2 */

  /* USER CODE END LPDMA1_Init 2 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
