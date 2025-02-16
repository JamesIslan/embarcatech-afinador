#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <stdio.h>
// Import de dispositivos
#include "env.h"
#include "src/bitmaps/bitmaps.h"
#include "src/botao/botao.h"
#include "src/display/display.h"
#include "src/joystick/joystick.h"
#include "src/led/led.h"
#include "src/wifi/wifi.h"
//

int main() {
  stdio_init_all();
  printf("Iniciando LED\n");
  configurar_led();
  printf("Iniciando display\n");
  configurar_display();
  exibir_bitmap_display(menu_conexao_pendente);
  sleep_ms(3000);
  printf("Iniciando configuração do joystick\n");
  setup_joystick();
  printf("Iniciando configuração do botão\n");
  configurar_botao();
  printf("Iniciando configuração do microfone!\n");
  if (cyw43_arch_init()) {
    printf("Inicialização do Wi-Fi falhou!\n");
    return -1;
  }
  printf("Configurando Wi-Fi...");

  configurar_wifi();
  iniciar_display();
  configurar_thingspeak();

  while (true) {
    sleep_ms(100); // Reduz o uso da CPU
  }
  cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
