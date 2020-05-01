#ifndef OV2640_GPIO_DRIVER_H
#define OV2640_GPIO_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"
#include "nrf_log.h"

#include "OV2640_gpio_def.h"

#define OV2640_SCCB_ID   		0X60  			//OV2640 ID

#define delay_ms  nrf_delay_ms
#define PRINTF	  NRF_LOG_INFO

#define OV2640_VSYNC_read()	  ((NRF_P0->IN) & (1 << OV_VSYNC))
#define OV2640_HREF_read()	  ((NRF_P0->IN) & (1 << OV_HREF))
#define OV2640_PCLK_read()	  ((NRF_P0->IN) & (1 << OV_PCLK))
#define OV2640_DATA			  ((NRF_P0->IN) >> OV_DATA0)

extern uint8_t href_loop;
extern uint32_t pixcnt;
extern uint16_t linecnt;

uint8_t OV2640_frame_handler(nrf_drv_gpiote_pin_t pin);

void OV2640_gpio_init(nrfx_gpiote_evt_handler_t evt_handler);

void OV2640_reset(void);
void OV2640_power(bool on);

void OV2640_VSYNC_int_enable(void);
void OV2640_VSYNC_int_disable(void);

void OV2640_write_register(uint8_t reg, uint8_t data);
uint8_t OV2640_read_register(uint8_t reg);

#endif