#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/interp.h"
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
// Import de dispositivos
#include "src/bitmaps.h"
#include "src/botao.h"
#include "src/constants.h"
#include "src/display.h"
#include "src/joystick.h"
#include "src/mic.h"
#include "src/wifi.h"
//
#include <stdio.h>

uint dma_channel; // Tentar mantê-los apenas no arquivo mic.h
dma_channel_config dma_cfg;

int64_t alarm_callback(alarm_id_t id, void *user_data) {
  return 0;
}

void configurar_perifericos() {
  spi_init(SPI_PORT, 1000 * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);

  // CONFIGURAÇÃO DE COMUNICAÇÃO UART
  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

int main() {
  stdio_init_all();
  configurar_perifericos();
  printf("Iniciando display\n");
  exibir_bitmap_display(menu_conexao_pendente);
  sleep_ms(3000);
  printf("Iniciando configuração do joystick\n");
  setup_joystick();
  printf("Iniciando configuração do botão\n");
  configurar_botao();
  printf("Iniciando configuração do microfone!\n");
  configurar_mic();

  if (cyw43_arch_init()) {
    printf("Inicialização do Wi-Fi falhou!\n");
    return -1;
  }

  interp_config cfg = interp_default_config();
  interp_set_config(interp0, 0, &cfg);
  add_alarm_in_ms(2000, alarm_callback, NULL, false);
  printf("Configurando Wi-Fi...");
  if (!configurar_wifi()) {
    exibir_bitmap_display(menu_conexao_concluida);
    sleep_ms(3000);
    printf("Setup concluído!\n");
    iniciar_display();
  }
  sleep_ms(2000);

  while (1) {
    enviar_para_thingspeak();
    printf("Dados enviados!");

    sleep_ms(30000); // Aguarda 30 segundos por conta do thingspeak
  }

  while (true) {
    sleep_ms(100); // Reduz o uso da CPU
  }
  cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
