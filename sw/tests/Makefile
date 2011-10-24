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

# TODO: Add proper dependency checking for h-files
# TODO: Output objs to dedicated build-dir or it gets mixed with AVR objs
# TODO: Generalize Make-targets 

WINDNODE=..

CFLAGS += -Wall -g3 -pedantic -std=c99 -I $(WINDNODE) -include config.h

.PHONY: all
all: TEST_list TEST_cmdline TEST_gsm

timers_OBJS=timers.o $(WINDNODE)/timer_queue.o
timers: $(timers_OBJS)
TEST_timers: timers
	./timers

gsm_OBJS=gsm.o time.o $(WINDNODE)/at.o $(WINDNODE)/gsm.o
gsm: $(gsm_OBJS)
TEST_gsm: gsm
	./gsm

list_OBJS=list.o
list: $(list_OBJS)
TEST_list: list
	./list

cmdline_OBJS=cmdline.o $(WINDNODE)/command.o
cmdline: $(cmdline_OBJS)
TEST_cmdline: cmdline
	./cmdline	

clean:
	rm -rf $(cmdline_OBJS) cmdline $(list_OBJS) list $(timers_OBJS) timers $(gsm_OBJS) gsm
