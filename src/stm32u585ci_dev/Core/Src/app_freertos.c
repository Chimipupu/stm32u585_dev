/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
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
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void DBGCmdTask(void *argument);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/**
 * @brief LPUART経由でprintf()相当の出力
 */
void DBG_LPUART_PRINTF(const char *format, ...)
{
#if 1
// #ifdef DEBUG_UART_USE
    char buffer[256];
    va_list args;
    int len;

    va_start(args, format);
    len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    for (int i = 0; i < len && i < sizeof(buffer); i++)
    {
        while (!LL_LPUART_IsActiveFlag_TXE(LPUART1));
        LL_LPUART_TransmitData8(LPUART1, (uint8_t)buffer[i]);
    }
#endif // DEBUG_UART_USE
}

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/**
 * @brief Bootカウント @バックアップSRAMに配置
 * @note BK_SRAMセクションに配置 (BK_SRAM (xrw) : ORIGIN = 0x40036400,  LENGTH = 2K)
 * @note BK_SRAMはVBATで保持できる2KBのSRAM
 */
volatile uint32_t g_bk_sram_boot_cnt __attribute__((section(".bk_sram"))) = 0;

osThreadId_t dbgCmdTaskHandle;
const osThreadAttr_t dbgCmdTask_attributes = {
  .name = "dbgCmdTask",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 256 * 4
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for appMainTask */
osThreadId_t appMainTaskHandle;
const osThreadAttr_t appMainTask_attributes = {
  .name = "appMainTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  appMainTaskHandle = osThreadNew(AppMainTask, NULL, &appMainTask_attributes);
  if (appMainTaskHandle == NULL) {
    DBG_LPUART_PRINTF("ERR: appMainTask create failed\r\n");
  } else {
    DBG_LPUART_PRINTF("OK: appMainTask created\r\n");
  }

  dbgCmdTaskHandle = osThreadNew(DBGCmdTask, NULL, &dbgCmdTask_attributes);
  if (dbgCmdTaskHandle == NULL) {
    DBG_LPUART_PRINTF("ERR: dbgCmdTask create failed\r\n");
  } else {
    DBG_LPUART_PRINTF("OK: dbgCmdTask created\r\n");
  }

  // defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END defaultTask */
}

/* USER CODE BEGIN Header_AppMainTask */
/**
* @brief Function implementing the appMainTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AppMainTask */
void AppMainTask(void *argument)
{
  /* USER CODE BEGIN appMainTask */

  // バックアップSRAMのBootカウントをインクリメント
  DBG_LPUART_PRINTF("[DEBUG] Boot Cnt = %d\r\n", g_bk_sram_boot_cnt);
  g_bk_sram_boot_cnt++;

  DBG_LPUART_PRINTF("AppMainTask\r\n");

  /* Infinite loop */
  for(;;)
  {
    // (DEBUG)基板のLEDをトグル
    LL_GPIO_TogglePin(OB_LED_GPIO_Port, OB_LED_Pin);
    osDelay(100);
  }
  /* USER CODE END appMainTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
 * @brief デバッグコマンドタスク
 * 
 * @param argument 未使用
 */
void DBGCmdTask(void *argument)
{
  bool ret;
  uint8_t cmd_buf[64] = {0};

  DBG_LPUART_PRINTF("DBGCmdTask\r\n");

  for(;;)
  {
    ret = dbg_cmd_ready(cmd_buf);
    if (ret != false) {
      DBG_LPUART_PRINTF("[DEBUG] Cmd: %s\r\n", cmd_buf);
      memset(&cmd_buf[0], 0x00, sizeof(cmd_buf));
    }
    osDelay(200);
  }
}
/* USER CODE END Application */

