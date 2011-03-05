#ifndef _GSM_H_

#include <stdint.h>

int gsm_init(void);
void gsm_power_up();
void gsm_power_down();
void gsm_reset();
int gsm_at(char *cmd);
int gsm_read_response(char *buf, size_t buf_len, uint8_t timeout);
int gsm_check_response(char *expected, uint8_t timeout);

#endif

