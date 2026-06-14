/**
 * @file app_uart_cmd.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief アプリ: UARTコマンド
 * @version 0.1
 * @date 2026-06-14
 * @copyright Copyright (c) 2026 Chimipupu All Rights Reserved.
 */

// My Src
#include "app_uart_cmd.h"
#include "pcb_define.h"
#include "app_main.h"
#include "app_util.h"
#include <string.h>

// --------------------------------------------------------------------------
// コマンド実行結果
typedef enum {
    CMD_RESULT_NONE                 = 0x00, // コマンド処理なし
    CMD_RESULT_EXEC_OK              = 0x01, // コマンド実行成功
    CMD_RESULT_EXEC_ERROR           = 0x02, // コマンド実行失敗
    CMD_RESULT_EXEC_IN_PROGRESS     = 0x03, // コマンド実行中
    CMD_RESULT_ARGS_ERROR           = 0xF0, // コマンド引数エラー
    CMD_RESULT_UNKNOWN_ERROR        = 0xFF, // 不明なエラー
} E_APP_UART_CMD_RESULT;

// コマンドコールバック関数
typedef E_APP_UART_CMD_RESULT(*p_cmd_func)(void *p_args);

// コマンドテーブル構造体
typedef struct {
    const char *p_cmd_str;          // コマンド文字列
    const char *p_cmd_str_short;    // コマンド短縮文字列
    p_cmd_func pfunc;               // コールバック関数
} app_uart_cmd_tbl_t;

// --------------------------------------------------------------------------
#define LPUART_RX_BUF_SIZE    128
static volatile uint8_t s_lpuart_rx_buf[LPUART_RX_BUF_SIZE];
static uint8_t s_rx_buf_idx = 0;

#define UART_CMD_RX_BUF_SIZE    64
static volatile uint8_t s_uart_cmd_rx_buf[UART_CMD_RX_BUF_SIZE];
static bool s_rx_uart_cmd_flg = false;

E_APP_UART_CMD_RESULT _cmd_help(void *p_args);
E_APP_UART_CMD_RESULT _cmd_rtc(void *p_args);

// コマンドテーブル
static const app_uart_cmd_tbl_t g_cmd_tbl[] = {
    {"help", "?", _cmd_help},
#ifdef PRINT_RTC_UPDATE
    {"rtc", "t", _cmd_rtc},
#endif // PRINT_RTC_UPDATE
};
static const uint8_t CMD_TBL_NUM = sizeof(g_cmd_tbl) / sizeof(g_cmd_tbl[0]);

static uint8_t s_rx_cmd_no;

bool _cmd_ready(uint8_t *p_cmd_buf);
// --------------------------------------------------------------------------
// [Static]

bool _cmd_ready(uint8_t *p_cmd_buf)
{
    bool ret = false;

    uint8_t i;
    uint8_t *p_ptr = p_cmd_buf;
    app_uart_cmd_tbl_t *p_tbl;

    for(i = 0; i < s_rx_buf_idx; i++)
    {
        if((s_lpuart_rx_buf[i] == '\r') || (s_lpuart_rx_buf[i] == '\n')) {
            break;
        }

        *p_ptr = s_lpuart_rx_buf[i];
        p_ptr++;
    }

    p_tbl = (app_uart_cmd_tbl_t *) &g_cmd_tbl[0];

    // テーブル検索
    for(i = 0; i < CMD_TBL_NUM; i++)
    {
        // バッファとテーブルのコマンド文字列を比較
        if(strcmp(p_tbl->p_cmd_str, (const char *) p_cmd_buf) == 0) {
            DBG_LPUART_PRINTF("[DEBUG] Cmd RX: %s\r\n", p_cmd_buf);
            s_rx_cmd_no = i;
            ret = true;
            break;
        }
        p_tbl++;
    }

    // 知らんコマンドなので「What?」を返す
    if(ret == false) {
        DBG_LPUART_PRINTF("What?\r\n");
    }

    // 変数初期化
    memset((void *)&s_lpuart_rx_buf[0], 0x00, LPUART_RX_BUF_SIZE);
    memset((void *) &s_uart_cmd_rx_buf[0], 0x00, UART_CMD_RX_BUF_SIZE);
    s_rx_buf_idx = 0;
    s_rx_uart_cmd_flg = false;

    return ret;
}

E_APP_UART_CMD_RESULT _cmd_help(void *p_args)
{
    uint8_t i;

    DBG_LPUART_PRINTF("Help CMD Exec\r\n");

    DBG_LPUART_PRINTF("CMD List: NO, Cmd, Short Cmd\r\n");
    for(i = 0; i < CMD_TBL_NUM; i++)
    {
        DBG_LPUART_PRINTF("%d, %s, %s\r\n", i, g_cmd_tbl[i].p_cmd_str, g_cmd_tbl[i].p_cmd_str_short);
    }

    return CMD_RESULT_EXEC_OK;
}

#ifdef PRINT_RTC_UPDATE
E_APP_UART_CMD_RESULT _cmd_rtc(void *p_args)
{
    app_rtc_update();

    return CMD_RESULT_EXEC_OK;
}
#endif // PRINT_RTC_UPDATE
// --------------------------------------------------------------------------
// [APP]

void lpuart1_irq_handler(void)
{
    uint8_t tmp;

#if 0
    // ORE （オーバーランエラー）
    if (LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
        LL_LPUART_ClearFlag_ORE(LPUART1);
    }
#endif

    /**
    * @brief RXFNE (Receive FIFO Not Empty) をチェック
    * @note LPUARTのFIFOサイズ = 8Byte
    * @note RXFNEの割り込みはFIFOが1/8埋まったら来る
    * @note CPUが割り込みハンドラに来るまでに残りの7Byteは埋まるからORE（オーバーランエラー）は起きにくい、はず
    */
    while (LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1))
    {
        tmp = (uint8_t)LL_LPUART_ReceiveData8(LPUART1);
        if ((tmp == '\r') || (tmp == '\n')) {
            s_rx_uart_cmd_flg = true;
        }

        s_lpuart_rx_buf[s_rx_buf_idx] = tmp;
        s_rx_buf_idx = (s_rx_buf_idx + 1) % LPUART_RX_BUF_SIZE;
    }
}

void app_uart_cmd_init(void)
{
    memset((void *)&s_lpuart_rx_buf[0], 0x00, LPUART_RX_BUF_SIZE);
    memset((void *) &s_uart_cmd_rx_buf[0], 0x00, UART_CMD_RX_BUF_SIZE);
    s_rx_buf_idx = 0;
    s_rx_uart_cmd_flg = false;
}

void app_uart_cmd_main(void)
{
    bool ret;

    if(s_rx_uart_cmd_flg != false) {
        ret = _cmd_ready((uint8_t *) &s_uart_cmd_rx_buf[0]);
        if (ret != false) {
            g_cmd_tbl[s_rx_cmd_no].pfunc(NULL); // コマンド実行
        }
    }
}
// --------------------------------------------------------------------------