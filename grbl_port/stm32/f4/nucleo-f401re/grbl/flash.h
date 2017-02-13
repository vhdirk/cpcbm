

#define MAIN_SECTOR 6
#define COPY_SECTOR 7

unsigned char flash_get_char( unsigned int addr );
void flash_put_char( unsigned int addr, unsigned char new_value);
unsigned int flash_verify_erase_need(char * destination, char *source, unsigned int size);
void memcpy_to_flash_with_checksum(unsigned int destination, char *source, unsigned int size);
void update_main_sector_status(uint32_t updated_status);
void delete_main_sector(void);
void delete_copy_sector(void);
void copy_main_from_copy(uint32_t start_address_offset, uint32_t end_address_offset);
void restore_main_sector(void);
