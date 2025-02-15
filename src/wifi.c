#include "wifi.h"
#include "display.h"
#include "inc/ssd1306.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "src/env.h"
#include <stdio.h>
#include <string.h>

// static struct tcp_pcb *tcp_client_pcb;
char request[256];
static struct tcp_pcb *tcp_client_pcb;

int configurar_wifi() {
  cyw43_arch_enable_sta_mode();

  printf("Conectando ao Wi-Fi...\n");
  uint8_t status_conexao = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_SENHA, CYW43_AUTH_WPA2_AES_PSK, 30000);

  if (status_conexao) {
    printf("Falha ao conectar.\n");
    return 1;
  } else {
    printf("Conectado!\n");
    printf("%s\n", WIFI_SSID);
    printf("%s\n", WIFI_SENHA);
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("Endereço de IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    return 0;
  }
}

// Callback quando dados são recebidos
err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  if (p == NULL) {
    printf("Conexão encerrada pelo servidor.\n");
    tcp_close(tpcb);
    return ERR_OK;
  }

  char buffer[512]; // Buffer maior para evitar overflow
  size_t copy_len = (p->len < sizeof(buffer) - 1) ? p->len : sizeof(buffer) - 1;
  pbuf_copy_partial(p, buffer, copy_len, 0);
  buffer[copy_len] = '\0';
  // printf("Recebido: %s\n", buffer);

  pbuf_free(p);
  return ERR_OK;
}

err_t enviar_dados(void *arg, struct tcp_pcb *tpcb, err_t err) {
  if (err != ERR_OK) {
    printf("Falha ao conectar ao servidor.\n");
    return err;
  }

  printf("Conectado ao servidor.\n");
  snprintf(request, sizeof(request),
           "GET /update?api_key=%s&field1=%d HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Connection: close\r\n\r\n",
           THINGSPEAK_TOKEN, 300, THINGSPEAL_HOST);

  tcp_write(tcp_client_pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
  tcp_output(tcp_client_pcb);
  tcp_recv(tcp_client_pcb, tcp_recv_callback);

  return ERR_OK;
}

// Callback de resolução DNS
void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
  if (ipaddr == NULL) {
    printf("Falha ao resolver DNS para %s\n", name);
    return;
  }

  printf("Resolvido %s para %s\n", name, ipaddr_ntoa(ipaddr));
  tcp_connect(tcp_client_pcb, ipaddr, 80, enviar_dados);
}

// Função para enviar dados ao ThingSpeak
void enviar_para_thingspeak() {
  tcp_client_pcb = tcp_new();
  if (!tcp_client_pcb) {
    printf("Falha ao criar o TCP PCB.\n");
    return;
  }

  printf("Resolvendo %s...\n", "api.thingspeak.com");
  ip_addr_t server_ip;
  ipaddr_aton(THINGSPEAK_IP, &server_ip);
  if (true) {
    printf("DNS resolvido imediatamente: %s\n", ipaddr_ntoa(&server_ip));
    tcp_connect(tcp_client_pcb, &server_ip, 80, enviar_dados);
  } else {
    tcp_close(tcp_client_pcb);
  }
}