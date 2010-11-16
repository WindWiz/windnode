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
} __attribute__((__packed__)); 

#endif
