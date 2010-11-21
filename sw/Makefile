CC=avr-gcc
OBJDUMP=avr-objdump
OBJCOPY=avr-objcopy
STRIP=avr-strip

PROGTYPE=stk500v2
PROGPORT=/dev/ttyUSB0
AVRDUDE=avrdude -p $(MCU) -c $(PROGTYPE) -P $(PROGPORT)
ELFFUSE=./elf-fuse

PROG=windwiz
IHEX=windwiz.ihex
OBJS=main.o hwparam.o uart.o adc.o wind.o eeprom_cfg.o swuart.o
FUSES=$(PROG).fuses

MCU=atmega168

CFLAGS  += -Wall -pedantic -Os -g -std=c99 -mmcu=$(MCU)
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
	rm -f $(OBJS) $(PROG) $(IHEX) $(FUSES)

include $(FUSES)
