#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#define THINGSPEAL_HOST "api.thingspeak.com"
#define THINGSPEAK_IP "3.230.136.118" // api.thingspeak.com

extern void configurar_wifi();
extern void configurar_thingspeak();