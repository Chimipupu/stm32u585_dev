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

/**
 * @brief コマンド受信フラグ(UART)
 */
volatile static bool s_rx_uart_cmd_flg = false;

/* Simple interrupt-driven TX/RX for LPUART1 */
#define LPUART_RX_BUF_SIZE    256
static volatile uint8_t s_lpuart_rx_buf[LPUART_RX_BUF_SIZE];
static uint8_t s_rx_buf_idx = 0;

/**
 * @brief デバッグコマンド受信確認
 * @param[out] p_cmd_buf コマンド受信バッファポインタ
 * @return true デバッグコマンド受信完了
 * @return false デバッグコマンド受信無し
 */
bool dbg_cmd_ready(uint8_t *p_cmd_buf)
{
  bool ret = false;
  uint8_t i;
  uint8_t *p_ptr = p_cmd_buf;

  // NOTE: コマンドを渡す(将来的にはDMAにさせたい)
  if(s_rx_uart_cmd_flg != false) {
    for(i = 0; i < s_rx_buf_idx; i++)
    {
      *p_ptr = s_lpuart_rx_buf[i];
      p_ptr++;
    }

    ret = true;

    // 変数初期化
    memset((void *)&s_lpuart_rx_buf[0], 0x00, LPUART_RX_BUF_SIZE);
    s_rx_buf_idx = 0;
    s_rx_uart_cmd_flg = false;
  }

  return ret;
}

/**
 * @brief LPUART割り込みハンドラ
 * 
 */
void lpuart1_irq_handler(void)
{
  volatile uint8_t tmp;

  /* RXNE */
  if (LL_LPUART_IsActiveFlag_RXNE(LPUART1)) {
      tmp = (uint8_t)LL_LPUART_ReceiveData8(LPUART1);

      // ASCIIの文字か特定の文字だけバッファで受け取る
      if ((tmp >= '0' && tmp <= '9') ||
          (tmp >= 'A' && tmp <= 'Z') || (tmp >= 'a' && tmp <= 'z') ||
          (tmp == '!') || (tmp == '?'))
      {
        s_lpuart_rx_buf[s_rx_buf_idx] = tmp;
        s_rx_buf_idx = (s_rx_buf_idx + 1) % LPUART_RX_BUF_SIZE;
      // コマンド受信 = デリミタ
      } else if ((s_rx_buf_idx > 0) && (tmp == '\r' || tmp == '\n')) {
          s_rx_uart_cmd_flg = true;
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
