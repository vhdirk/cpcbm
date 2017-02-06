

unsigned char flash_get_char( unsigned int addr );
void flash_put_char( unsigned int addr, unsigned char new_value);
unsigned int flash_verify_erase_need(char * destination, char *source, unsigned int size);
void memcpy_to_flash_with_checksum(unsigned int destination, char *source, unsigned int size);
