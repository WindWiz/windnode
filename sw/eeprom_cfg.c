/* Store/load cfg to/from EEPROM memory 
 * 
 * EEPROM layout (little endian):
 * uint16 <magic>  Magic (must always be CFG_MAGIC)
 * uint16 <len>	   Size of config data (should be sizeof(struct cfg))
 * uint16 <chksum> CRC16 checksum of config data
 * <len> bytes	   Config data
 *
 */

#include <util/crc16.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "eeprom_cfg.h"
#include "cfg.h"

void eeprom_write_config(struct cfg *c)
{
	uint16_t addr = 0;	
	uint16_t i;
	uint16_t cfg_size = sizeof(*c);
	uint16_t magic = CFG_MAGIC;
	uint16_t csum = 0xffff;
	
	eeprom_update_block(&magic, (uint16_t *) addr, sizeof(magic));
	addr += sizeof(magic);

	eeprom_update_block(&cfg_size, (uint16_t *) addr, sizeof(cfg_size));
	addr += sizeof(cfg_size);

	for(i = 0; i < cfg_size; i++) 
		csum = _crc16_update(csum, ((char *) c)[i]);

	eeprom_update_block(&csum, (uint16_t *) addr, sizeof(csum));
	addr += sizeof(csum); 

	eeprom_update_block(c, (uint16_t *) addr, cfg_size);
}

uint8_t eeprom_read_config(struct cfg *c)
{
	uint16_t addr = 0;
	uint16_t i;
	uint16_t cfg_size;
	uint16_t magic;
	uint16_t csum, calc_csum = 0xffff;
	
	eeprom_read_block((uint16_t *) addr, &magic, sizeof(magic));
	addr += sizeof(magic);	
	if (magic != CFG_MAGIC)
		return 1;

	eeprom_read_block((uint16_t *) addr, &cfg_size, sizeof(cfg_size));
	addr += sizeof(cfg_size);
	if (cfg_size != sizeof(struct cfg))
		return 1;

	/* XXX Check cfg_size < EEPROM flash size */

	eeprom_read_block((uint16_t *) addr, &csum, sizeof(csum));
	addr += sizeof(csum);

	eeprom_read_block((uint16_t *) addr, c, cfg_size);

	for(i = 0; i < cfg_size; i++) 
		calc_csum = _crc16_update(calc_csum, ((char *) c)[i]);		
	
	if (calc_csum != csum)
		return 2;

	return 0;
}
