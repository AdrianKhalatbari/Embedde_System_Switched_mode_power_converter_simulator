/*
 * model.c
 *
 *  Created on: 28.1.2026
 *      Author: x144785
 */


#include "model.h"
#include "xil_printf.h"

/*
 * Discrete state-space model (Eq. 3)
 *
 * x(k+1) = A * x(k) + B * uin(k)
 * y(k)   = u3 = x[5]
 *
 * x = [ i1  u1  i2  u2  i3  u3 ]^T
 */

static float x[6];   /* state vector */

/* Reset model states */
void model_reset(void)
{
    for (int i = 0; i < 6; i++) {
        x[i] = 0.0f;
    }
}

/* One model step */
float model_step(float uin)
{



    /* A matrix (Eq. 3) */
    static const float A[6][6] = {
        { 0.9652f, -0.0172f,  0.0057f, -0.0058f,  0.0052f, -0.0251f },
        { 0.7732f,  0.1252f,  0.2315f,  0.07f,    0.1282f,  0.7754f },
        { 0.8278f, -0.7522f, -0.0956f,  0.3299f, -0.4855f,  0.3915f },
        { 0.9948f,  0.2655f, -0.3848f,  0.4212f,  0.3927f,  0.2899f },
        { 0.7648f, -0.4165f, -0.4855f, -0.3366f, -0.0986f,  0.7281f },
        { 1.1056f,  0.7587f,  0.1179f,  0.0748f, -0.2192f,  0.1491f }
    };

    /* B vector (Eq. 3) */
    static const float B[6] = {
        0.0471f,
        0.0377f,
        0.0404f,
        0.0485f,
        0.0373f,
        0.0539f
    };



    float xn[6];

    /* x(k+1) = A*x(k) + B*uin */
    for (int i = 0; i < 6; i++) {
        float sum = 0.0f;
        for (int j = 0; j < 6; j++) {
            sum += A[i][j] * x[j];
        }
        sum += B[i] * uin;
        xn[i] = sum;
    }

    /* Update states */
    for (int i = 0; i < 6; i++) {
        x[i] = xn[i];
    }

    /* Output u3 */
    return x[5];
}
