/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <nrfx_pwm.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   50

#define PWM_COUNTERTOP  100

static nrfx_pwm_t my_pwm = NRFX_PWM_INSTANCE(1);

static int pwm_init(void)
{
	static nrfx_pwm_config_t const config0 =
    {
        .output_pins =
        {
            30,						// channel 0
            31,						// channel 1
            NRFX_PWM_PIN_NOT_USED,	// channel 2
            NRFX_PWM_PIN_NOT_USED	// channel 3
        },
        .irq_priority = 5,
        .base_clock   = NRF_PWM_CLK_1MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = PWM_COUNTERTOP,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    return (nrfx_pwm_init(&my_pwm, &config0, NULL, NULL) == NRFX_SUCCESS) ? 0 : -1;

	// If PWM callbacks are to be used, remember to configure the interrupts correctly
	//IRQ_DIRECT_CONNECT(PWM1_IRQn, 0, nrfx_pwm_1_irq_handler, 0);
	//irq_enable(PWM1_IRQn);
}

static void pwm_set_duty_cycle(uint32_t dc1, uint32_t dc2)
{
	static bool pwm_running = false;

    // This array cannot be allocated on stack (hence "static") and it must be in RAM 
    static nrf_pwm_values_individual_t seq_values;

	// Update the respective channels
	seq_values.channel_0 = (dc1 <= PWM_COUNTERTOP) ? dc1 : PWM_COUNTERTOP;
	seq_values.channel_1 = (dc2 <= PWM_COUNTERTOP) ? dc2 : PWM_COUNTERTOP;

    nrf_pwm_sequence_t const seq =
    {
        .values.p_individual = &seq_values,
        .length              = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats             = 0,
        .end_delay           = 0
    };

	if(!pwm_running){
		pwm_running = true;
		(void)nrfx_pwm_simple_playback(&my_pwm, &seq, 1000, NRFX_PWM_FLAG_LOOP);
	}
}


void main(void)
{
	if(pwm_init() != 0){
		printk("Error initializing PWM\n");
		return;
	}

	printk("NRFX PMW example started\n");

	while (1) {
		static int counter = 0;
		pwm_set_duty_cycle(counter, PWM_COUNTERTOP - counter);
		counter = (counter + 1) % PWM_COUNTERTOP;
		k_msleep(SLEEP_TIME_MS);
	}
}
