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

#ifndef _AT_H_
#define _AT_H_

#include <stdio.h>
#include <stdint.h>

int at_verify_response(FILE *stream, char *expected, uint16_t timeout);
int at_cmd(FILE *stream, char *cmd);
int at_response(FILE *stream, char *buf, size_t buf_len, uint16_t timeout);
int at_info(FILE *stream, char *cmd, char *buf, size_t buflen, uint16_t timeout);
int at_simple(FILE *stream, char *cmd, uint16_t timeout);

#endif
