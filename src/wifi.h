#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#define LED_PIN 12 // Pino do LED
extern void iniciar_servidor_http(void);

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
extern ip_addr_t obter_ip_via_dns(const char *hostname);
// extern void enviar_dados(const char *url);
// extern static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
