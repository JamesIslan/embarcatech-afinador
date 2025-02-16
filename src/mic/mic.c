// Baseado no algorítmo de Alex Wulff
// https://github.com/AlexFWulff/awulff-pico-playground/tree/e0c98d544ad0cf7972edaf5215ae165e835f29eb/adc_fft

#include "mic.h"
#include "../botao/botao.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

volatile float max_freq = 0.0;
uint16_t buffer_capturado[N_AMOSTRAS];
kiss_fft_scalar fft_in[N_AMOSTRAS];
kiss_fft_cpx fft_out[N_AMOSTRAS];
kiss_fftr_cfg cfg_fftr;

dma_channel_config cfg_dma;
uint dma_chan;
float freqs[N_AMOSTRAS];

struct repeating_timer timer_mic;

// Frequências retiradas de:
// https://en.wikipedia.org/wiki/Guitar_tunings
struct corda_violao cordas[] = {
  {.frequencia_lida = 0.0, .frequencia_desejada = 82.41},   // Corda E grave
  {.frequencia_lida = 0.0, .frequencia_desejada = 110.0},   // Corda A
  {.frequencia_lida = 0.0, .frequencia_desejada = 146.83},  // Corda D
  {.frequencia_lida = 0.0, .frequencia_desejada = 196.0},   // Corda B
  {.frequencia_lida = 0.0, .frequencia_desejada = 246.94},  // Corda G
  {.frequencia_lida = 0.0, .frequencia_desejada = 329.63}}; // Corda E agudo

// Acessa o ADC e armazena o valor no buffer
void pegar_amostra(uint16_t *capture_buf) {
  adc_fifo_drain();
  adc_run(false);

  dma_channel_configure(dma_chan, &cfg_dma,
                        capture_buf,   // Variável de escrita (buffer)
                        &adc_hw->fifo, // Leitura de dados a partir do ADC
                        N_AMOSTRAS,    // Quantidade de amostras
                        true           // Liga o DMA
  );

  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
}

// Configuração inicial dos parâmetros FFT
void configurar_fft_mic() {
  float f_max = FSAMP;
  float f_res = f_max / N_AMOSTRAS;
  for (int i = 0; i < N_AMOSTRAS; i++) {
    freqs[i] = f_res * i;
  }
  cfg_fftr = kiss_fftr_alloc(N_AMOSTRAS, false, 0, 0);
}

// Callback executado a cada 50ms que pega os dados do microfone e realiza FFT
// Retorna: true para continuar temporizador; false para pará-lo
// OBS: Checar arquivo lib/fft/INFO.MD
bool executar_fft(struct repeating_timer *t) {
  if (modo_menu) {
    printf("Saindo do microfone\n");
    return false;
  }
  // ALGORITMO FFT - FILTRAGEM DE FREQUÊNCIAS
  pegar_amostra(buffer_capturado);
  uint64_t sum = 0;
  for (int i = 0; i < N_AMOSTRAS; i++) {
    sum += buffer_capturado[i];
  }
  float avg = (float)sum / N_AMOSTRAS;
  for (int i = 0; i < N_AMOSTRAS; i++) {
    fft_in[i] = (float)buffer_capturado[i] - avg;
  }
  kiss_fftr(cfg_fftr, fft_in, fft_out);
  //

  // PEGAR A FREQUÊNCIA MÁXIMA LIDA
  float max_power = 0;
  int max_idx = 0;
  for (int i = 0; i < FREQUENCIA_MAXIMA_CAPTURA; i++) {
    float power = fft_out[i].r * fft_out[i].r + fft_out[i].i * fft_out[i].i;
    if (power > max_power) {
      max_power = power;
      max_idx = i;
    }
  }
  max_freq = freqs[max_idx];
  printf("Greatest Frequency Component: %0.2f Hz\n", max_freq);
  return true;
  //
}

// Configura o Conversor Analógico-Digital (ADC) do microfone
void configurar_adc_mic() {
  adc_gpio_init(MIC_GPIO);
  adc_init();
  adc_select_input(MIC_CANAL_ADC);
  adc_fifo_setup(
      true,
      true,
      1,
      false,
      false);
  adc_set_clkdiv(DIVISOR_CLOCK);
}

// Integra o microfone com o Direct Memory Access
void configurar_dma_mic() {
  uint dma_chan = dma_claim_unused_channel(true);
  cfg_dma = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&cfg_dma, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg_dma, false);
  channel_config_set_write_increment(&cfg_dma, true);
  channel_config_set_dreq(&cfg_dma, DREQ_ADC);
}

// Cria um temporizador de 50ms para executar FFT
void configurar_temporizador_mic() {
  cancel_repeating_timer(&timer_mic);
  add_repeating_timer_ms(50, executar_fft, NULL, &timer_mic);
}

// Configuração geral do microfone
void configurar_mic() {
  configurar_adc_mic();
  configurar_dma_mic();
  configurar_fft_mic();
  configurar_temporizador_mic();
}