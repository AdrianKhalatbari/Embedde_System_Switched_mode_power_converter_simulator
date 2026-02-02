/*
 * uart_cmd.c
 *
 *  Created on: 27.1.2026
 *      Author: x144785
 */


#include "uart_cmd.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "xil_printf.h"
#include "xuartps.h"
#include "xparameters.h"

#include "mode.h"
#include "params.h"
#include "leds.h"
#include "lock.h"

/* ===== UART instance ===== */
#define UART_DEVICE_ID  XPAR_XUARTPS_0_DEVICE_ID

static XUartPs UartPs;

/* line buffer */
#define CMD_BUF_LEN 64
static char cmd_buf[CMD_BUF_LEN];
static int  cmd_len = 0;

/* -------- helpers -------- */

static void print_help(void)
{
    xil_printf(
        "Commands (end line with Enter):\r\n"
        "  help\r\n"
        "  status\r\n"
        "  mode idle|config|mod\r\n"
        "  kp <milli>   (e.g. kp 120 => 0.120)\r\n"
        "  ki <milli>   (e.g. ki 10  => 0.010)\r\n"
        "  ref <milli>  (e.g. ref 1000 => 1.000)\r\n"
        "  sel kp|ki\r\n"
        "\r\n"
    );
}

static void print_status(void)
{
    int kp_m = (int)(params_get_kp() * 1000.0f);
    int ki_m = (int)(params_get_ki() * 1000.0f);
    int rf_m = (int)(params_get_ref() * 1000.0f);

    system_mode_t m = get_mode();

    xil_printf("STATUS: mode=%s sel=%s Kp=%d.%03d Ki=%d.%03d REF=%d.%03d\r\n",
               (m == MODE_IDLE) ? "IDLE" : (m == MODE_CONFIG) ? "CONFIG" : "MODULATING",
               (params_get_selected() == PARAM_KP) ? "Kp" : "Ki",
               kp_m/1000, kp_m%1000,
               ki_m/1000, ki_m%1000,
               rf_m/1000, rf_m%1000);
}

/* parse integer millis and convert to float */
static float milli_to_float(int milli)
{
    return ((float)milli) / 1000.0f;
}

static void handle_line(char *line)
{
    /* trim leading spaces */
    while (*line && isspace((unsigned char)*line)) line++;

    if (*line == '\0') return;

    /* tokenize */
    char *cmd = strtok(line, " ");
    if (!cmd) return;

    if (strcmp(cmd, "help") == 0) {
        print_help();
        return;
    }

    if (strcmp(cmd, "status") == 0) {
        print_status();
        return;
    }

    if (strcmp(cmd, "mode") == 0) {
        char *arg = strtok(NULL, " ");
        if (!arg) { xil_printf("ERR: mode needs arg\r\n"); return; }

        if (strcmp(arg, "config") == 0) {
            if (!lock_take(LOCK_OWNER_UART)) {
                xil_printf("ERR: CONFIG locked\r\n");
                return;
            }
            set_mode(MODE_CONFIG);
            leds_set_for_mode(get_mode());
            xil_printf("LOCK: UART\r\n");
            print_status();
            return;
        }

        /* leaving CONFIG releases lock */
        if (strcmp(arg, "idle") == 0) {
            lock_release(LOCK_OWNER_UART);
            set_mode(MODE_IDLE);
            leds_set_for_mode(get_mode());
            xil_printf("LOCK: released\r\n");
            print_status();
            return;
        }

        if (strcmp(arg, "mod") == 0 || strcmp(arg, "modulating") == 0) {
            lock_release(LOCK_OWNER_UART);
            set_mode(MODE_MODULATING);
            leds_set_for_mode(get_mode());
            xil_printf("LOCK: released\r\n");
            print_status();
            return;
        }

        xil_printf("ERR: mode idle|config|mod\r\n");
        return;
    }

    if (strcmp(cmd, "sel") == 0) {

    	if (get_mode() != MODE_CONFIG || lock_get_owner() != LOCK_OWNER_UART)
    		{
    	        xil_printf("ERR: enter 'mode config' first (UART lock)\r\n");
    	        return;
    	    }

    	char *arg = strtok(NULL, " ");
        if (!arg) { xil_printf("ERR: sel needs kp|ki\r\n"); return; }

        if (strcmp(arg, "kp") == 0) params_set_selected(PARAM_KP);
        else if (strcmp(arg, "ki") == 0) params_set_selected(PARAM_KI);
        else { xil_printf("ERR: sel kp|ki\r\n"); return; }

        print_status();
        return;
    }

    if (strcmp(cmd, "kp") == 0 || strcmp(cmd, "ki") == 0) {

        if (get_mode() != MODE_CONFIG || lock_get_owner() != LOCK_OWNER_UART) {
            xil_printf("ERR: enter 'mode config' first (UART lock)\r\n");
            return;
        }

        char *arg = strtok(NULL, " ");
        if (!arg) { xil_printf("ERR: %s needs integer milli\r\n", cmd); return; }

        int milli = atoi(arg);
        float v = ((float)milli) / 1000.0f;

        if (strcmp(cmd, "kp") == 0) params_set_kp(v);
        else params_set_ki(v);

        print_status();
        return;
    }


    if (strcmp(cmd, "ref") == 0) {

        if (get_mode() != MODE_MODULATING) {
            xil_printf("ERR: ref only in MODULATING\r\n");
            return;
        }

        char *arg = strtok(NULL, " ");
        if (!arg) { xil_printf("ERR: ref needs integer milli\r\n"); return; }

        int milli = atoi(arg);
        float v = ((float)milli) / 1000.0f;

        params_set_ref(v);
        print_status();
        return;
    }

    xil_printf("ERR: unknown cmd. type 'help'\r\n");
}

void uart_cmd_init(void)
{
    XUartPs_Config *cfg = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (!cfg) {
        xil_printf("UART cfg not found\r\n");
        return;
    }

    int st = XUartPs_CfgInitialize(&UartPs, cfg, cfg->BaseAddress);
    if (st != XST_SUCCESS) {
        xil_printf("UART init failed\r\n");
        return;
    }

    /* 115200 8N1 (SDK Terminal default) */
    XUartPs_SetBaudRate(&UartPs, 115200);

    cmd_len = 0;
    xil_printf("UART CMD ready. type 'help'\r\n");
}

void uart_cmd_process(void)
{
    u8 c;
    int n = XUartPs_Recv(&UartPs, &c, 1);
    if (n <= 0) return; /* non-blocking */

    /* echo (optional but nice) */
    if (c != '\r' && c != '\n') {
        xil_printf("%c", c);
    }

    if (c == '\r') { /* SDK Terminal line ends with '\r' /  / see Ex4 hint / /  [oai_citation:2 Ex4_2023.pdf](sediment://file_00000000cb6471f4b6e623abcbcf193f) */
        xil_printf("\r\n");

        cmd_buf[cmd_len] = '\0';
        handle_line(cmd_buf);

        cmd_len = 0;
        return;
    }

    /* ignore '\n' */
    if (c == '\n') return;

    if (cmd_len < (CMD_BUF_LEN - 1)) {
        cmd_buf[cmd_len++] = (char)c;
    } else {
        /* buffer overflow -> reset */
        cmd_len = 0;
        xil_printf("\r\nERR: line too long\r\n");
    }
}
