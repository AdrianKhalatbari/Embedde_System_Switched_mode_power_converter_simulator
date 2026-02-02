/*
 * buttons.h
 *
 *  Created on: 24.1.2026
 *      Author: x144785
 */

#ifndef SRC_BUTTONS_H_
#define SRC_BUTTONS_H_

#include <stdint.h>

/* Button event flags */
#define BTN_EVENT_NONE   0x00
#define BTN_EVENT_BTN1   0x01
#define BTN_EVENT_BTN2   0x02
#define BTN_EVENT_BTN3   0x04
#define BTN_EVENT_BTN4   0x08

/* Initialize button hardware and interrupts */
void buttons_init(void);

/* Get and clear pending button events */
uint32_t buttons_get_events(void);

void buttons_isr_handler(uint32_t btn_mask);

#endif /* SRC_BUTTONS_H_ */
