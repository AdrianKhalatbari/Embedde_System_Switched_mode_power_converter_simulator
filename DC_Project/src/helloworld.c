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

    pi_t pi;
    /* --- Step 9 simple scheduler timers --- */
    static XTime t_btn   = 0;
    static XTime t_ctrl  = 0;
    static XTime t_print = 0;
    pi_init(&pi, params_get_kp(), params_get_ki(), 0.0f, 1.0f);

    uart_cmd_init();

    model_reset();

    /* Fake measurement starts at 0, reference is 1 */
    //float ref = 1.0f;
    //float meas = 0.0f;

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

    // XTime t_last_print = 0;
    // XTime t_now;


    while (1) {

        XTime now;
        XTime_GetTime(&now);

        if (t_btn == 0)   t_btn   = now;
        if (t_ctrl == 0)  t_ctrl  = now;
        if (t_print == 0) t_print = now;

        /* Always process UART (non-blocking) */
        uart_cmd_process();

        /* ---------------- Buttons task (every 10 ms) ---------------- */
        if ((now - t_btn) >= (COUNTS_PER_SECOND / 100)) {
            t_btn = now;

            /* If UART owns CONFIG lock => ignore buttons */
            if (lock_get_owner() != LOCK_OWNER_UART) {

                uint32_t ev = buttons_get_events();
                if (ev) {

                    system_mode_t mode = get_mode();

                    /* BTN0: mode cycle */
                    if (ev & EV_BTN0) {
                        if (mode == MODE_IDLE)        mode = MODE_CONFIG;
                        else if (mode == MODE_CONFIG) mode = MODE_MODULATING;
                        else                          mode = MODE_IDLE;

                        set_mode(mode);
                        leds_set_for_mode(mode);

                        /* Optional: reset when entering MODULATING */
                        if (mode == MODE_MODULATING) {
                            pi.integ = 0.0f;
                            model_reset();
                        }

                        if (mode == MODE_CONFIG)           print_config_status();
                        else if (mode == MODE_MODULATING)  print_ref();
                    }

                    /* CONFIG buttons */
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

                    /* MODULATING buttons */
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
                }
            }
        }

        /* ---------------- Control + Model task (every 10 ms) ---------------- */
        if (get_mode() == MODE_MODULATING) {

            if ((now - t_ctrl) >= (COUNTS_PER_SECOND / 100)) {
                t_ctrl = now;

                float meas = model_get_u3();

                /* clamp measurement (anti-blowup) */
                if (meas > 5.0f) meas = 5.0f;
                if (meas < 0.0f) meas = 0.0f;

                float ref = params_get_ref();

                /* live gains */
                pi.kp = params_get_kp();
                pi.ki = params_get_ki();

                float u  = pi_step(&pi, ref, meas);
                float u3 = model_step(u);

                /* ---------------- Print task (every 5 s) ---------------- */
                if ((now - t_print) >= (5 * COUNTS_PER_SECOND)) {
                    t_print = now;

                    int ref_m = (int)(ref * 1000.0f);
                    int u_m   = (int)(u   * 1000.0f);

                    int u3_m   = (int)(u3 * 1000.0f);
                    int u3_abs = (u3_m < 0) ? -u3_m : u3_m;

                    xil_printf("CL: ref=%d.%03d u=%d.%03d u3=%s%d.%03d\r\n",
                               ref_m/1000, ref_m%1000,
                               u_m/1000,   u_m%1000,
                               (u3_m < 0) ? "-" : "",
                               u3_abs/1000, u3_abs%1000);
                }
            }
        } else {
            /* Not modulating: prevent windup, and reset print timer so it doesn't "burst" */
            pi.integ = 0.0f;
            t_print = now;
        }
    }

    cleanup_platform();
    return 0;
}
