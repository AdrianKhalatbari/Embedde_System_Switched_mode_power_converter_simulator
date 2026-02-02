/*
 * params.c
 *
 *  Created on: 26.1.2026
 *      Author: x144785
 */


#include "params.h"

/* Simple defaults (minimum-pass) */
static float kp = 0.10f;
static float ki = 0.01f;
static float ref = 1.00f;

static param_sel_t sel = PARAM_KP;

/* step sizes */
#define KP_STEP   0.01f
#define KI_STEP   0.001f
#define REF_STEP  0.10f

/* simple limits to avoid nonsense */
#define KP_MIN 0.0f
#define KP_MAX 5.0f
#define KI_MIN 0.0f
#define KI_MAX 5.0f
#define REF_MIN 0.0f
#define REF_MAX 5.0f

static float clamp(float x, float lo, float hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

void params_init(void)
{
    kp  = 0.10f;
    ki  = 0.01f;
    ref = 1.00f;
    sel = PARAM_KP;
}

void params_toggle_selected(void)
{
    sel = (sel == PARAM_KP) ? PARAM_KI : PARAM_KP;
}

param_sel_t params_get_selected(void) { return sel; }

float params_get_kp(void) { return kp; }
float params_get_ki(void) { return ki; }

void params_inc_selected(void)
{
    if (sel == PARAM_KP) kp = clamp(kp + KP_STEP, KP_MIN, KP_MAX);
    else                ki = clamp(ki + KI_STEP, KI_MIN, KI_MAX);
}

void params_dec_selected(void)
{
    if (sel == PARAM_KP) kp = clamp(kp - KP_STEP, KP_MIN, KP_MAX);
    else                ki = clamp(ki - KI_STEP, KI_MIN, KI_MAX);
}

float params_get_ref(void) { return ref; }

void params_inc_ref(void) { ref = clamp(ref + REF_STEP, REF_MIN, REF_MAX); }
void params_dec_ref(void) { ref = clamp(ref - REF_STEP, REF_MIN, REF_MAX); }

void params_set_kp(float v) { kp = clamp(v, KP_MIN, KP_MAX); }
void params_set_ki(float v) { ki = clamp(v, KI_MIN, KI_MAX); }
void params_set_ref(float v){ ref = clamp(v, REF_MIN, REF_MAX); }
void params_set_selected(param_sel_t s){ sel = s; }
