#include "botao.h"
#include "../display/display.h"
#include "../joystick/joystick.h"
#include "../led/led.h"
#include "../mic/mic.h"
#include "pico/stdlib.h"
#include <stdio.h>

static char event_str[128];     // Buffer para armazenar a descrição do evento
volatile bool modo_menu = true; // Variável para controlar estados do display, botão e microfone

// Função para converter eventos de GPIO em uma string legível
void converter_eventos_gpio(char *buf, uint32_t events) {
  const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
  };

  for (uint i = 0; i < 4; i++) {
    uint mask = (1 << i);                      // Cria uma máscara para cada tipo de evento
    if (events & mask) {                       // Verifica se o evento ocorreu
      const char *event_str = gpio_irq_str[i]; // Pega a string correspondente
      while (*event_str != '\0') {             // Copia a string para o buffer
        *buf++ = *event_str++;
      }
      events &= ~mask; // Remove o evento já processado

      if (events) { // Se houver mais eventos, adiciona ", "
        *buf++ = ',';
        *buf++ = ' ';
      }
    }
  }
  *buf++ = '\0'; // Finaliza a string
}

// Função de callback chamada quando ocorre um pressionamento no botão
void callback_botao_pressionado(uint gpio, uint32_t events) {
  converter_eventos_gpio(event_str, events);
  // printf("GPIO %d %s\n", gpio, event_str); // Imprime o evento
  modo_menu = !modo_menu;
  if (modo_menu) { // Modo menu ligado -> Tela de seleção de cordas
    busy_wait_ms(200);
    max_freq = 0;
    apagar_led();            // Apaga LED se estiver aceso
    kiss_fft_free(cfg_fftr); // Reseta configurações de FFT
    iniciar_temporizador_joystick();
    iniciar_menu();
  } else { // Modo menu desligado -> Tela de afinação de corda
    busy_wait_ms(200);
    configurar_mic();
    gerenciar_afinacao();
  }
}

// Configura interrupção quando botão é pressionado e levantado (EDGE_FALL -> EDG_RISE)
void configurar_interrupcao_botao(bool estado) {
  gpio_set_irq_enabled_with_callback(PINO_BOTAO, GPIO_IRQ_EDGE_FALL && GPIO_IRQ_EDGE_RISE, estado, &callback_botao_pressionado);
}

// Configuração inicial do joystick
void configurar_botao() {
  gpio_init(PINO_BOTAO);
  gpio_set_dir(PINO_BOTAO, GPIO_IN);
  gpio_pull_up(PINO_BOTAO);
  configurar_interrupcao_botao(true);
  // printf("IRQ definido!");
}
