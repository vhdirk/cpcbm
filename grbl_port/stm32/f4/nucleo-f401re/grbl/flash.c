
#include <libopencm3/stm32/flash.h>
#include <grbl.h>


//flash_unlock();
//flash_lock();

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
	unsigned char value = *((unsigned char*)addr);
	flash_wait_for_last_operation(); // Wait for completion of previous write.
	/* Temporarily do not manage read protection RDP */
	return value; // Return the byte read from EEPROM.
}


unsigned int flash_verify_erase_need(char * destination, char *source, unsigned int size)
{
	char new_value; // New EFLASH value.
	char old_value; // Old EFLASH value.
	char diff_mask; // Difference mask, i.e. old value XOR new value.

	for(unsigned int i = 0; i < size; i++)
	{
		new_value = *(source+i); // new EFLASH value.
		old_value = *(destination+i); // Get old EFLASH value.
		diff_mask = old_value ^ new_value; // Get bit differences.

		// Check if any bits are changed to '1' in the new value.
		if( diff_mask & new_value )
		{
			return ((unsigned int)1);
		}
	}
	return ((unsigned int)0);

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
void flash_put_char( unsigned int addr, unsigned char new_value)
{
	__disable_irq(); // Ensure atomic operation for the write operation.
	flash_unlock();

	flash_program_byte((uint32_t) addr, (uint8_t)new_value);
	
	flash_lock();
	__enable_irq(); // Restore interrupt flag state.
}


void memcpy_to_flash_with_checksum(unsigned int destination, char *source, unsigned int size)
{
	unsigned char checksum = 0;
	for(; size > 0; size--)
	{
		checksum = (checksum << 1) || (checksum >> 7);
		checksum += *source;
		flash_put_char(destination++, *(source++));
	}
	flash_put_char(destination, checksum);
}


void update_main_sector_status(uint32_t updated_status)
{
	flash_unlock();
	flash_program_word(((uint32_t)EFLASH_MAIN_SECTOR_STATUS), updated_status);
	flash_lock();
}

void delete_main_sector(void)
{
	flash_unlock();
	flash_erase_sector(((uint8_t)MAIN_SECTOR), ((uint32_t)0));
	flash_lock();
}

void delete_copy_sector(void)
{
	flash_unlock();
	flash_erase_sector(((uint8_t)COPY_SECTOR), ((uint32_t)0));
	flash_lock();
}

void copy_main_from_copy(uint32_t start_address_offset, uint32_t end_address_offset)
{
	uint32_t * address = (uint32_t*)(start_address_offset + EFLASH_MAIN_BASE_ADDRESS);
	uint32_t value;

	flash_unlock();

	for(uint32_t i = 0; (start_address_offset+i) < end_address_offset; i++)
	{
		value = *(address+i); // new EFLASH value.
		flash_program_word((start_address_offset+i+EFLASH_COPY_BASE_ADDRESS), value);
	}

	flash_lock();
}

void restore_main_sector()
{
	uint32_t * address = ((uint32_t*)EFLASH_ADDR_GLOBAL_COPY);
	uint32_t value;

	flash_unlock();
	for(uint32_t i = 0; (EFLASH_ADDR_GLOBAL_COPY+i) < EFLASH_MAIN_SECTOR_STATUS; i++)
	{
		value = *(address+i); // new EFLASH value.
		flash_program_word((EFLASH_ADDR_GLOBAL_COPY+i+EFLASH_MAIN_BASE_ADDRESS), value);
	}
	flash_lock();
}


