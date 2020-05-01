#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_delay.h"

#include "sccb_gpio_def.h"

void sccb_gpio_init(void)
{
    nrf_gpio_pin_set(SIO_C_pin);
    nrf_gpio_cfg_output(SIO_C_pin);

//	nrf_gpio_cfg_input(SIO_D_pin, GPIO_PIN_CNF_PULL_Disabled);//GPIO_PIN_CNF_PULL_Pullup);
	nrf_gpio_pin_set(SIO_D_pin);
	nrf_gpio_cfg_output(SIO_D_pin);
}

void sccb_gpio_sot(void)
{
	nrf_gpio_pin_set(SIO_D_pin);
	nrf_gpio_pin_set(SIO_C_pin);
	nrf_delay_us(50);
	nrf_gpio_pin_clear(SIO_D_pin);
    nrf_delay_us(50);
    nrf_gpio_pin_clear(SIO_C_pin);
    nrf_delay_us(50);
}

void sccb_gpio_eot(void)
{
    nrf_gpio_pin_clear(SIO_D_pin);
    nrf_gpio_pin_clear(SIO_C_pin);
    nrf_delay_us(50);
    nrf_gpio_pin_set(SIO_C_pin);
    nrf_delay_us(50);
    nrf_gpio_pin_set(SIO_D_pin);
    nrf_delay_us(50);
}

void sccb_gpio_write_bit(bool bit)
{
    nrf_gpio_pin_clear(SIO_C_pin);
    if(bit == 1)
    {
		nrf_gpio_pin_set(SIO_D_pin);
    }
    else
    {
		nrf_gpio_pin_clear(SIO_D_pin);
    }
    nrf_delay_us(50);
    nrf_gpio_pin_set(SIO_C_pin);
    nrf_delay_us(50);
}

uint8_t sccb_gpio_read_bit(void)
{
	uint32_t ret;

    nrf_gpio_cfg_input(SIO_D_pin, GPIO_PIN_CNF_PULL_Pullup);
    nrf_gpio_pin_clear(SIO_C_pin);
    nrf_delay_us(50);
    ret = nrf_gpio_pin_read(SIO_D_pin);
    nrf_gpio_pin_set(SIO_C_pin);
	nrf_delay_us(50);

    return (uint8_t)ret;
}

void sccb_gpio_dc(void)
{
    nrf_gpio_pin_clear(SIO_C_pin);
	nrf_gpio_cfg_input(SIO_D_pin, GPIO_PIN_CNF_PULL_Pullup);
    nrf_delay_us(50);
    nrf_gpio_pin_set(SIO_C_pin);
    nrf_delay_us(50);
    nrf_gpio_cfg_output(SIO_D_pin);
}

void sccb_gpio_na(void)
{
	nrf_gpio_pin_clear(SIO_C_pin);
	nrf_gpio_pin_set(SIO_D_pin);
    nrf_gpio_cfg_output(SIO_D_pin);
    nrf_delay_us(50);
    nrf_gpio_pin_set(SIO_C_pin);
    nrf_delay_us(50);
}

