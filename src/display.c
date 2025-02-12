#include "display.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "joystick.h"
#include "pico/stdlib.h"
#include "src/bitmaps.h"
#include "src/inc/ssd1306.h"
#include <stdbool.h> // Adicione esta linha
#include <stdio.h>

struct repeating_timer sw_timer;

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

  adc_select_input(ADC_CHANNEL_VRY); // Y axis
  sleep_us(2);
  uint16_t vry_value = adc_read();
  printf("X: %d, Y: %d\n", vrx_value, vry_value);
  return true;
}

void iniciar_display() {
  ssd1306_draw_bitmap(&ssd_bm, menu_opcao_um);
  add_repeating_timer_ms(50, joystick_callback, NULL, &sw_timer);
}