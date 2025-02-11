#include "wifi.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
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

static ip_addr_t resolved_ip;
static bool dns_resolved = false;

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
  if (ipaddr == NULL) {
    printf("DNS lookup failed for %s\n", name);
    dns_resolved = true;
    return;
  }

  ip_addr_copy(resolved_ip, *ipaddr);
  dns_resolved = true;
  printf("Resolved IP address for %s: %s\n", name, ipaddr_ntoa(ipaddr));
}

ip_addr_t obter_ip_via_dns(const char *hostname) {
  ip_addr_t addr;
  dns_resolved = false;
  err_t err = dns_gethostbyname(hostname, &addr, dns_callback, NULL);
  switch (err) {
  case ERR_OK: // Processo concluído
    dns_callback(hostname, &addr, NULL);
    break;
  case ERR_INPROGRESS: // Processo em andamento
    printf("DNS request in progress for %s\n", hostname);
    while (!dns_resolved) {
      cyw43_arch_poll();
    }
    break;
  default: // Processo falho
    printf("DNS request failed for %s: %d\n", hostname, err);
    break;
  }
  return resolved_ip;
}

// void enviar_dados(const char *url) {
//   const char *hostname = "api.thingspeak.com"; // https://api.thingspeak.com
//   ip_addr_t addr;

//   struct tcp_pcb *pcb;
//   struct ip4_addr dest_ip;
//   err_t err;

//   // Resolve the IP address of the server
//   ip4addr_aton("192.168.1.100", &dest_ip); // Replace with the server's IP address

//   // Create a new TCP PCB
//   pcb = tcp_new();
//   if (!pcb) {
//     printf("Error creating PCB\n");
//     return;
//   }

//   // Connect to the server
//   err = tcp_connect(pcb, &dest_ip, 80, NULL);
//   if (err != ERR_OK) {
//     printf("Error connecting to server: %d\n", err);
//     tcp_close(pcb);
//     return;
//   }

//   // Create the HTTP GET request
//   char request[256];
//   snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: 192.168.1.100\r\n\r\n", url);

//   // Send the request
//   err = tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY);
//   if (err != ERR_OK) {
//     printf("Error sending request: %d\n", err);
//     tcp_close(pcb);
//     return;
//   }

//   // Close the connection
//   tcp_close(pcb);
// }

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