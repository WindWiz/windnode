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

#include "time.h"
#include "errno.h"
#include "at.h"
#include "gsm.h"

int gsm_status(FILE * stream)
{
	int gsm_status;
	int len;
	char buf[12];

	len = at_info(stream, "AT+CREG?", buf, sizeof(buf), 1 * HZ);
	if (len < 0)
		return len;

	if (!sscanf(buf, "+CREG: 0,%d", &gsm_status))
		return GSM_UNKNOWN;
	else
		return gsm_status;
}

int gsm_activate_context(FILE * stream)
{
	int len;
	int gprs_active;
	char buf[22];

	len = at_info(stream, "AT#GPRS?", buf, sizeof(buf), 10 * HZ);
	if (len < 0)
		return len;

	if (!sscanf(buf, "#GPRS: %d", &gprs_active))
		return -EFAULT;

	if (gprs_active)
		return 0;	/* already active */

	len = at_info(stream, "AT#GPRS=1", buf, sizeof(buf), 10 * HZ);
	if (len < 0)
		return len;

	return 0;
}
