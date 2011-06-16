#include <stdint.h>

static inline void led(uint8_t en)
{
	CONFIG_LED_DIR |= _BV(CONFIG_LED_PIN);
	if (en)
		CONFIG_LED_PORT |= _BV(CONFIG_LED_PIN);
	else
		CONFIG_LED_PORT &= ~(_BV(CONFIG_LED_PIN));
}

