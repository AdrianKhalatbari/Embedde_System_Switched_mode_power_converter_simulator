/*
 * model.h
 *
 *  Created on: 28.1.2026
 *      Author: x144785
 */
#ifndef SRC_MODEL_H_
#define SRC_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Reset model states to zero */
void model_reset(void);

/* One discrete-time step: input uin, output u3 (measured voltage) */
float model_step(float uin);

float model_get_u3(void);

#ifdef __cplusplus
}
#endif

#endif /* SRC_MODEL_H_ */
