/*
 * params.h
 *
 *  Created on: 26.1.2026
 *      Author: x144785
 */

#ifndef SRC_PARAMS_H_
#define SRC_PARAMS_H_

#include <stdint.h>

typedef enum {
    PARAM_KP = 0,
    PARAM_KI
} param_sel_t;

void params_init(void);

/* selection for CONFIG mode */
void params_toggle_selected(void);
param_sel_t params_get_selected(void);

/* Kp / Ki */
float params_get_kp(void);
float params_get_ki(void);
void  params_inc_selected(void);
void  params_dec_selected(void);

/* Reference for MODULATING */
float params_get_ref(void);
void  params_inc_ref(void);
void  params_dec_ref(void);

void params_set_kp(float v);
void params_set_ki(float v);
void params_set_ref(float v);
void params_set_selected(param_sel_t s);

#endif /* SRC_PARAMS_H_ */
