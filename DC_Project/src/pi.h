/*
 * pi.h
 *
 *  Created on: 28.1.2026
 *      Author: x144785
 */

#ifndef SRC_PI_H_
#define SRC_PI_H_

typedef struct {
    float kp;
    float ki;
    float integ;
    float u_min;
    float u_max;
} pi_t;

void  pi_init(pi_t *pi, float kp, float ki, float u_min, float u_max);
float pi_step(pi_t *pi, float ref, float meas);

#endif /* SRC_PI_H_ */
