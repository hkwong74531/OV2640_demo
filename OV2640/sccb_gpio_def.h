#ifndef SCCB_GPIO_DEF_H
#define SCCB_GPIO_DEF_H

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"

#define SIO_C_pin	11
#define SIO_D_pin	12

#define delay_us  nrf_delay_us

void sccb_gpio_init(void);
void sccb_gpio_sot(void);
void sccb_gpio_eot(void);
void sccb_gpio_write_bit(bool bit);
uint8_t sccb_gpio_read_bit(void);
void sccb_gpio_dc(void);
void sccb_gpio_na(void);

#endif
