# Copyright (C) 2010-2011 Magnus Olsson
# 
# This file is part of Windnode
# Windnode is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Windnode is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Windnode.  If not, see <http://www.gnu.org/licenses/>.
# 

CC=avr-gcc
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
STRIP=avr-strip

PROGTYPE=stk500v2
PROGPORT=/dev/avrusb0
AVRDUDE=avrdude -p $(MCU) -c $(PROGTYPE) -P $(PROGPORT)
MAGBOOT=magboot.py $(PROGPORT) $(MCU) -z -i
ELFFUSE=./elf-fuse

PROG=windnode
IHEX=windnode.ihex
BIN=windnode.bin
SRCS=main.c hwparam.c uart.c adc.c wind.c eeprom_cfg.c swuart.c ringbuf.c \
	command.c time.c timer_queue.c
OBJS=$(SRCS:%.c=%.o)
DEPS=$(SRCS:%.c=%.d)
FUSES=$(PROG).fuses

MCU=atmega328p

CFLAGS  += -Wall -pedantic -Os -g -std=c99 -mmcu=$(MCU) -include config.h
LDFLAGS += -mmcu=$(MCU)

all: $(PROG) $(IHEX)

# Link the ELF binary
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Generate Intel Hex image from ELF binary, also remove .fuse section
$(IHEX): $(PROG)
	$(OBJCOPY) -R .fuse -R .eeprom -R .lock -R .signature -S -O ihex $^ $@ 

# Generate raw binaries from ELF, strips all sections except .text and .data
$(BIN): $(PROG)
	$(OBJCOPY) -j .data -j .text -S -O binary $^ $@

# Extract fuse bytes from ELF binary (firmware needs to use the AVR libc FUSE API)
%.fuses: $(PROG)
	$(ELFFUSE) $* > $@

%.d: %.c
	$(CC) $(CFLAGS) -MM -o $@ $^

# Flash firmware
.PHONY: flash	
flash: $(IHEX)
	$(AVRDUDE) -e -U flash:w:$(IHEX)

# Flash firmware using magboot
.PHONY: mflash
mflash: $(BIN)
	$(MAGBOOT) -a 0x0 -w $(BIN) -r

# Downloads current fuse bytes and displays them on stdout
.PHONY: fusedump
fusedump: 
	$(AVRDUDE) -U hfuse:r:-:h -U lfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h

# Updates fuse bytes on target (with values extracted from ELF binary)
# TODO: Can't program efuse right now, avrdude writes fail (?!)
.PHONY: fuseprog
fuseprog: $(FUSES)
	$(AVRDUDE) -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

# Clean up all generated content
.PHONY: clean
clean:
	rm -f $(OBJS) $(PROG) $(IHEX) $(FUSES) $(DEPS)

include $(FUSES)
include $(DEPS)
