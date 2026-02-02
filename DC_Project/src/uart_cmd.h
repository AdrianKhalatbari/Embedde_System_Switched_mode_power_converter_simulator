/*
 * uart_cmd.h
 *
 *  Created on: 27.1.2026
 *      Author: x144785
 */

#ifndef SRC_UART_CMD_H_
#define SRC_UART_CMD_H_

void uart_cmd_init(void);

/* Call often from main loop (non-blocking) */
void uart_cmd_process(void);

#endif /* SRC_UART_CMD_H_ */
