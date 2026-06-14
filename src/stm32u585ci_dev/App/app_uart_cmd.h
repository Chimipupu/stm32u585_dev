/**
 * @file app_uart_cmd.h
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief アプリ: UARTコマンド
 * @version 0.1
 * @date 2026-06-14
 * @copyright Copyright (c) 2026 Chimipupu All Rights Reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
void lpuart1_irq_handler(void);
void app_uart_cmd_init(void);
void app_uart_cmd_main(void);
// --------------------------------------------------------------------------