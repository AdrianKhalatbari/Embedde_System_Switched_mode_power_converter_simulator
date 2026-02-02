#include "buttons.h"

#include "xgpio.h"
#include "xparameters.h"
#include "xil_printf.h"

/* ===== Your AXI GPIO for switches+buttons ===== */
#define BTN_DEVICE_ID      XPAR_AXI_GPIO_SW_BTN_DEVICE_ID

/* In many course designs (Ex3/Ex4), buttons are on channel 2. If yours are on 1, change to 1. */
#define BTN_GPIO_CHANNEL   2

/* Button bit masks in that channel (typical: 4 buttons on bits [3:0]) */
#define BTN_MASK_ALL       0x0F
#define BTN_BIT0_MASK      0x01
#define BTN_BIT1_MASK      0x02
#define BTN_BIT2_MASK      0x04
#define BTN_BIT3_MASK      0x08

/* Some boards/designs are active-low (pressed = 0). If pressing does nothing, flip to 1. */
#define BUTTONS_ACTIVE_LOW 0

static XGpio BtnGpio;

/* last normalized pressed-state (bits 0..3), used for edge detect */
static uint32_t last_pressed = 0;

void buttons_init(void)
{
    int status = XGpio_Initialize(&BtnGpio, BTN_DEVICE_ID);
    if (status != XST_SUCCESS) {
        xil_printf("GPIO(SW_BTN) init failed\r\n");
        return;
    }

    /* Set channel as input */
    XGpio_SetDataDirection(&BtnGpio, BTN_GPIO_CHANNEL, 0xFFFFFFFF);

    /* Initialize last_pressed so we don’t generate a fake event at boot */
    uint32_t v = XGpio_DiscreteRead(&BtnGpio, BTN_GPIO_CHANNEL);

#if BUTTONS_ACTIVE_LOW
    last_pressed = (~v) & BTN_MASK_ALL;
#else
    last_pressed = v & BTN_MASK_ALL;
#endif

    xil_printf("Buttons init OK (polling)\r\n");
}

/* Poll buttons, detect new presses, return events (and clear them) */
uint32_t buttons_get_events(void)
{
    uint32_t v = XGpio_DiscreteRead(&BtnGpio, BTN_GPIO_CHANNEL);

    uint32_t pressed;
#if BUTTONS_ACTIVE_LOW
    pressed = (~v) & BTN_MASK_ALL;   /* normalize: pressed = 1 */
#else
    pressed = v & BTN_MASK_ALL;
#endif

    /* rising edge: newly pressed bits */
    uint32_t rising = (pressed ^ last_pressed) & pressed;
    last_pressed = pressed;

    uint32_t ev = BTN_EVENT_NONE;

    /*
      Mapping note:
      - We map GPIO bit0..bit3 to BTN_EVENT_BTN1..BTN_EVENT_BTN4.
      - If your course uses BTN0..BTN3 naming on silkscreen, then:
          bit0 is "BTN0" physically, but in software we call it BTN_EVENT_BTN1.
        That’s OK: you only need consistent mapping for your project.
    */
    if (rising & BTN_BIT0_MASK) ev |= BTN_EVENT_BTN1;
    if (rising & BTN_BIT1_MASK) ev |= BTN_EVENT_BTN2;
    if (rising & BTN_BIT2_MASK) ev |= BTN_EVENT_BTN3;
    if (rising & BTN_BIT3_MASK) ev |= BTN_EVENT_BTN4;

    return ev;
}
