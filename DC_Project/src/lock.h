/*
 * lock.h
 *
 *  Created on: 27.1.2026
 *      Author: x144785
 */

#ifndef SRC_LOCK_H_
#define SRC_LOCK_H_

#include <stdint.h>

typedef enum {
    LOCK_OWNER_NONE = 0,
    LOCK_OWNER_UART = 1
} lock_owner_t;

/* Try to take lock for owner. Returns 1 if taken, 0 if already taken. */
int lock_take(lock_owner_t owner);

/* Release lock (only owner can release). */
void lock_release(lock_owner_t owner);

/* Query current owner */
lock_owner_t lock_get_owner(void);

#endif /* SRC_LOCK_H_ */
