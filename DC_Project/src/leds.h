#ifndef SRC_LEDS_H_
#define SRC_LEDS_H_

#include "mode.h"

/* Initialize LED hardware */
void leds_init(void);

/* Update LEDs according to current mode */
void leds_set_for_mode(system_mode_t mode);

#endif /* SRC_LEDS_H_ */
