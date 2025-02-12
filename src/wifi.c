#include "wifi.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"
#include "src/constants.h"

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

// void enviar_dados() {
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
