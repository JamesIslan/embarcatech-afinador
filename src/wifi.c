#include "wifi.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "pico/cyw43_arch.h"

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
