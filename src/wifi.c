#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

#include "wifi.h"
// Estado dos botões (inicialmente sem mensagens)
char button1_message[50] = "Nenhum evento no botão 1";
char button2_message[50] = "Nenhum evento no botão 2";

char http_response[1024];

void criar_resposta_http() {
  snprintf(http_response, sizeof(http_response),
           "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
           "<!DOCTYPE html>"
           "<html>"
           "<head>"
           "  <meta charset=\"UTF-8\">"
           "  <title>Controle do LED e Botões</title>"
           "</head>"
           "<body>"
           "  <h1>Controle do LED e Botões</h1>"
           "  <p><a href=\"/led/on\">Ligar LED</a></p>"
           "  <p><a href=\"/led/off\">Desligar LED</a></p>"
           "  <p><a href=\"/update\">Atualizar Estado</a></p>"
           "  <h2>Estado dos Botões:</h2>"
           "  <p>Botão 1: %s</p>"
           "  <p>Botão 2: %s</p>"
           "</body>"
           "</html>\r\n",
           button1_message, button2_message);
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
  if (p == NULL) {
    // Cliente fechou a conexão
    tcp_close(tpcb);
    return ERR_OK;
  }

  // Processa a requisição HTTP
  char *request = (char *)p->payload;

  if (strstr(request, "GET /led/on")) {
    gpio_put(LED_PIN, 1); // Liga o LED
  } else if (strstr(request, "GET /led/off")) {
    gpio_put(LED_PIN, 0); // Desliga o LED
  }

  // Atualiza o conteúdo da página com base no estado dos botões
  criar_resposta_http();

  // Envia a resposta HTTP
  tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);

  // Libera o buffer recebido
  pbuf_free(p);

  return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
  tcp_recv(newpcb, http_callback); // Associa o callback HTTP
  return ERR_OK;
}

// Função de setup do servidor TCP
void iniciar_servidor_http(void) {
  struct tcp_pcb *pcb = tcp_new();
  if (!pcb) {
    printf("Erro ao criar PCB\n");
    return;
  }

  // Liga o servidor na porta 80
  if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK) {
    printf("Erro ao ligar o servidor na porta 80\n");
    return;
  }

  pcb = tcp_listen(pcb);                // Coloca o PCB em modo de escuta
  tcp_accept(pcb, connection_callback); // Associa o callback de conexão

  printf("Servidor HTTP rodando na porta 80...\n");
}

// Função para monitorar o estado dos botões
void monitor_buttons() {
  static bool button1_last_state = false;
  static bool button2_last_state = false;

  bool button1_state = !gpio_get(BUTTON1_PIN); // Botão pressionado = LOW
  bool button2_state = !gpio_get(BUTTON2_PIN);

  if (button1_state != button1_last_state) {
    button1_last_state = button1_state;
    if (button1_state) {
      snprintf(button1_message, sizeof(button1_message), "Botão 1 foi pressionado!");
      printf("Botão 1 pressionado\n");
    } else {
      snprintf(button1_message, sizeof(button1_message), "Botão 1 foi solto!");
      printf("Botão 1 solto\n");
    }
  }

  if (button2_state != button2_last_state) {
    button2_last_state = button2_state;
    if (button2_state) {
      snprintf(button2_message, sizeof(button2_message), "Botão 2 foi pressionado!");
      printf("Botão 2 pressionado\n");
    } else {
      snprintf(button2_message, sizeof(button2_message), "Botão 2 foi solto!");
      printf("Botão 2 solto\n");
    }
  }
}