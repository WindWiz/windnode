CC=avr-gcc
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
STRIP=avr-strip

PROGTYPE=stk500v2
PROGPORT=/dev/avrusb0
AVRDUDE=avrdude -p $(MCU) -c $(PROGTYPE) -P $(PROGPORT)
ELFFUSE=./elf-fuse

PROG=windnode
IHEX=windnode.ihex
SRCS=main.c hwparam.c uart.c adc.c wind.c eeprom_cfg.c swuart.c ringbuf.c
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

# Extract fuse bytes from ELF binary (firmware needs to use the AVR libc FUSE API)
%.fuses: $(PROG)
	$(ELFFUSE) $* > $@

%.d: %.c
	$(CC) $(CFLAGS) -MM -o $@ $^

# Flash firmware
.PHONY: flash	
flash: $(IHEX)
	$(AVRDUDE) -e -U flash:w:$(IHEX)

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
