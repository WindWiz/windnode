#ifndef EEPROM_CFG_H
#define EEPROM_CFG_H

#include <stdint.h>
#include "cfg.h"

#define CFG_MAGIC (0xCAFE)

void eeprom_write_config(struct cfg *c);
uint8_t eeprom_read_config(struct cfg *c);

#endif
