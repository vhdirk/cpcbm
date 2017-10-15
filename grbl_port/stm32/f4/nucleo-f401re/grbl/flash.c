
#include <libopencm3/stm32/flash.h>
#include <grbl.h>

#define FLASH_WAIT_FOR_LAST_OP while((FLASH_SR & FLASH_SR_BSY) == FLASH_SR_BSY);

static void flash_unlock_private(void)
{
	/* Clear the unlock sequence state. */
	FLASH_CR |= FLASH_CR_LOCK;

	/* Authorize the FPEC access. */
	FLASH_KEYR = FLASH_KEYR_KEY1;
	FLASH_KEYR = FLASH_KEYR_KEY2;
};

static void flash_lock_private(void)
{
	FLASH_CR |= FLASH_CR_LOCK;
}

static void flash_program_byte_private(uint32_t address, uint8_t data)
{
	FLASH_WAIT_FOR_LAST_OP
	FLASH_CR &= ~(FLASH_CR_PROGRAM_MASK << FLASH_CR_PROGRAM_SHIFT);
	FLASH_CR |= FLASH_CR_PROGRAM_X8 << FLASH_CR_PROGRAM_SHIFT;

	FLASH_CR |= FLASH_CR_PG;

	MMIO8(address) = data;

	FLASH_WAIT_FOR_LAST_OP

	FLASH_CR &= ~FLASH_CR_PG;		/* Disable the PG bit. */
}

static void flash_program_word_private(uint32_t address, uint32_t data)
{
	/* Ensure that all flash operations are complete. */
	FLASH_WAIT_FOR_LAST_OP
	FLASH_CR &= ~(FLASH_CR_PROGRAM_MASK << FLASH_CR_PROGRAM_SHIFT);
	FLASH_CR |= FLASH_CR_PROGRAM_X32 << FLASH_CR_PROGRAM_SHIFT;

	/* Enable writes to flash. */
	FLASH_CR |= FLASH_CR_PG;

	/* Program the word. */
	MMIO32(address) = data;

	/* Wait for the write to complete. */
	FLASH_WAIT_FOR_LAST_OP

	/* Disable writes to flash. */
	FLASH_CR &= ~FLASH_CR_PG;
}

static void flash_erase_sector_private(uint8_t sector, uint32_t program_size)
{
	FLASH_WAIT_FOR_LAST_OP
	FLASH_CR &= ~(FLASH_CR_PROGRAM_MASK << FLASH_CR_PROGRAM_SHIFT);
	FLASH_CR |= program_size << FLASH_CR_PROGRAM_SHIFT;

	FLASH_CR &= ~(FLASH_CR_SNB_MASK << FLASH_CR_SNB_SHIFT);
	FLASH_CR |= (sector & FLASH_CR_SNB_MASK) << FLASH_CR_SNB_SHIFT;
	FLASH_CR |= FLASH_CR_SER;
	FLASH_CR |= FLASH_CR_STRT;

	FLASH_WAIT_FOR_LAST_OP
	FLASH_CR &= ~FLASH_CR_SER;
	FLASH_CR &= ~(FLASH_CR_SNB_MASK << FLASH_CR_SNB_SHIFT);
}

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
    FLASH_WAIT_FOR_LAST_OP // Wait for completion of previous write.
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
    //__disable_irq(); // Ensure atomic operation for the write operation.
	flash_unlock_private();

    flash_program_byte_private((uint32_t) addr, (uint8_t)new_value);
    
    flash_lock_private();
    //__enable_irq(); // Restore interrupt flag state.
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


int memcpy_from_flash_with_checksum(char *destination, unsigned int source, unsigned int size) {
  unsigned char data, checksum = 0;
  for(; size > 0; size--) {
    data = flash_get_char(source++);
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += data;
    *(destination++) = data;
  }
  return(checksum == flash_get_char(source));
}


void update_main_sector_status(uint32_t updated_status)
{
    flash_unlock_private();
    flash_program_word_private(((uint32_t)EFLASH_MAIN_SECTOR_STATUS), updated_status);
    flash_lock_private();
}

void delete_main_sector(void)
{
    flash_unlock_private();
    flash_erase_sector_private(((uint8_t)MAIN_SECTOR), ((uint32_t)0));
    flash_lock_private();
}

void delete_copy_sector(void)
{
    flash_unlock_private();
    flash_erase_sector_private(((uint8_t)COPY_SECTOR), ((uint32_t)0));
    flash_lock_private();
}

void copy_from_main_to_copy(uint32_t start_address_offset, uint32_t end_address_offset)
{
    uint32_t * address = (uint32_t*)(start_address_offset + EFLASH_MAIN_BASE_ADDRESS);
    uint32_t value;

    flash_unlock_private();

    for(uint32_t i = 0; (start_address_offset+(i<<2)) < end_address_offset; i++)
    {
        value = *(address+i); // new EFLASH value.
        flash_program_word_private((start_address_offset+(i<<2)+EFLASH_COPY_BASE_ADDRESS), value);
    }

    flash_lock_private();
}

void restore_main_sector()
{
    uint32_t * address = ((uint32_t*)EFLASH_COPY_BASE_ADDRESS);
    uint32_t destination = ((uint32_t)EFLASH_MAIN_BASE_ADDRESS);
    uint32_t value;

    flash_unlock_private();
    for(uint32_t i = 0; i < EFLASH_ERASE_AND_RESTORE_OFFSET; i++)
    {
        value = *(address+i); // new EFLASH value.
        flash_program_word_private(((i<<2)+destination), value);
    }
    flash_lock_private();
}


void restore_default_sector_status()
{
	uint32_t* main_sector_status_addr = ((uint32_t*)EFLASH_MAIN_SECTOR_STATUS);

	switch(*main_sector_status_addr)
	{
	case(MAIN_SECTOR_COPIED):
		//delete main-sector and update status is done implicitly
		delete_main_sector();
	case(MAIN_SECTOR_ERASED):
		//copy from copy-sector to main-sector to restore it and update status
		restore_main_sector();
	case(MAIN_SECTOR_RESTORED):
		//delete copy sector and update status
		delete_copy_sector();
		update_main_sector_status(COPY_SECTOR_CLEARED);
		break;
	case(COPY_SECTOR_CLEARED):
		break;
	default:
		//delete main and copy sector, setting restore function will do the rest
		delete_main_sector();
		delete_copy_sector();
		break;
	}
}
