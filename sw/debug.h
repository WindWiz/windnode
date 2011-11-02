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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
#define debug(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)
#define debug_P(fmt, ...) \
	printf_P(fmt, ##__VA_ARGS__)
#else
static inline int no_printf(const char *fmt, ...)
{
	return 0;
}

#define debug(fmt, ...) \
	no_printf(fmt, ##__VA_ARGS__)
#define debug_P(fmt, ...) \
	no_printf(fmt, ##__VA_ARGS__)
#endif

#endif				/* _DEBUG_H_ */
