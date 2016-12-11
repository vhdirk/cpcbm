/*
 * test_nucleo.c
 *
 *  Created on: Dec 8, 2016
 *      Author: Angelo Di Chello
 */
#include "test_nucleo.h"

void test_initialization()
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO5 (in GPIO port A) to 'output push-pull'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
}




void test_interrupt_signalling(uint32_t num_signals)
{
    uint32_t j;
    uint32_t i;
    for(j = 0; j < num_signals; j++)
    {
    	gpio_set(GPIOA, GPIO5);	/* LED on/off */
    	for (i = 0; i < 1000000; i++)
    	{	/* Wait a bit. */
    		__asm__("nop");
    	}
    	gpio_clear(GPIOA, GPIO5);	/* LED on/off */
    	for (i = 0; i < 1000000; i++)
    	{	/* Wait a bit. */
    		__asm__("nop");
    	}
    }
}
