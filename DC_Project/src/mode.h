/*
 * mode.h
 *
 *  Created on: 24.1.2026
 *      Author: x144785
 */

#ifndef SRC_MODE_H_
#define SRC_MODE_H_

/* System operating modes */
typedef enum {
    MODE_CONFIG = 0,
    MODE_IDLE,
    MODE_MODULATING
} system_mode_t;

/* Mode manager interface */
void set_mode(system_mode_t mode);
system_mode_t get_mode(void);

#endif /* SRC_MODE_H_ */
