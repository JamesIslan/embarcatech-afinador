// Baseado no algorítmo de Alex Wulff
// https://github.com/AlexFWulff/awulff-pico-playground/tree/e0c98d544ad0cf7972edaf5215ae165e835f29eb/adc_fft

#include "mic.h"
#include "fft/kiss_fftr.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

dma_channel_config cfg;
uint dma_chan;
float freqs[N_AMOSTRAS];

struct corda_violao cordas[] = {
  {.frequencia_lida = 0, .frequencia_desejada = 82},   // Corda E grave
  {.frequencia_lida = 0, .frequencia_desejada = 119},  // Corda B
  {.frequencia_lida = 0, .frequencia_desejada = 196},  // Corda G
  {.frequencia_lida = 0, .frequencia_desejada = 247},  // Corda D
  {.frequencia_lida = 0, .frequencia_desejada = 330},  // Corda A
  {.frequencia_lida = 0, .frequencia_desejada = 330}}; // Corda E agudo

void setup_mic() {
  // stdio_init_all();

  // CONFIGURAÇÃO DO ADC
  adc_gpio_init(MIC_GPIO);
  adc_init();
  adc_select_input(MIC_CANAL_ADC);
  adc_fifo_setup(
      true,
      true,
      1,
      false,
      true);
  adc_set_clkdiv(DIVISOR_CLOCK);
  //

  sleep_ms(1000);

  // CONFIGURAÇÃO DO DIRECT MEMORY ACCESS
  uint dma_chan = dma_claim_unused_channel(true);
  cfg = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, true);
  channel_config_set_dreq(&cfg, DREQ_ADC);
  //

  float f_max = FSAMP;
  float f_res = f_max / N_AMOSTRAS;
  for (int i = 0; i < N_AMOSTRAS; i++) {
    freqs[i] = f_res * i;
  }
}

void pegar_amostra(uint8_t *capture_buf) { // Acessa o ADC e armazena o valor no buffer
  adc_fifo_drain();
  adc_run(false);

  dma_channel_configure(dma_chan, &cfg,
                        capture_buf,   // dst
                        &adc_hw->fifo, // src
                        N_AMOSTRAS,    // transfer count
                        true           // start immediately
  );

  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_chan);
}

void main_fft() {
  uint8_t buffer_capturado[N_AMOSTRAS];
  kiss_fft_scalar fft_in[N_AMOSTRAS];
  kiss_fft_cpx fft_out[N_AMOSTRAS];
  kiss_fftr_cfg cfg = kiss_fftr_alloc(N_AMOSTRAS, false, 0, 0);

  setup_mic();

  while (true) {
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
    kiss_fftr(cfg, fft_in, fft_out);
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
    float max_freq = freqs[max_idx];
    printf("Greatest Frequency Component: %0.2f Hz\n", max_freq);
    //
  }

  kiss_fft_free(cfg);
}