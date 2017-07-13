
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

int main(void)
{
	uint32_t timer = TIM2;
	uint8_t chan = TIM_OC1;
	uint32_t period = 1000;


	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_TIM2);

	//timer_reset(TIM2);
	timer_set_mode(timer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_CENTER_1,
		       TIM_CR1_DIR_UP);
	timer_set_mode(timer, TIM_CR1_CKD_CK_INT, // clock division
	                      TIM_CR1_CMS_EDGE,   // Center-aligned mode selection
	                      TIM_CR1_DIR_UP);    // TIMx_CR1 DIR: Direction

	timer_continuous_mode(timer);             // Disables TIM_CR1_OPM (One pulse mode)
	  timer_set_period(timer, period);                    // Sets TIMx_ARR
	  timer_set_prescaler(timer, 256);               // Adjusts speed of timer
	  timer_set_clock_division(timer, 0);            // Adjusts speed of timer
	  timer_set_master_mode(timer, TIM_CR2_MMS_UPDATE);   // Master Mode Selection
	  timer_enable_preload(timer);                        // Set ARPE bit in TIMx_CR1

	  // Channel-specific settings
	  timer_set_oc_value(timer, chan, 0);             // sets TIMx_CCRx
	  timer_set_oc_mode(timer, chan, TIM_OCM_PWM1);   // Sets PWM Mode 1
	  timer_enable_oc_preload(timer, chan);           // Sets OCxPE in TIMx_CCMRx
	  timer_set_oc_polarity_high(timer, chan);        // set desired polarity in TIMx_CCER
	timer_enable_oc_output(timer, chan); // set CCxE bit in TIMx_CCER  (enable output)

	timer_set_oc_value(TIM2, TIM_OC1, 0x8000);
	timer_set_period(TIM2, 0xffff);

	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO5);
	gpio_set_af(GPIOA, GPIO_AF1, GPIO5);

	timer_generate_event(TIM2, TIM_EGR_UG);

	timer_enable_counter(TIM2);

	while (1);

}
