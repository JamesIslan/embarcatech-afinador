#include "display.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "joystick.h"
#include "pico/stdlib.h"
#include "src/bitmaps.h"
#include "src/inc/ssd1306.h"
#include <stdbool.h> // Adicione esta linha
#include <stdio.h>

struct repeating_timer timer_joystick;
struct repeating_timer timer_display;
volatile int display_menu_index = 0;

ssd1306_t ssd_bm;

void configurar_display() {
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();

  struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
  };

  calculate_render_area_buffer_length(&frame_area);
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);
}

bool joystick_callback(struct repeating_timer *t) {
  adc_select_input(ADC_CHANNEL_VRX); // X axis
  sleep_us(2);
  uint16_t vrx_value = adc_read();
  printf("X: %d", vrx_value);

  if (vrx_value >= 4000) { // Joystick up?
    display_menu_index = (display_menu_index == 0) ? 5 : --display_menu_index;
    printf("Index: %i\n", display_menu_index);
  } else if (vrx_value <= 100) { // Joystick down?
    display_menu_index = (display_menu_index == 5) ? 0 : ++display_menu_index;
    printf("Index: %i\n", display_menu_index);
  }
  return true;
}

bool display_callback(struct repeating_timer *t) {
  ssd1306_draw_bitmap(&ssd_bm, menu_opcoes[display_menu_index]);
  printf("Display atualizado");
  return true;
}

void iniciar_display() {
  ssd1306_draw_bitmap(&ssd_bm, menu_opcao_um);
  add_repeating_timer_ms(75, joystick_callback, NULL, &timer_joystick);
  add_repeating_timer_ms(75, display_callback, NULL, &timer_display);
}