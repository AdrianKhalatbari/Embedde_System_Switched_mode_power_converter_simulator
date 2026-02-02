/*
 * mode.c
 *
 *  Created on: 24.1.2026
 *      Author: x144785
 */


#include "mode.h"
#include <stdio.h>

/* Current system mode (private to this file) */
static system_mode_t current_mode = MODE_IDLE;

/* Set system mode */
void set_mode(system_mode_t mode)
{
    current_mode = mode;

    switch (current_mode) {
    case MODE_CONFIG:
        printf("Mode: CONFIG\r\n");
        break;
    case MODE_IDLE:
        printf("Mode: IDLE\r\n");
        break;
    case MODE_MODULATING:
        printf("Mode: MODULATING\r\n");
        break;
    default:
        break;
    }
}

/* Get current system mode */
system_mode_t get_mode(void)
{
    return current_mode;
}
