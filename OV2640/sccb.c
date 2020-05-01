#include "sccb_gpio_def.h"

static void sccb_phase_1(uint8_t id)	// common for both write and read
{
	sccb_gpio_sot();
    for(int8_t i = 7; i >= 1; i--)
    {
		if(id & (1 << i))
		{
			sccb_gpio_write_bit(1);
        }
        else
        {
			sccb_gpio_write_bit(0);
        }
    }
	sccb_gpio_write_bit(0);	// for write
    sccb_gpio_dc();
}

static void sccb_phase_2_write(uint8_t sub)
{
    for(int8_t i = 7; i >= 0; i--)
    {
		if(sub & (1 << i))
		{
			sccb_gpio_write_bit(1);
        }
        else
        {
			sccb_gpio_write_bit(0);
        }
    }
    sccb_gpio_dc();
}

static void sccb_phase_3_write(uint8_t data)
{
    for(int8_t i = 7; i >= 0; i--)
    {
		if(data & (1 << i))
		{
			sccb_gpio_write_bit(1);
        }
        else
        {
			sccb_gpio_write_bit(0);
        }
    }
    sccb_gpio_dc();
    sccb_gpio_eot();
}

static void sccb_phase_2_read(uint8_t sub)
{
    for(int8_t i = 7; i >= 0; i--)
    {
		if(sub & (1 << i))
		{
			sccb_gpio_write_bit(1);
        }
        else
        {
			sccb_gpio_write_bit(0);
        }
    }
    sccb_gpio_dc();
    sccb_gpio_eot();
}

static void sccb_phase_3_read(uint8_t id)
{
	sccb_gpio_sot();
    for(int8_t i = 7; i >= 1; i--)
    {
		if(id & (1 << i))
		{
			sccb_gpio_write_bit(1);
        }
        else
        {
			sccb_gpio_write_bit(0);
        }
    }
	sccb_gpio_write_bit(1);	// for read
    sccb_gpio_dc();
}

static uint8_t sccb_phase_4_read(void)
{
	uint8_t ret = 0;
    for(int8_t i = 7; i >= 0; i--)
    {
		uint8_t bit;

		bit = sccb_gpio_read_bit();
		ret |= (bit << i);
    }
	sccb_gpio_na();
    sccb_gpio_eot();

    return ret;
}

void sccb_init(void)
{
	sccb_gpio_init();
}

void sccb_write(uint8_t id, uint8_t sub, uint8_t data)
{
	sccb_phase_1(id);
    sccb_phase_2_write(sub);
    sccb_phase_3_write(data);
}

uint8_t sccb_read(uint8_t id, uint8_t sub)
{
	uint8_t ret;

	sccb_phase_1(id);
    sccb_phase_2_read(sub);
    sccb_phase_3_read(id);
    ret = sccb_phase_4_read();

    return ret;
}