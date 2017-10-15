/*
 * reset_vector.c
 *
 *  Created on: 15 ott 2017
 *      Author: A
 */
#include "grbl.h"

extern int main(void);

/* Symbols exported by the linker script(s): */
extern unsigned _data_loadaddr, _data, _edata, _ebss, _stack, _ramcode, _eramcode, _ramcode_loadaddr;

/* Redefine the reset handler to allow ram code copy */
void reset_handler(void)
{
	volatile unsigned *src, *dest;

	for (src = &_data_loadaddr, dest = &_data;
		dest < &_edata;
		src++, dest++) {
		*dest = *src;
	}

	for (src = &_ramcode_loadaddr, dest = &_ramcode;
			dest < &_eramcode;
			src++, dest++) {
			*dest = *src;
		}

	while (dest < &_ebss) {
		*dest++ = 0;
	}

	/* Disable interrupts to relocate VTOR */
   __disable_irq(); // Global disable interrupts
   /* Relocate vector table */
   SCB->VTOR = (uint32_t)&_ramcode;
   __enable_irq(); // Global enable interrupts

	/* Ensure 8-byte alignment of stack pointer on interrupts */
	/* Enabled by default on most Cortex-M parts, but not M3 r1 */
	SCB_CCR |= SCB_CCR_STKALIGN;

	/* Enable access to Floating-Point coprocessor. */
	SCB_CPACR |= SCB_CPACR_FULL * (SCB_CPACR_CP10 | SCB_CPACR_CP11);

	/* Call the application's entry point. */
	main();
}
