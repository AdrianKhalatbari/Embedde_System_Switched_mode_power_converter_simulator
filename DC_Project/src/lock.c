/*
 * lock.c
 *
 *  Created on: 27.1.2026
 *      Author: x144785
 */


#include "lock.h"
#include "xil_exception.h"

static volatile lock_owner_t g_owner = LOCK_OWNER_NONE;

int lock_take(lock_owner_t owner)
{
    int ok = 0;

    Xil_ExceptionDisable();
    if (g_owner == LOCK_OWNER_NONE) {
        g_owner = owner;
        ok = 1;
    }
    Xil_ExceptionEnable();

    return ok;
}

void lock_release(lock_owner_t owner)
{
    Xil_ExceptionDisable();
    if (g_owner == owner) {
        g_owner = LOCK_OWNER_NONE;
    }
    Xil_ExceptionEnable();
}

lock_owner_t lock_get_owner(void)
{
    lock_owner_t o;
    Xil_ExceptionDisable();
    o = g_owner;
    Xil_ExceptionEnable();
    return o;
}
