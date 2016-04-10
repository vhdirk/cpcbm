
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#include <libopencmsis/core_cm3.h>

uint16_t compare_time;
uint16_t new_time;
uint16_t frequency;
int debug = 0;

static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_84MHZ]);
}

static void gpio_setup(void)
{
	/* Enable GPIOC clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Set GPIO13 (in GPIO port C) to 'input'. */
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT,
		      GPIO_PUPD_NONE, GPIO13);


	/* Set GPIO5 (in GPIO port A) to 'output push-pull'. */
	//gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
	/* Set GPIO5 (in GPIO port A) to 'Alternate function output push-pull'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5);

	gpio_set_af(GPIOA, 0x1, GPIO5);


}

static void tim_1_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM1);

	/* Reset TIM1 peripheral. */
	timer_reset(TIM1);

	/* Continous mode. */
	timer_continuous_mode(TIM1);

	/* Period. */
	timer_set_period(TIM1, 0xFFFF);

	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM1, TIM_OC1, 0x7FFF);

	timer_set_prescaler(TIM2, (rcc_apb2_frequency / 10000));

	/* Timer global mode:
		• Configure Timer 1 master mode to send its Output Compare 1 Reference (OC1REF)
		signal as trigger output (MMS=100 in the TIM1_CR2 register).
		• Configure the Timer 1 OC1REF waveform (TIM1_CCMR1 register).
		• Configure Timer 2 to get the input trigger from Timer 1 (TS=000 in the TIM2_SMCR
		register).
		• Configure Timer 2 in gated mode (SMS=101 in TIM2_SMCR register).
		• Enable Timer 2 by writing ‘1 in the CEN bit (TIM2_CR1 register).
		• Start Timer 1 by writing ‘1 in the CEN bit (TIM1_CR1 register).
	 */
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_master_mode(TIM1, TIM_CR2_MMS_COMPARE_OC1REF);

	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_TOGGLE);

	timer_enable_oc_output(TIM1, TIM_OC1);

	/* Counter enable. */
    timer_enable_counter(TIM1);

}

static void tim_2_setup(void)
{
	/* Enable TIM2 clock. */
	rcc_periph_clock_enable(RCC_TIM2);

	/* Enable TIM2 interrupt. */
	nvic_enable_irq(NVIC_TIM2_IRQ);

	/* Reset TIM2 peripheral. */
	timer_reset(TIM2);

	/* Select the valid trigger input */
	timer_slave_set_trigger(TIM2,TIM_SMCR_TS_ITR0);
	timer_slave_set_mode(TIM2, TIM_SMCR_SMS_GM);

	/* Timer global mode:
	 * - No divider
	 * - Alignment edge
	 * - Direction up
	 */
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

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
	 * In our case here the APB1 is divided by 4 system frequency and APB2
	 * divided by 2. This means APB1 timer will be 2 x APB1 and APB2 will
	 * be 2 x APB2. So when we try to calculate the prescaler value we have
	 * to use rcc_apb1_freqency * 2!!!
	 *
	 * For additional information see reference manual for the stm32f4
	 * family of chips. Page 204 and 213
	 */
	timer_set_prescaler(TIM2, ((rcc_apb1_frequency * 2) / 1000000));

	/* Enable preload. */
	//timer_disable_preload(TIM2);

	/* Continous mode. */
	timer_continuous_mode(TIM2);

	/* Period (36kHz). */
	timer_set_period(TIM2, 1000000);

	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC1, 200000);

	/* Set the capture compare value for OC1. */
	timer_set_oc_value(TIM2, TIM_OC2, 0);

	/* Disable outputs. */
	//timer_disable_oc_output(TIM2, TIM_OC1);
	timer_disable_oc_output(TIM2, TIM_OC2);
	timer_disable_oc_output(TIM2, TIM_OC3);
	timer_disable_oc_output(TIM2, TIM_OC4);

	/* -- OC1 configuration -- */
	/* Configure global mode of line 1. */
	timer_disable_oc_clear(TIM2, TIM_OC1);
	timer_disable_oc_preload(TIM2, TIM_OC1);
	timer_set_oc_slow_mode(TIM2, TIM_OC1);
	timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);

	timer_enable_oc_output(TIM2, TIM_OC1);

	/* ---- */

	/* ARR reload enable. */
	//timer_disable_preload(TIM2);

	/* Counter enable. */
	timer_enable_counter(TIM2);

	/* Enable commutation interrupt. */
	//timer_enable_irq(TIM2, TIM_DIER_CC2IE);
}


int main(void)
{
	clock_setup();
	gpio_setup();
	tim_1_setup();
	tim_2_setup();

	while (1);

}
