#include "display.h"
#include "../bitmaps/bitmaps.h"
#include "../botao/botao.h"
#include "../inc/ssd1306.h"
#include "../joystick/joystick.h"
#include "../led/led.h"
#include "../mic/mic.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdbool.h> // Adicione esta linha
#include <stdio.h>
#include <string.h>

struct repeating_timer timer_joystick;
struct repeating_timer timer_display;
volatile int display_menu_index = 0;
volatile bool display_modo_bitmap = false;

ssd1306_t ssd_bm;
char buffer_freq_lida[128];
char buffer_freq_desejada[128];

char *TEMPLATE_LEITURA_MIC[] = {
  "FREQUENCIA LIDA",
  "               ",
  buffer_freq_lida,
  "               ",
  "   FREQUENCIA  ",
  "    DESEJADA   ",
  "               ",
  buffer_freq_desejada};

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
  calculate_render_area_buffer_length(&frame_area);
}

void configurar_display_texto() {
  ssd1306_init();
}

void configurar_display_bitmap() {
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);
}

bool joystick_callback(struct repeating_timer *t) {
  // printf("Entrando no joystick callback\n");
  if (!modo_menu) {
    // cancel_repeating_timer(&timer_joystick);
    // printf("Retornando false joystick callback\n");
    return false;
  }
  adc_select_input(ADC_CHANNEL_VRX); // X axis
  // sleep_us(2);
  uint16_t vrx_value = adc_read();
  // printf("X: %d", vrx_value);
  if (vrx_value >= 4000) { // Joystick up?
    display_menu_index = (display_menu_index == 0) ? 5 : --display_menu_index;
    exibir_bitmap_display(menu_opcoes[display_menu_index]);
  } else if (vrx_value <= 100) { // Joystick down?
    display_menu_index = (display_menu_index == 5) ? 0 : ++display_menu_index;
    exibir_bitmap_display(menu_opcoes[display_menu_index]);
  }
  return true;
}

// bool display_callback(struct repeating_timer *t) {
//   if (!modo_menu) {
//     cancel_repeating_timer(&timer_display);
//     printf("Retornando false display callback\n");
//     return false;
//   }
//   ssd1306_draw_bitmap(&ssd_bm, menu_opcoes[display_menu_index]);
//   printf("Callback display rodou");
//   return true;
// }

void escrever_string_display(char *text[], uint8_t *ssd, struct render_area *frame_area, size_t line_count) {
  if (display_modo_bitmap) {
    configurar_display_texto();
    display_modo_bitmap = false;
  }
  memset(ssd, 0, ssd1306_buffer_length);
  int y_axis = 0;

  for (size_t index = 0; index < line_count; index++) {
    ssd1306_draw_string(ssd, 5, y_axis, text[index]);
    y_axis += 8;
  }

  render_on_display(ssd, frame_area);
}

void exibir_leitura_mic(float frequencia_lida, float frequencia_desejada) {
  sprintf(buffer_freq_lida, "    %0.2f Hz", frequencia_lida);
  sprintf(buffer_freq_desejada, "    %0.2f Hz", frequencia_desejada);
  for (int i = 0; i < 8; i++) {
    if (i == 2) {
      // Substitui a linha 3 pelo valor formatado de "FREQUENCIA LIDA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_lida;
    } else if (i == 7) {
      // Substitui a linha 6 pelo valor formatado de "FREQUENCIA DESEJADA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_desejada;
    } else {
      // Copia as outras linhas do array original
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = TEMPLATE_LEITURA_MIC[i];
    }
  }
  // for (int i = 0; i < 8; i++) {
  //   printf("%s\n", TEMPLATE_LEITURA_MIC_FORMATADO[i]);
  // }
  escrever_string_display(TEMPLATE_LEITURA_MIC_FORMATADO, ssd, &frame_area, count_of(TEMPLATE_LEITURA_MIC_FORMATADO));
  // printf("Escreveu string!\n");
}

void exibir_bitmap_display(uint8_t text[]) {
  if (!display_modo_bitmap) {
    configurar_display_bitmap();
    display_modo_bitmap = true;
  }
  ssd1306_draw_bitmap(&ssd_bm, text);
  // printf("Escreveu bitmap!\n");
}

// void gerenciar_afinacao() {
//   struct corda_violao obj = cordas[display_menu_index];
//   obj.frequencia_lida = max_freq;
//   exibir_leitura_mic(obj.frequencia_lida, obj.frequencia_desejada);
//   // while (true) {
//   //   // printf("Objeto: %d\n", obj.frequencia_desejada);
//   //   printf("Frequência lida: %f", obj.frequencia_lida);
//   //   busy_wait_ms(50);
//   // }
//   // exibir_bitmap_display(menu_afinacao_concluida); // Mensagem de afinação concluída
// }

bool display_notas_callback() {
  if (modo_menu) {
    return false;
  }
  struct corda_violao obj = cordas[display_menu_index];
  obj.frequencia_lida = max_freq;
  float diferenca_leitura = fabs(obj.frequencia_desejada - obj.frequencia_lida);
  if (diferenca_leitura <= 5) {
    acender_led_verde();
    busy_wait_ms(2000);
    configurar_interrupcao_botao(true); // Permitir pressionamento
    exibir_bitmap_display(menu_afinacao_concluida);
    cancel_repeating_timer(&timer_mic);
    return false;
  }
  if (5 < diferenca_leitura && diferenca_leitura < 10) {
    acender_led_amarelo();
  } else {
    acender_led_vermelho();
  }
  exibir_leitura_mic(obj.frequencia_lida, obj.frequencia_desejada);
  return true;
}

void gerenciar_afinacao() {
  configurar_interrupcao_botao(false);
  add_repeating_timer_ms(50, display_notas_callback, NULL, &timer_display);
}

void iniciar_display() {
  // sleep_ms(200);
  exibir_bitmap_display(menu_opcoes[display_menu_index]);
  cancel_repeating_timer(&timer_joystick);
  add_repeating_timer_ms(100, joystick_callback, NULL, &timer_joystick);
  // add_repeating_timer_ms(75, display_callback, NULL, &timer_display);
}