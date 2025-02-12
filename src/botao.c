#include "botao.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Buffer para armazenar a descrição do evento
static char event_str[128];

// Função para converter eventos de GPIO em uma string legível
void gpio_event_string(char *buf, uint32_t events) {
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

// Função de callback chamada quando ocorre uma interrupção no GPIO
void gpio_callback(uint gpio, uint32_t events) {
  gpio_event_string(event_str, events);    // Converte os eventos em uma string
  printf("GPIO %d %s\n", gpio, event_str); // Imprime o evento
}

void configurar_botao() {
  gpio_init(PINO_BOTAO);
  gpio_set_dir(PINO_BOTAO, GPIO_IN);
  gpio_pull_up(PINO_BOTAO);
  gpio_set_irq_enabled_with_callback(PINO_BOTAO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
  printf("IRQ definido!");
}
