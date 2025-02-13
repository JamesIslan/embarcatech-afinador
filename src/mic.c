#include "mic.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include <math.h>

// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];

struct corda_violao cordas[] = {
  {.frequencia_lida = 0, .frequencia_desejada = 82},   // Corda E grave
  {.frequencia_lida = 0, .frequencia_desejada = 119},  // Corda B
  {.frequencia_lida = 0, .frequencia_desejada = 196},  // Corda G
  {.frequencia_lida = 0, .frequencia_desejada = 247},  // Corda D
  {.frequencia_lida = 0, .frequencia_desejada = 330},  // Corda A
  {.frequencia_lida = 0, .frequencia_desejada = 330}}; // Corda E agudo

void sample_mic() {
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false);   // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
                        adc_buffer,      // Escreve no buffer.
                        &(adc_hw->fifo), // Lê do ADC.
                        SAMPLES,         // Faz "SAMPLES" amostras.
                        true             // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);

  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);
}

float mic_power() {
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];

  avg /= SAMPLES;
  return sqrt(avg);
}

uint8_t get_intensity(float v) {
  uint count = 0;

  while ((v -= ADC_STEP / 20) > 0.f)
    ++count;

  return count;
}

void configurar_mic() {
  adc_init();
  adc_gpio_init(MIC_PIN);
  // adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
      true,  // Habilitar FIFO
      true,  // Habilitar request de dados do DMA
      1,     // Threshold para ativar request DMA é 1 leitura do ADC
      false, // Não usar bit de erro
      false  // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );
  adc_set_clkdiv(ADC_CLOCK_DIV);
}