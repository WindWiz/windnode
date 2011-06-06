/* Copyright (C) 2010-2011 Magnus Olsson
 * 
 * This file is part of Windnode
 * Windnode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Windnode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Windnode.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "stackmon.h"

/* NOTE! This stack monitor assumes no memory is allocated using malloc(), and
 * thus all dynamic memory (from heap start to RAMEND) is used for stack.
 */

extern uint8_t __heap_start;
extern uint8_t __stack;

void stackmon_paint(void) __attribute__ ((naked))
	__attribute__ ((section (".init1")));

void stackmon_paint(void)
{
	__asm volatile ("    ldi r30,lo8(__heap_start)\n"
                    "    ldi r31,hi8(__heap_start)\n"
                    "    ldi r24,lo8(0xAC)\n"
                    "    ldi r25,hi8(__stack)\n"
                    "    rjmp .cmp\n"
                    ".loop:\n"
                    "    st Z+,r24\n"
                    ".cmp:\n"
                    "    cpi r30,lo8(__stack)\n"
                    "    cpc r31,r25\n"
                    "    brlo .loop\n"
                    "    breq .loop"::);
}

static uint16_t stackmon_minfree(void)
{
    const uint8_t *p = &__heap_start;
    uint16_t minfree = 0;

    while(*p == 0xAC && p <= &__stack) {
        p++;
        minfree++;
    }

    return minfree;
}

uint16_t stackmon_stacksize(void)
{
	return &__stack - &__heap_start;
}

uint16_t stackmon_maxdepth(void)
{
	return stackmon_stacksize() - stackmon_minfree();
}
