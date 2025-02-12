#include "botao.h"
#include "pico/stdlib.h"

void configurar_botao() {
  gpio_init(PINO_BOTAO);
  gpio_set_dir(PINO_BOTAO, GPIO_IN);
  gpio_pull_up(PINO_BOTAO);
}