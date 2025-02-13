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
  sleep_ms(10000);
  configurar_perifericos();
  printf("Iniciando display\n");
  exibir_bitmap_display(menu_conexao_pendente);
  sleep_ms(5000);
  printf("Iniciando configuração do joystick\n");
  setup_joystick();
  printf("Iniciando configuração do botão\n");
  configurar_botao();

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

  sleep_ms(5000);

  // printf("Iniciando configuração do microfone!");

  // configurar_mic();
  // dma_channel = dma_claim_unused_channel(true);
  // // Configurações do DMA.
  // dma_cfg = dma_channel_get_default_config(dma_channel);

  // // channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)

  // // channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)

  // // channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)

  // // channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

  // // Amostragem de teste.
  // printf("Amostragem de teste...\n");
  // sample_mic();

  // printf("Configuracoes completas!\n");
  // while (true) {
  //   adc_select_input(2);
  //   uint16_t mic_value = adc_read();
  //   // Realiza uma amostragem do microfone.
  //   // sample_mic();

  //   // Pega a potência média da amostragem do microfone.
  //   // float avg = mic_power();
  //   printf("Intensidade: %i\n", mic_value);
  //   sleep_ms(100);
  //   // avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
  // }

  while (true) {
    sleep_ms(100); // Reduz o uso da CPU
  }
  cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
}
