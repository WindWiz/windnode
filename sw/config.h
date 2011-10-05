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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define F_CPU (16000000UL)
#define CONFIG_DEBUG_BAUDRATE (2400)
#define CONFIG_GSM_BAUDRATE (38400)

/* System timer granularity (1, 10, 100 or 1000) */
#define CONFIG_HZ	(10)

/* Debug LED */
#define CONFIG_LED_PORT (PORTB)
#define CONFIG_LED_DIR  (DDRB)
#define CONFIG_LED_PIN  (PB5)

/* Commandline interface */
#define CONFIG_CMDLINE_SEPARATOR ";"

/* DTMF ENABLE*/
#define CONFIG_DTMF_EN_PIN     (PD2)
#define CONFIG_DTMF_EN_DIR	   (DDRD)
#define CONFIG_DTMF_EN_PORT	   (PORTD)

/* GSM ON/OFF */
#define CONFIG_GSM_EN_PIN      (PD5)
#define CONFIG_GSM_EN_DIR      (DDRD)
#define CONFIG_GSM_EN_PORT     (PORTD)

/* GSM RESET */
#define CONFIG_GSM_RST_PIN     (PD4)
#define CONFIG_GSM_RST_DIR     (DDRD)
#define CONFIG_GSM_RST_PORT    (PORTD)

/* GSM UART */
#define CONFIG_GSM_RX_BUF (20)
#define CONFIG_GSM_TX_BUF (20)

/* Debug port buffers */
#define CONFIG_DEBUG_RX_BUF (20)
#define CONFIG_DEBUG_TX_BUF (20)

/* Software UART */
#define CONFIG_SWUART_RX_BIT	(PB3)
#define CONFIG_SWUART_RX_PORT	(PORTB)
#define CONFIG_SWUART_RX_PIN	(PINB)
#define CONFIG_SWUART_RX_DIR	(DDRB)
#define CONFIG_SWUART_RX_PCBIT	(PCINT3)	/* PCINTx of your RX_BIT pin */
#define CONFIG_SWUART_RX_PCVECT	(PCINT0_vect) 
#define CONFIG_SWUART_RX_PCMSK	(PCMSK0)	
#define CONFIG_SWUART_RX_PCCTRL	(PCIE0)

#define CONFIG_SWUART_TX_BIT	(PB4)
#define CONFIG_SWUART_TX_PORT	(PORTB)
#define CONFIG_SWUART_TX_DIR	(DDRB)

#endif
