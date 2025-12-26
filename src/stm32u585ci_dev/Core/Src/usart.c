/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

/* Simple interrupt-driven TX/RX for LPUART1 */
#define LPUART1_RX_BUF_SIZE 256
static volatile uint8_t lpuart1_rx_buf[LPUART1_RX_BUF_SIZE];
static volatile uint16_t lpuart1_rx_head = 0;
static volatile uint16_t lpuart1_rx_tail = 0;

int lpuart1_rx_available(void)
{
  uint16_t head = lpuart1_rx_head;
  uint16_t tail = lpuart1_rx_tail;
  return (head >= tail) ? (head - tail) : (LPUART1_RX_BUF_SIZE - tail + head);
}

int lpuart1_read_rx(uint8_t *buf, int buf_len)
{
  if (buf_len <= 0) return 0;
  int cnt = 0;
  while (cnt < buf_len && lpuart1_rx_tail != lpuart1_rx_head) {
    buf[cnt++] = lpuart1_rx_buf[lpuart1_rx_tail++];
    if (lpuart1_rx_tail >= LPUART1_RX_BUF_SIZE) lpuart1_rx_tail = 0;
  }
  return cnt;
}

void lpuart1_irq_handler(void)
{
  volatile uint8_t tmp;

  /* RXNE */
  if (LL_LPUART_IsActiveFlag_RXNE(LPUART1)) {
    tmp = (uint8_t)LL_LPUART_ReceiveData8(LPUART1);
    /* store only ASCII alphanumeric characters */
    if ((tmp >= '0' && tmp <= '9') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= 'a' && tmp <= 'z')) {
      uint16_t next = (lpuart1_rx_head + 1) % LPUART1_RX_BUF_SIZE;
      if (next != lpuart1_rx_tail) {
        lpuart1_rx_buf[lpuart1_rx_head] = tmp;
        lpuart1_rx_head = next;
      }
    }
  }
}
/* USER CODE END 0 */

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  LL_LPUART_InitTypeDef LPUART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK3);

  /* Peripheral clock enable */
  LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_LPUART1);

  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**LPUART1 GPIO Configuration
  PA2   ------> LPUART1_TX
  PA3   ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_8;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* LPUART1 interrupt Init */
  NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(LPUART1_IRQn);

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
  LPUART_InitStruct.BaudRate = 921600;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
  LL_LPUART_SetTXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(LPUART1, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_DisableFIFO(LPUART1);
  LL_LPUART_Enable(LPUART1);
  /* USER CODE BEGIN LPUART1_Init 2 */
  LL_LPUART_EnableIT_RXNE(LPUART1);
  // LL_LPUART_EnableIT_TXE(LPUART1);
  /* USER CODE END LPUART1_Init 2 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
