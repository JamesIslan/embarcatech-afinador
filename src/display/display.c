#include "display.h"
#include "../botao/botao.h"
#include "../led/led.h"
#include "../mic/mic.h"
#include "bitmaps.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>

struct repeating_timer timer_display;
volatile int indice_menu = 0;
volatile bool display_modo_bitmap = false;

ssd1306_t ssd_bm;
uint8_t ssd[ssd1306_buffer_length];

char buffer_freq_lida[128];
char buffer_freq_desejada[128];

char *TEMPLATE_LEITURA_MIC[] = { // Template da string a ser exibida no display
  "FREQUENCIA LIDA",
  "               ",
  buffer_freq_lida,
  "               ",
  "   FREQUENCIA  ",
  "    DESEJADA   ",
  "               ",
  buffer_freq_desejada};

char *TEMPLATE_LEITURA_MIC_FORMATADO[8];

struct render_area frame_area = { // Representa as dimensões do display
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

// Configuração inicial do display
void configurar_display() {
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(DISPLAY_PINO_SDA, GPIO_FUNC_I2C);
  gpio_set_function(DISPLAY_PINO_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(DISPLAY_PINO_SDA);
  gpio_pull_up(DISPLAY_PINO_SCL);
  calculate_render_area_buffer_length(&frame_area);
}

// Configuração específica para exibir texto (strings)
void configurar_display_texto() {
  ssd1306_init();
}

// Configuração específica para exibir bitmaps
void configurar_display_bitmap() {
  ssd1306_init_bm(&ssd_bm, 128, 64, false, 0x3C, i2c1);
  ssd1306_config(&ssd_bm);
}

// Sai do modo bitmap (caso esteja) e renderiza uma string no display
void escrever_string_display(char *texto[], uint8_t *ssd, struct render_area *frame_area, size_t line_count) {
  if (display_modo_bitmap) {
    configurar_display_texto();
    display_modo_bitmap = false;
  }
  memset(ssd, 0, ssd1306_buffer_length);
  int display_eixo_y = 0;

  for (size_t indice = 0; indice < line_count; indice++) {
    ssd1306_draw_string(ssd, 5, display_eixo_y, texto[indice]);
    display_eixo_y += 8;
  }
  render_on_display(ssd, frame_area);
}

// Entra no modo bitmap (caso não esteja) e renderiza um bitmap no display
void exibir_bitmap_display(uint8_t bitmap[]) {
  if (!display_modo_bitmap) {
    configurar_display_bitmap();
    display_modo_bitmap = true;
  }
  ssd1306_draw_bitmap(&ssd_bm, bitmap);
}

// Preenche os placeholders da variável TEMPLATE_LEITURA_MIC com valores reais
void exibir_leitura_mic(float frequencia_lida, float frequencia_desejada) {
  sprintf(buffer_freq_lida, "    %0.2f Hz", frequencia_lida);
  sprintf(buffer_freq_desejada, "    %0.2f Hz", frequencia_desejada);
  for (int i = 0; i < 8; i++) {
    if (i == 2) {
      // Substitui a linha 3 pelo valor formatado de "FREQUENCIA LIDA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_lida;
    } else if (i == 7) {
      // Substitui a linha 8 pelo valor formatado de "FREQUENCIA DESEJADA"
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = buffer_freq_desejada;
    } else {
      // Copia as outras linhas do array original
      TEMPLATE_LEITURA_MIC_FORMATADO[i] = TEMPLATE_LEITURA_MIC[i];
    }
  }
  escrever_string_display(TEMPLATE_LEITURA_MIC_FORMATADO, ssd, &frame_area, count_of(TEMPLATE_LEITURA_MIC_FORMATADO));
}

// Callback que verifica a proximidade entre os dados de frequência captados e desejados
// Gerencia o LED RGB com base na leitura
// Retorna: true para continuar temporização; false para pará-la
bool display_notas_callback() {
  if (modo_menu) {
    return false;
  }
  struct corda_violao obj = cordas[indice_menu];
  obj.frequencia_lida = max_freq;
  float diferenca_leitura = fabs(obj.frequencia_desejada - obj.frequencia_lida);
  if (diferenca_leitura <= 4) { // Afinação concluida -> LED verde
    // OBS: Devido a uma limitação na resolução do hardware (microfone), e seguindo diversos
    // testes de performance, constatou-se que uma margem de 4Hz para mais ou para menos é
    // o máximo de precisão que se pode obter com este periférico.
    acender_led_verde();
    configurar_interrupcao_botao(true); // Permitir pressionamento
    exibir_bitmap_display(menu_afinacao_concluida);
    cancel_repeating_timer(&timer_mic);
    return false;
  }
  if (4 < diferenca_leitura && diferenca_leitura < 10) { // Diferença pequena -> LED amarelo
    acender_led_amarelo();
  } else { // Diferença grande -> LED vermelho
    acender_led_vermelho();
  }
  exibir_leitura_mic(obj.frequencia_lida, obj.frequencia_desejada);
  return true;
}

// Desabilita o botão temporariamente e configura um temporizador de 50ms oara o display
void gerenciar_afinacao() {
  configurar_interrupcao_botao(false);
  add_repeating_timer_ms(50, display_notas_callback, NULL, &timer_display);
}

// Inicialização básica do menu, exibindo a opção correspondente ao índice de indice_menu
void iniciar_menu() {
  exibir_bitmap_display(menu_opcoes[indice_menu]);
}