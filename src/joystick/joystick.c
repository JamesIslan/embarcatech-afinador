#include "joystick.h"
#include "../bitmaps/bitmaps.h"
#include "../botao/botao.h"
#include "../display/display.h"
#include "hardware/adc.h"
#include <stdio.h>

struct repeating_timer timer_joystick; // Mover para pasta de joystick

bool joystick_callback(struct repeating_timer *t) { // Mover para pasta de joystick
  // printf("Entrando no joystick callback\n");
  if (!modo_menu) {
    // cancel_repeating_timer(&timer_joystick);
    // printf("Retornando false joystick callback\n");
    return false;
  }
  adc_select_input(VRX_CANAL_ADC); // X axis
  uint16_t vrx_value = adc_read();
  if (vrx_value >= 4000) { // Joystick up?
    display_menu_index = (display_menu_index == 0) ? 5 : --display_menu_index;
    exibir_bitmap_display(menu_opcoes[display_menu_index]);
  } else if (vrx_value <= 100) { // Joystick down?
    display_menu_index = (display_menu_index == 5) ? 0 : ++display_menu_index;
    exibir_bitmap_display(menu_opcoes[display_menu_index]);
  }
  return true;
}

void iniciar_temporizador_joystick() {
  cancel_repeating_timer(&timer_joystick);                               // Mover para pasta de joystick
  add_repeating_timer_ms(100, joystick_callback, NULL, &timer_joystick); // Mover para pasta de joystick
}

void setup_joystick() {
  adc_init();
  adc_gpio_init(JOYSTICK_PINO_VRX); // Define JOYSTICK_PINO_VRX como input ADC
  iniciar_temporizador_joystick();
}