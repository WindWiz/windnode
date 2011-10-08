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

#ifndef CFG_H
#define CFG_H

#include <stdint.h>

#define MAX_APN_LEN (30)
#define MAX_ID_LEN (30)
#define MAX_PASSWORD_LEN (30)
#define MAX_HOST_LEN (30)

struct cfg {
	char apn[MAX_APN_LEN];
	char station_id[MAX_ID_LEN];
	char password[MAX_PASSWORD_LEN];
	uint16_t server_port;
	char server_host[MAX_HOST_LEN];
	uint8_t sample_interval;
	uint8_t samples_per_report;
} __attribute__ ((__packed__));

#endif
