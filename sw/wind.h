#include <stdint.h>

void wind_init(void);
void wind_power_up(void);
void wind_power_down(void);
uint16_t wind_direction_sample(void);
void wind_speed_start(void);
uint16_t wind_speed_sample(void);
uint16_t wind_speed_stop(void);

