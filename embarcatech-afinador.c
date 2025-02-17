#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
// Import de dispositivos
#include "env.h"
#include "src/botao/botao.h"
#include "src/display/bitmaps.h"
#include "src/display/display.h"
#include "src/joystick/joystick.h"
#include "src/led/led.h"
#include "src/wifi/wifi.h"
//

int main() {
  stdio_init_all();
  printf("Iniciando configuração de LED...\n");
  configurar_led();
  printf("LED configurado.\n");
  printf("Iniciando configuração de display...\n");
  configurar_display();
  printf("Display configurado.\n");
  exibir_bitmap_display(menu_conexao_pendente);
  sleep_ms(3000);
  while (cyw43_arch_init()) {
    printf("Inicialização da placa wi-fi falhou!\n");
    sleep_ms(3000);
  }
  printf("Placa wi-fi iniciada!\n");
  printf("Iniciando configuração de Wi-Fi...\n");
  configurar_wifi();
  printf("Wi-fi configurado!\n");

  printf("Iniciando configuração do joystick...\n");
  configurar_joystick();
  printf("Joystick configurado!\n");
  printf("Iniciando configuração do botão...\n");
  configurar_botao();
  printf("Botão configurado!\n");

  printf("Iniciando menu...\n");
  iniciar_menu();
  printf("Menu iniciado!\n");
  printf("Iniciando configuração de ThingSpeak...\n");
  configurar_thingspeak();
  printf("ThingSpeak configurado!\n");

  while (true) {
    sleep_ms(100); // Reduz o uso da CPU
  }
  // tcp_close(tcp_client_pcb); // Fecha a conexão com o PCB
  cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
