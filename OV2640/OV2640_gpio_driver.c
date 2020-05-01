#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "boards.h"

#include "sccb.h"
#include "OV2640_gpio_driver.h"

uint8_t href_loop;
uint32_t pixcnt;
uint16_t linecnt;

uint8_t OV2640_frame_handler(nrf_drv_gpiote_pin_t pin)
{
	uint8_t capture_flag = 0;

	if(pin == OV_VSYNC)
    {
        if(((NRF_P0->IN) & (1 << 2)))
        {
            pixcnt = 0;
			linecnt = 0;
			href_loop = 1;
            nrf_gpio_pin_clear(BSP_LED_2);
        }
        else
        {
			if(!nrf_gpio_pin_read(BSP_BUTTON_2))
			{
				capture_flag = 1;
            }
            href_loop = 0;
//          NRF_LOG_INFO("%d, %d", pixcnt, linecnt);
			// LED blinking.
			nrf_gpio_pin_set(BSP_LED_2);
        }
    }
    return capture_flag;
}

void OV2640_gpio_init(nrfx_gpiote_evt_handler_t evt_handler)
{
    ret_code_t err_code;

	nrf_gpio_range_cfg_input(OV_DATA0, OV_DATA7, GPIO_PIN_CNF_PULL_Pullup);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    err_code = nrf_drv_gpiote_out_init(OV_PWDN_PIN, &out_config);
    APP_ERROR_CHECK(err_code);	
    nrf_drv_gpiote_out_set(OV_PWDN_PIN);

    err_code = nrf_drv_gpiote_out_init(OV_RESET_PIN, &out_config);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_out_set(OV_RESET_PIN);
		
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(OV_PCLK, &in_config, evt_handler);
    APP_ERROR_CHECK(err_code);

    in_config.sense = GPIOTE_CONFIG_POLARITY_LoToHi;
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    err_code = nrf_drv_gpiote_in_init(OV_HREF, &in_config, evt_handler);
    APP_ERROR_CHECK(err_code);

	in_config.sense = NRF_GPIOTE_POLARITY_TOGGLE;
    err_code = nrf_drv_gpiote_in_init(OV_VSYNC, &in_config, evt_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(OV_VSYNC, true);
}

void OV2640_reset(void)
{
	nrf_drv_gpiote_out_clear(OV_RESET_PIN);
    nrf_delay_ms(10);
    nrf_drv_gpiote_out_set(OV_RESET_PIN);
    sccb_init();
}

void OV2640_power(bool on)
{
	if(on == true)
	{
		nrf_drv_gpiote_out_clear(OV_PWDN_PIN);
	}
    else
    {
		nrf_drv_gpiote_out_set(OV_PWDN_PIN);
    }
    nrf_delay_ms(10);
}

void OV2640_VSYNC_int_enable(void)
{
	nrf_drv_gpiote_in_event_enable(OV_VSYNC, true);
}

void OV2640_VSYNC_int_disable(void)
{
	nrf_drv_gpiote_in_event_disable(OV_VSYNC);
}

void OV2640_write_register(uint8_t reg, uint8_t data)
{
	sccb_write(OV2640_SCCB_ID, reg, data);
}

uint8_t OV2640_read_register(uint8_t reg)
{
	uint8_t ret;

	ret = sccb_read(OV2640_SCCB_ID, reg);

    return ret;
}
