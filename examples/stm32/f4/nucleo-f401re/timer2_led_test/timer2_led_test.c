
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
//#include <libopencm3/cm3/nvic.h>

//#include <libopencmsis/core_cm3.h>

int main(void)
{
	rcc_periph_clock_enable(RCC_TIM2);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP,
				GPIO_OSPEED_50MHZ, GPIO5);

	timer_reset(TIM2);
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT_MUL_2,
			   TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_TOGGLE);
	timer_enable_oc_output(TIM2, TIM_OC1);
	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC1, 100);
	timer_set_period(TIM2, 10000000);
	timer_enable_counter(TIM2);

	/* Enable GPIOA clock. */
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO5 (in GPIO port A) to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5);

	gpio_set_af(GPIOA, 0x1, GPIO5);

	while (1);

}
