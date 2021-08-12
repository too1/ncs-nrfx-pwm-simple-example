NRFX PWM Simple Example
#######################

Overview
********

This is a simple example showing how to use the nrfx_pwm driver to access the NRF_PWM module in a more direct way compared to using the native Zephyr PWM driver. 
The example configures two channels and implements a function to set the duty cycle for these channels when needed. 
The main loop updates the duty cycle based on a rolling counter. 

Requirements
************

Tested on the following boards:

- nrf52840DK_nrf52840