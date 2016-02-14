
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


#include <libopencmsis/core_cm3.h>

uint16_t frequency_sequence[18] = {
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
	1000,
};

uint16_t frequency_sel = 0;

uint16_t compare_time;
uint16_t new_time;
uint16_t frequency;
int debug = 0;

static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO13 (in GPIO port C) to 'input'. */
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT,
		      GPIO_PUPD_NONE, GPIO13);


	/* Set GPIO6 (in GPIO port A) to 'output push-pull'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
		      GPIO_PUPD_NONE, GPIO6);

	/* Set GPIO7 (in GPIO port A) to 'output push-pull'. */
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
			      GPIO_PUPD_NONE, GPIO7);

    /* Set GPIO9 (in GPIO port A) to 'output push-pull'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
    		      GPIO_PUPD_NONE, GPIO9);

	/* ENABLE NEGATED */
	gpio_clear(GPIOA, GPIO9);

}


static void tim_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral. */
	timer_reset(TIM2);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		       TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	/* Reset prescaler value.
	 * Running the clock at 5kHz.
	 */
	/*
	 * On STM32F4 the timers are not running directly from pure APB1 or
	 * APB2 clock busses.  The APB1 and APB2 clocks used for timers might
	 * be the double of the APB1 and APB2 clocks.  This depends on the
	 * setting in DCKCFGR register. By default the behaviour is the
	 * following: If the Prescaler APBx is greater than 1 the derived timer
	 * APBx clocks will be double of the original APBx frequencies. Only if
	 * the APBx prescaler is set to 1 the derived timer APBx will equal the
	 * original APBx frequencies.
	 *
	 * In our case here the APB1 is devided by 4 system frequency and APB2
	 * divided by 2. This means APB1 timer will be 2 x APB1 and APB2 will
	 * be 2 x APB2. So when we try to calculate the prescaler value we have
	 * to use rcc_apb1_freqency * 2!!!
	 *
	 * For additional information see reference manual for the stm32f4
	 * familiy of chips. Page 204 and 213
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 100000));

	/* Enable preload. */
	timer_disable_preload(TIM2);

	/* Continous mode. */
	timer_continuous_mode(TIM2);

	/* Period (36kHz). */
	timer_set_period(TIM2, 65535);

	/* Disable outputs. */
	timer_disable_oc_output(TIM2, TIM_OC1);
	timer_disable_oc_output(TIM2, TIM_OC2);
	timer_disable_oc_output(TIM2, TIM_OC3);
	timer_disable_oc_output(TIM2, TIM_OC4);

	/* -- OC1 configuration -- */

	/* Configure global mode of line 1. */
	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);
	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_FROZEN);

	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC1, 1000);

	/* ---- */

	/* ARR reload enable. */
	timer_disable_preload(TIM2);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable commutation interrupt. */
	timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}
void tim2_isr(void)
{
	if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {

		/* Clear compare interrupt flag. */
		//timer_clear_flag(TIM2, TIM_SR_CC1IF);

		/*
		 * Get current timer value to calculate next
		 * compare register value.
		 */
		compare_time = timer_get_counter(TIM2);

		/* Calculate and set the next compare value. */
		frequency = frequency_sequence[frequency_sel++];
		new_time = compare_time + frequency;

		timer_set_oc_value(TIM2, TIM_OC1, new_time);
		if (frequency_sel == 18)
			frequency_sel = 0;
int i;

        if(gpio_get(GPIOC, GPIO13))
        {
        	gpio_set(GPIOA, GPIO6);
        }
        else
        {
        	gpio_clear(GPIOA, GPIO6);
        }

		for (i = 0; i < 25; i++) /* Wait a bit. */
			__asm__("nop");

		/* Manually: */
		gpio_set(GPIOA, GPIO7); /* LED on */
		for (i = 0; i < 25; i++) /* Wait a bit. */
			__asm__("nop");
		gpio_clear(GPIOA, GPIO7);/* LED off */
		//for (i = 0; i < 1000000; i++)	/* Wait a bit. */
		//	__asm__("nop");

		timer_clear_flag(TIM2, TIM_SR_CC1IF);

	}
}

int main(void)
{
	gpio_setup();
	tim_setup();

	while (1);

}
