#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#define LED_PIN 12    // Pino do LED
#define BUTTON1_PIN 5 // Pino do botão 1
#define BUTTON2_PIN 6 // Pino do botão 2

extern void criar_resposta_http();
extern void monitor_buttons();
extern void iniciar_servidor_http(void);

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
extern ip_addr_t obter_ip_via_dns(const char *hostname);
// extern void enviar_dados(const char *url);
// extern static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
