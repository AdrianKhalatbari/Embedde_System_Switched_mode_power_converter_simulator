#include "platform.h"
#include "xil_printf.h"

#include "mode.h"
#include "buttons.h"
#include "leds.h"
#include "params.h"
#include "uart_cmd.h"
#include "lock.h"
#include "pi.h"
#include "model.h"
#include "xtime_l.h"

/* Button event mapping (from your test) */
#define EV_BTN0  BTN_EVENT_BTN1
#define EV_BTN1  BTN_EVENT_BTN2
#define EV_BTN2  BTN_EVENT_BTN3
#define EV_BTN3  BTN_EVENT_BTN4

static void print_config_status(void)
{
    int kp_m = (int)(params_get_kp() * 1000.0f);
    int ki_m = (int)(params_get_ki() * 1000.0f);

    xil_printf(
        "CONFIG: %s | Kp=%d.%03d Ki=%d.%03d\r\n",
        (params_get_selected() == PARAM_KP) ? "Kp" : "Ki",
        kp_m / 1000, kp_m % 1000,
        ki_m / 1000, ki_m % 1000
    );
}

static void print_ref(void)
{
    int ref_m = (int)(params_get_ref() * 1000.0f);
    xil_printf("REF=%d.%03d\r\n", ref_m / 1000, ref_m % 1000);
}

int main(void)
{
    init_platform();

    xil_printf("Init Ok\r\n");

    buttons_init();

    leds_init();

    params_init();

    uart_cmd_init();

    model_reset();

    pi_t pi;
    pi_init(&pi, 0.5f, 0.1f, 0.0f, 1.0f);

    /* Fake measurement starts at 0, reference is 1 */
    float ref = 1.0f;
    float meas = 0.0f;

    /* This is just a test for step 6 */
    // for (int k = 0; k < 10; k++) {
    //     float u = pi_step(&pi, ref, meas);
    //
    //     /* Fake plant: measurement moves a bit toward u */
    //     meas = meas + 0.5f * (u - meas);
    //
    //     int u_m = (int)(u * 1000.0f);
    //     int m_m = (int)(meas * 1000.0f);
    //
    //     xil_printf("k=%d u=%d.%03d meas=%d.%03d\r\n",
    //             k, u_m/1000, u_m%1000, m_m/1000, m_m%1000);
    // }


    set_mode(MODE_IDLE);
    leds_set_for_mode(get_mode());

    XTime t_last_print = 0;
    XTime t_now;

    while (1) {

        /* Step 4: UART commands must be processed continuously */
        uart_cmd_process();

        /* Step 7: run model and print u3 periodically (0.1–1s) */
        if (get_mode() == MODE_MODULATING) {

            float uin = params_get_ref();
            float u3  = model_step(uin);


            XTime_GetTime(&t_now);

            /* print every 1 second */
            if (t_last_print == 0) t_last_print = t_now;

            if ((t_now - t_last_print) > (5 * COUNTS_PER_SECOND)) {
                t_last_print = t_now;

                int u3_m  = (int)(u3 * 1000000.0f);
                int in_m  = (int)(uin * 1000.0f);

                xil_printf("MODEL: uin=%d.%03d u3=%d.%06d\r\n",
                    in_m/1000, in_m%1000,
                    u3_m/1000000, u3_m%1000000);
            }
        } else {
            /* reset timer when not modulating so it doesn't "burst" on re-enter */
            t_last_print = 0;
        }


        if (lock_get_owner() == LOCK_OWNER_UART) {
            /* UART owns CONFIG lock => buttons disabled */
            continue;
        }

        /* Step 3: Button events */
        uint32_t ev = buttons_get_events();
        if (ev == 0) {
            /* small delay to reduce CPU usage */
            for (volatile int i = 0; i < 200000; i++) { }
            continue;
        }

        system_mode_t mode = get_mode();

        /* BTN0: change mode */
        if (ev & EV_BTN0) {
            if (mode == MODE_IDLE)        mode = MODE_CONFIG;
            else if (mode == MODE_CONFIG) mode = MODE_MODULATING;
            else                           mode = MODE_IDLE;

            set_mode(mode);
            leds_set_for_mode(mode);

            if (mode == MODE_CONFIG)      print_config_status();
            else if (mode == MODE_MODULATING) print_ref();
        }

        /* CONFIG mode buttons */
        if (mode == MODE_CONFIG) {

            if (ev & EV_BTN1) {
                params_toggle_selected();
                xil_printf("Selected: %s\r\n",
                           (params_get_selected() == PARAM_KP) ? "Kp" : "Ki");
                print_config_status();
            }

            if (ev & EV_BTN2) {
                params_dec_selected();
                print_config_status();
            }

            if (ev & EV_BTN3) {
                params_inc_selected();
                print_config_status();
            }
        }

        /* MODULATING mode buttons */
        if (mode == MODE_MODULATING) {

            if (ev & EV_BTN2) {
                params_dec_ref();
                print_ref();
            }

            if (ev & EV_BTN3) {
                params_inc_ref();
                print_ref();
            }
        }

        /* debounce */
        for (volatile int i = 0; i < 800000; i++) { }
    }

    cleanup_platform();
    return 0;
}
