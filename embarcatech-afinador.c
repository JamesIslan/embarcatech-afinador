#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/interp.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
// Import de dispositivos
#include "src/bitmaps.h"
#include "src/botao.h"
#include "src/display.h"
#include "src/env.h"
#include "src/joystick.h"
// #include "src/mic.h"
#include "src/wifi.h"
//
#include <stdio.h>

int main() {
  stdio_init_all();
  printf("Iniciando display\n");
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

  interp_config cfg = interp_default_config();
  interp_set_config(interp0, 0, &cfg);
  printf("Configurando Wi-Fi...");
  if (!configurar_wifi()) {
    exibir_bitmap_display(menu_conexao_concluida);
    printf("Setup concluído!\n");
    iniciar_display();
  }

  // while (1) {
  //   enviar_para_thingspeak();
  //   printf("Dados enviados!");

  //   sleep_ms(30000); // Aguarda 30 segundos por conta do thingspeak
  // }

  while (true) {
    sleep_ms(100); // Reduz o uso da CPU
  }
  cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
