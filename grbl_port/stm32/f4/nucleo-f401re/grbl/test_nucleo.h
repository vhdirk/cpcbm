/*
 * test_nucleo.h
 *
 *  Created on: Dec 8, 2016
 *      Author: ngiule
 */

#ifndef _TEST_NUCLEO_H_
#define _TEST_NUCLEO_H_

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void test_initialization(void);

void test_interrupt_signalling(uint32_t num_signals);

#endif /* _TEST_NUCLEO_H_ */
