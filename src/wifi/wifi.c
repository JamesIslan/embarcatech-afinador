#include "wifi.h"
#include "../display/bitmaps.h"
#include "../display/display.h"
#include "../mic/mic.h"
#include "env.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

char request[256];
static struct tcp_pcb *tcp_client_pcb;
struct repeating_timer timer_thingspeak;

// Configuração inicial do módulo wi-fi
void configurar_wifi() {
  cyw43_arch_enable_sta_mode();
  printf("Conectando ao Wi-Fi...\n");

  while (true) {
    uint8_t status_conexao = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_SENHA, CYW43_AUTH_WPA2_AES_PSK, 30000);
    if (status_conexao) {
      printf("Falha ao conectar. Tentando novamente...\n");
      exibir_bitmap_display(menu_conexao_falha);
      sleep_ms(3000);
      continue;
    } else {
      printf("Conectado!\n");
      exibir_bitmap_display(menu_conexao_concluida);
      uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
      printf("Endereço de IP %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
      sleep_ms(3000);
      break;
    }
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

  pbuf_free(p);
  return ERR_OK;
}

// Realiza o envio de dados para a nuvem via HTTP
err_t enviar_dados(void *arg, struct tcp_pcb *tpcb, err_t err) {
  if (err != ERR_OK) {
    printf("Falha ao conectar ao servidor.\n");
    return err;
  }

  printf("Conectado ao servidor.\n");
  float valor_frequencia = max_freq;
  snprintf(request, sizeof(request),
           "GET /update?api_key=%s&field1=%f HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Connection: close\r\n\r\n",
           THINGSPEAK_TOKEN, valor_frequencia, THINGSPEAL_HOST);

  tcp_write(tcp_client_pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
  tcp_output(tcp_client_pcb);
  tcp_recv(tcp_client_pcb, tcp_recv_callback);

  return ERR_OK;
}

// Função callback para enviar dados ao ThingSpeak
// Retorna: true para continuar rodando; false para parar de rodar
bool conectar_thingspeak_callback(struct repeating_timer *t) {
  tcp_client_pcb = tcp_new();
  if (!tcp_client_pcb) {
    printf("Falha ao criar o TCP PCB.\n");
    return false;
  }

  ip_addr_t server_ip;
  ipaddr_aton(THINGSPEAK_IP, &server_ip);
  if (true) {
    printf("DNS resolvido imediatamente: %s\n", ipaddr_ntoa(&server_ip));
    tcp_connect(tcp_client_pcb, &server_ip, 80, enviar_dados);
  } else {
    tcp_close(tcp_client_pcb);
  }
  return true;
}

// Adiciona um temporizador dinâmico para enviar dados ao ThingSpeak
void configurar_thingspeak() {
  add_repeating_timer_ms(THINGSPEAK_INTERVALO, conectar_thingspeak_callback, NULL, &timer_thingspeak);
}