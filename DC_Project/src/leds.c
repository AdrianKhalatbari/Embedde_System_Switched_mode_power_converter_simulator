/*
 * leds.c
 *
 *  Created on: 26.1.2026
 *      Author: x144785
 */


#include "leds.h"

#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"

/* AXI GPIO for LEDs */
#define LED_DEVICE_ID     XPAR_AXI_GPIO_LED_DEVICE_ID
#define LED_GPIO_CHANNEL  1   /* usually channel 1 for LEDs */

/* LED bit masks */
#define LED0_MASK  0x01
#define LED1_MASK  0x02
#define LED2_MASK  0x04
#define LED3_MASK  0x08

static XGpio LedGpio;

void leds_init(void)
{
    int status = XGpio_Initialize(&LedGpio, LED_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("LED GPIO init failed\r\n");
        return;
    }

    /* LEDs are outputs */
    XGpio_SetDataDirection(&LedGpio, LED_GPIO_CHANNEL, 0x00);

    /* Turn all LEDs off initially */
    XGpio_DiscreteWrite(&LedGpio, LED_GPIO_CHANNEL, 0x00);

    xil_printf("LEDs init OK\r\n");
}

void leds_set_for_mode(system_mode_t mode)
{
    uint32_t led_value = 0x00;

    switch (mode) {
    case MODE_IDLE:
        led_value = LED0_MASK;   /* LD0 ON */
        break;

    case MODE_CONFIG:
        led_value = LED1_MASK;   /* LD1 ON */
        break;

    case MODE_MODULATING:
        led_value = LED2_MASK | LED3_MASK;
        break;


    default:
        led_value = 0x00;
        break;
    }


    XGpio_DiscreteWrite(&LedGpio, LED_GPIO_CHANNEL, led_value);
}
