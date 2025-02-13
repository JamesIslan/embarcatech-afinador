#include "display.h"
#include "bitmaps.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "joystick.h"
#include "pico/stdlib.h"
#include "src/inc/ssd1306.h"
#include <stdbool.h> // Adicione esta linha
#include <stdio.h>
#include <string.h>

struct repeating_timer timer_joystick;
struct repeating_timer timer_display;
volatile int display_menu_index = 0;

ssd1306_t ssd_bm;
char buffer_freq_lida[128];
char buffer_freq_desejada[128];

char *TEMPLATE_LEITURA_MIC[] = {
  "               ",
  "FREQUENCIA LIDA",
  buffer_freq_lida,
  "               ",
  "   FREQUENCIA  ",
  "    DESEJADA   ",
  buffer_freq_desejada,
  "               "};

char *TEMPLATE_LEITURA_MIC_FORMATADO[8];

uint8_t ssd[ssd1306_buffer_length];

struct render_area frame_area = {
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

void configurar_display() {
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  ssd1306_init();
  calculate_render_area_buffer_length(&frame_area);
}

void configurar_display_texto() {
  configurar_display();
}

void configurar_display_bitmap() {
  configurar_display();
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);
}
bool joystick_callback(struct repeating_timer *t) {
  adc_select_input(ADC_CHANNEL_VRX); // X axis
  sleep_us(2);
  uint16_t vrx_value = adc_read();
  // printf("X: %d", vrx_value);

  if (vrx_value >= 4000) { // Joystick up?
    display_menu_index = (display_menu_index == 0) ? 5 : --display_menu_index;
    // printf("Index: %i\n", display_menu_index);
  } else if (vrx_value <= 100) { // Joystick down?
    display_menu_index = (display_menu_index == 5) ? 0 : ++display_menu_index;
    // printf("Index: %i\n", display_menu_index);
  }
  return true;
}

bool display_callback(struct repeating_timer *t) {
  ssd1306_draw_bitmap(&ssd_bm, menu_opcoes[display_menu_index]);
  return true;
}

void escrever_string_display(char *text[], uint8_t *ssd, struct render_area *frame_area, size_t line_count) {
  configurar_display_texto();
  memset(ssd, 0, ssd1306_buffer_length);
  int y_axis = 0;

  for (size_t index = 0; index < line_count; index++) {
    ssd1306_draw_string(ssd, 5, y_axis, text[index]);
    y_axis += 8;
  }

  render_on_display(ssd, frame_area);
}

void exibir_leitura_mic(int frequencia_lida, int frequencia_desejada) {
  sprintf(buffer_freq_lida, "    %d Hz", frequencia_lida);
  sprintf(buffer_freq_desejada, "    %d Hz", frequencia_desejada);
  for (int i = 0; i < 8; i++) {
    if (i == 2) {
      // Substitui a linha 3 pelo valor formatado de "FREQUENCIA LIDA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_lida;
    } else if (i == 6) {
      // Substitui a linha 6 pelo valor formatado de "FREQUENCIA DESEJADA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_desejada;
    } else {
      // Copia as outras linhas do array original
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = TEMPLATE_LEITURA_MIC[i];
    }
  }
  for (int i = 0; i < 8; i++) {
    printf("%s\n", TEMPLATE_LEITURA_MIC_FORMATADO[i]);
  }
  escrever_string_display(TEMPLATE_LEITURA_MIC_FORMATADO, ssd, &frame_area, count_of(TEMPLATE_LEITURA_MIC_FORMATADO));
  printf("Escreveu string!");
}

void exibir_bitmap_display(uint8_t text[]) {
  configurar_display_bitmap();
  ssd1306_draw_bitmap(&ssd_bm, text);
  printf("Escreveu bitmap!");
}

void iniciar_display() {
  exibir_bitmap_display(menu_opcao_um);
  // ssd1306_draw_bitmap(&ssd_bm, menu_opcao_um);
  add_repeating_timer_ms(75, joystick_callback, NULL, &timer_joystick);
  add_repeating_timer_ms(75, display_callback, NULL, &timer_display);
}