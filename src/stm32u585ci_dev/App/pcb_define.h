/**
 * @file pcb_define.h
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief 基板定義 (STM32U585CIU6)
 * @version 0.1
 * @date 2026-06-07
 * @copyright Copyright (c) 2026 Chimipupu All Rights Reserved.
 */

#ifndef PCB_DEFINE_H
#define PCB_DEFINE_H

#include "stm32u5xx_hal.h"

// -----------------------------------------------------------
// [コンパイルスイッチ]
#define PCB_WEACT_STM32U585CIU6

// -----------------------------------------------------------
#ifdef PCB_WEACT_STM32U585CIU6
// 基板青色LED @PC13
#define PCB_LED_PORT       GPIOC
#define PCB_LED_PIN        GPIO_PIN_13

// 基板ボタン @PA0
#define PCB_BUTTON_PORT    GPIOA
#define PCB_BUTTON_PIN     GPIO_PIN_0

#define UART_BAUD         921600
#endif // PCB_WEACT_STM32U585CIU6

// -----------------------------------------------------------

#endif // PCB_DEFINE_H