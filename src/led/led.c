#include "led.h"
#include "pico/stdlib.h"
#include <stdio.h>

void apagar_led() {
  gpio_put(PINO_LED_VERMELHO, 0);
  gpio_put(PINO_LED_VERDE, 0);
  gpio_put(PINO_LED_AZUL, 0);
};

void acender_led_verde() {
  gpio_put(PINO_LED_VERMELHO, 0);
  gpio_put(PINO_LED_VERDE, 1);
  gpio_put(PINO_LED_AZUL, 0);
}

void acender_led_amarelo() {
  gpio_put(PINO_LED_VERMELHO, 1);
  gpio_put(PINO_LED_VERDE, 1);
  gpio_put(PINO_LED_AZUL, 0);
}

void acender_led_vermelho() {
  gpio_put(PINO_LED_VERMELHO, 1);
  gpio_put(PINO_LED_VERDE, 0);
  gpio_put(PINO_LED_AZUL, 0);
}

void configurar_led() {
  gpio_init(PINO_LED_VERMELHO);
  gpio_set_dir(PINO_LED_VERMELHO, GPIO_OUT);
  gpio_init(PINO_LED_VERDE);
  gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
  gpio_init(PINO_LED_AZUL);
  gpio_set_dir(PINO_LED_AZUL, GPIO_OUT);
}