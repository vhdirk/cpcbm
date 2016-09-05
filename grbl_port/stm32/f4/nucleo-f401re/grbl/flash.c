
#include <libopencm3/stm32/flash.h>


flash_unlock();
flash_lock();

/*! \brief  Read byte from FLASH.
 *
 *  This function reads one byte from a given FLASH address.
 *
 *  \note  The CPU is halted for 4 clock cycles during FLASH read.
 *
 *  \param  addr  FLASH address to read from.
 *  \return  The byte read from the FLASH address.
 */
unsigned char flash_get_char( unsigned int addr )
{
	uint8_t* value = *addr;
	flash_wait_for_last_operation(); // Wait for completion of previous write.
	/* Temporarily do not manage read protection RDP */
	return value; // Return the byte read from EEPROM.
}


/*! \brief  Write byte to FLASH.
 *
 *  This function writes one byte to a given FLASH address.
 *  The differences between the existing byte and the new value is used
 *  to select the most efficient FLASH programming mode.
 *
 *  \note  The CPU is halted for 2 clock cycles during FLASH programming.
 *
 *  \note  When this function returns, the new FLASH value is not available
 *         until the FLASH programming time has passed. The EEPE bit in EECR
 *         should be polled to check whether the programming is finished.
 *
 *  \note  The EEPROM_GetChar() function checks the EEPE bit automatically.
 *
 *  \param  addr  FLASH address to write to.
 *  \param  new_value  New FLASH value.
 */
void flash_put_char( unsigned int addr, unsigned char new_value )
{
	char old_value; // Old EEPROM value.
	char diff_mask; // Difference mask, i.e. old value XOR new value.

	__disable_irq(); // Ensure atomic operation for the write operation.
	
	do {} while( EECR & (1<<EEPE) ); // Wait for completion of previous write.
	#ifndef EEPROM_IGNORE_SELFPROG
	do {} while( SPMCSR & (1<<SELFPRGEN) ); // Wait for completion of SPM.
	#endif
	
	EEAR = addr; // Set EEPROM address register.
	EECR = (1<<EERE); // Start EEPROM read operation.
	old_value = EEDR; // Get old EEPROM value.
	diff_mask = old_value ^ new_value; // Get bit differences.
	
	// Check if any bits are changed to '1' in the new value.
	if( diff_mask & new_value ) {
		// Now we know that _some_ bits need to be erased to '1'.
		
		// Check if any bits in the new value are '0'.
		if( new_value != 0xff ) {
			// Now we know that some bits need to be programmed to '0' also.
			
			EEDR = new_value; // Set EEPROM data register.
			EECR = (1<<EEMPE) | // Set Master Write Enable bit...
			       (0<<EEPM1) | (0<<EEPM0); // ...and Erase+Write mode.
			EECR |= (1<<EEPE);  // Start Erase+Write operation.
		} else {
			// Now we know that all bits should be erased.

			EECR = (1<<EEMPE) | // Set Master Write Enable bit...
			       (1<<EEPM0);  // ...and Erase-only mode.
			EECR |= (1<<EEPE);  // Start Erase-only operation.
		}
	} else {
		// Now we know that _no_ bits need to be erased to '1'.
		
		// Check if any bits are changed from '1' in the old value.
		if( diff_mask ) {
			// Now we know that _some_ bits need to the programmed to '0'.
			
			EEDR = new_value;   // Set EEPROM data register.
			EECR = (1<<EEMPE) | // Set Master Write Enable bit...
			       (1<<EEPM1);  // ...and Write-only mode.
			EECR |= (1<<EEPE);  // Start Write-only operation.
		}
	}
	
	__enable_irq(); // Restore interrupt flag state.
}

