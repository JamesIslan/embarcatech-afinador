#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#define THINGSPEAL_HOST "api.thingspeak.com"
#define THINGSPEAK_IP "3.230.136.118" // api.thingspeak.com
#define THINGSPEAK_PORT 80
#define THINGSPEAK_INTERVALO 15000 // Valor mínimo: 15000

extern void configurar_wifi();
extern void configurar_thingspeak();