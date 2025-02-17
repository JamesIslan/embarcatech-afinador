#include "joystick.h"
#include "../botao/botao.h"
#include "../display/bitmaps.h"
#include "../display/display.h"
#include "hardware/adc.h"
#include <stdio.h>

struct repeating_timer timer_joystick;

// Callback disparado a casa 100ms que verifica o estado do joystick.
// Atualiza o display caso alguma mudança no joystick seja detectada.
// Retorna: true para continuar temporização; false para pará-la
bool joystick_callback(struct repeating_timer *t) {
  // printf("Entrando no joystick callback\n");
  if (!modo_menu) {
    // printf("Retornando false joystick callback\n");
    return false;
  }
  adc_select_input(VRX_CANAL_ADC); // Eixo X
  // Aviso importante: Pelo fato de o joystick da BitDogLab estar
  // com um sentido "invertido" em relação ao norte da placa, optou-se
  // por utilizar o eixo "X" para interação com o menu. Desta forma,
  // ao posicionar o joystick para cima, o menu também sobe, e vice-versa.
  uint16_t vrx_value = adc_read();
  if (vrx_value >= 4000) { // Joystick para cima?
    indice_menu = (indice_menu == 0) ? 5 : --indice_menu;
    exibir_bitmap_display(menu_opcoes[indice_menu]);
  } else if (vrx_value <= 100) { // Joystick para baixo?
    indice_menu = (indice_menu == 5) ? 0 : ++indice_menu;
    exibir_bitmap_display(menu_opcoes[indice_menu]);
  }
  return true;
}

// Inicia temporizador que verifica o estado do joystick a casa 100ms
void iniciar_temporizador_joystick() {
  cancel_repeating_timer(&timer_joystick);
  add_repeating_timer_ms(100, joystick_callback, NULL, &timer_joystick);
}

// Configuração inicial do joystick
void configurar_joystick() {
  adc_init();
  adc_gpio_init(JOYSTICK_PINO_VRX); // Define JOYSTICK_PINO_VRX como input ADC
  iniciar_temporizador_joystick();
}