#include "hardware/dma.h"

#define DIVISOR_CLOCK 960
#define FSAMP 50000
#define FREQUENCIA_MAXIMA_CAPTURA 500
#define MIC_CANAL_ADC 2
#define MIC_GPIO 28
#define N_AMOSTRAS 4000

struct corda_violao {
  int frequencia_lida;
  int frequencia_desejada;
};
extern struct corda_violao cordas[];
extern uint dma_channel;
extern dma_channel_config dma_cfg;

extern void setup_mic();
extern void pegar_amostra(uint8_t *capture_buf);
extern void main_fft();
