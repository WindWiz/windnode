#include <stdint.h>

void adc_init();
void adc_disable();
void adc_enable();
uint16_t adc_sample(uint8_t channel);

