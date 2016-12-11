/*
  system.c - Handles system level commands and real-time processes
  Part of Grbl

  Copyright (c) 2014-2015 Sungeun K. Jeon  

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "grbl.h"
#ifdef TEST_NUCLEO_EXTI_PINS
#include "test_nucleo.h"
#endif

volatile uint8_t sys_probe_state;   // Probing state value.  Used to coordinate the probing cycle with stepper ISR.
volatile uint8_t sys_rt_exec_state;  // Global realtime executor bitflag variable for state management. See EXEC bitmasks.
volatile uint8_t sys_rt_exec_alarm;  // Global realtime executor bitflag variable for setting various alarms.

void system_init()
{
#ifdef NUCLEO
	/* Enable GPIOA and GPIOB clocks. */
	rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
	SET_CONTROLS_DDR; // Configure as input pins
#ifdef DISABLE_CONTROL_PIN_PULL_UP
	UNSET_CONTROLS_PU;
#else
	SET_CONTROLS_PU;
#endif
	exti_enable_request(CONTROL_INT_vect);
	exti_set_trigger(CONTROL_INT_vect, EXTI_TRIGGER_FALLING);
	//nvic_enable_irq(RESET_CONTROL_INT);// Enable control pin Interrupt
	nvic_enable_irq(FEED_HOLD_CONTROL_INT);// Enable control pin Interrupt
	nvic_enable_irq(CYCLE_START_CONTROL_INT);// Enable control pin Interrupt
	nvic_enable_irq(SAFETY_DOOR_CONTROL_INT);// Enable control pin Interrupt
#ifdef TEST_NUCLEO_EXTI_PINS
    test_initialization();
#endif

#else
	CONTROL_DDR &= ~(CONTROL_MASK); // Configure as input pins
  #ifdef DISABLE_CONTROL_PIN_PULL_UP
    CONTROL_PORT &= ~(CONTROL_MASK); // Normal low operation. Requires external pull-down.
  #else
    CONTROL_PORT |= CONTROL_MASK;   // Enable internal pull-up resistors. Normal high operation.
  #endif
  CONTROL_PCMSK |= CONTROL_MASK;  // Enable specific pins of the Pin Change Interrupt
  PCICR |= (1 << CONTROL_INT);   // Enable Pin Change Interrupt
#endif
}

#ifdef NUCLEO
void exti0_isr()
{
#ifdef TEST_NUCLEO_EXTI_PINS
    test_interrupt_signalling((uint32_t)10);
#endif
}

void exti1_isr()
{
	nvic_clear_pending_irq(NVIC_EXTI1_IRQ);
#ifdef TEST_NUCLEO_EXTI_PINS
    test_interrupt_signalling((uint32_t)1);
#endif
    bit_true(sys_rt_exec_state, EXEC_FEED_HOLD);
}

void exti2_isr()
{
#ifdef TEST_NUCLEO_EXTI_PINS
    test_interrupt_signalling((uint32_t)2);
#endif
	//TODO: uncomment this when function is imported
	//mc_reset();
}

void exti3_isr()
{
#ifdef TEST_NUCLEO_EXTI_PINS
    test_interrupt_signalling((uint32_t)3);
#endif
    bit_true(sys_rt_exec_state, EXEC_SAFETY_DOOR);
}

void exti4_isr()
{
#ifdef TEST_NUCLEO_EXTI_PINS
    test_interrupt_signalling((uint32_t)4);
#endif
	bit_true(sys_rt_exec_state, EXEC_CYCLE_START);
}
#else

// Pin change interrupt for pin-out commands, i.e. cycle start, feed hold, and reset. Sets
// only the realtime command execute variable to have the main program execute these when
// its ready. This works exactly like the character-based realtime commands when picked off
// directly from the incoming serial data stream.
ISR(CONTROL_INT_vect)
{
  uint8_t pin = (CONTROL_PIN & CONTROL_MASK);
  #ifndef INVERT_ALL_CONTROL_PINS
    pin ^= CONTROL_INVERT_MASK;
  #endif
  // Enter only if any CONTROL pin is detected as active.
  if (pin) {
    if (bit_istrue(pin,bit(RESET_BIT))) {
      mc_reset();
    } else if (bit_istrue(pin,bit(CYCLE_START_BIT))) {
      bit_true(sys_rt_exec_state, EXEC_CYCLE_START);
    #ifndef ENABLE_SAFETY_DOOR_INPUT_PIN
      } else if (bit_istrue(pin,bit(FEED_HOLD_BIT))) {
        bit_true(sys_rt_exec_state, EXEC_FEED_HOLD);
    #else
      } else if (bit_istrue(pin,bit(SAFETY_DOOR_BIT))) {
        bit_true(sys_rt_exec_state, EXEC_SAFETY_DOOR);
    #endif
    }
  }
}
#endif
