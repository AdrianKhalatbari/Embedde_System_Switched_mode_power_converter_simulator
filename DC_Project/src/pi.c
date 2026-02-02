/*
 * pi.c
 *
 *  Created on: 28.1.2026
 *      Author: x144785
 */


#include "pi.h"

static float clampf(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

void pi_init(pi_t *pi, float kp, float ki, float u_min, float u_max)
{
    pi->kp = kp;
    pi->ki = ki;
    pi->integ = 0.0f;
    pi->u_min = u_min;
    pi->u_max = u_max;
}

float pi_step(pi_t *pi, float ref, float meas)
{
    float e = ref - meas;

    /* Integrator (no dt for minimum-pass) */
    pi->integ += (pi->ki * e);
    pi->integ = clampf(pi->integ, pi->u_min, pi->u_max);

    /* PI output */
    float u = (pi->kp * e) + pi->integ;
    u = clampf(u, pi->u_min, pi->u_max);

    return u;
}
