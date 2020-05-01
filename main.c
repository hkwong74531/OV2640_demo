/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 * @defgroup pin_change_int_example_main main.c
 * @{
 * @ingroup pin_change_int_example
 * @brief Pin Change Interrupt Example Application main file.
 *
 * This file contains the source code for a sample application using interrupts triggered by GPIO pins.
 *
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "app_error.h"
#include "boards.h"

#include "OV2640_driver.h"

#ifdef BSP_BUTTON_0
    #define PIN_IN BSP_BUTTON_0
#endif
#ifndef PIN_IN
    #error "Please indicate input pin"
#endif

#ifdef BSP_LED_0
//    #define PIN_OUT BSP_LED_0
    #define PIN_OUT BSP_LED_1
#endif
#ifndef PIN_OUT
    #error "Please indicate output pin"
#endif

static volatile uint8_t capture_button_flag;

void in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(OV2640_frame_handler(pin))
    {
		capture_button_flag = 1;
    }

	if(pin == BSP_BUTTON_0)
	{
		nrf_drv_gpiote_out_toggle(PIN_OUT);
	}
	else if(pin == BSP_BUTTON_1)
    {
		nrf_gpio_pin_toggle(BSP_LED_3);
    }
}
/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(nrfx_gpiote_evt_handler_t evt_handler)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(BSP_LED_1, &out_config);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_pin_set(BSP_LED_2);
	nrf_gpio_cfg_output(BSP_LED_2);
    nrf_gpio_pin_set(BSP_LED_0);
	nrf_gpio_cfg_output(BSP_LED_0);

    nrf_gpio_cfg_output(BSP_LED_3);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(BSP_BUTTON_0, &in_config, evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_gpiote_in_init(BSP_BUTTON_1, &in_config, evt_handler);
    APP_ERROR_CHECK(err_code);

	nrf_gpio_cfg_input(BSP_BUTTON_2, GPIO_PIN_CNF_PULL_Pullup);

    nrf_drv_gpiote_in_event_enable(BSP_BUTTON_0, true);
    nrf_drv_gpiote_in_event_enable(BSP_BUTTON_1, true);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
	uint32_t time;

    /* Initializing Log */
	ret_code_t err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_DEFAULT_BACKENDS_INIT();

    gpio_init(in_pin_handler);

	OV2640_gpio_init(in_pin_handler);
    OV2640_Init();
	OV2640_RGB565_Mode();
  	OV2640_OutSize_Set(320, 240); //(160, 120); 
    OV2640_FrameRate_Ctrl();

	capture_button_flag = 0;

    NRF_LOG_INFO("main starts");

    while (true)
    {
		if(capture_button_flag)
        {
			nrf_gpio_pin_clear(BSP_LED_0);

			OV2640_JPG_Data();
			OV2640_RGB565_Mode();
			OV2640_OutSize_Set(320, 240); //(160, 120); 
			OV2640_FrameRate_Ctrl();

			nrf_gpio_pin_set(BSP_LED_0);
            capture_button_flag = 0;
		}
        else
        {
			OV2640_Preview_Loop();
        }
        NRF_LOG_PROCESS();
    }
}


/** @} */
