#include "../fft/kiss_fftr.h"
#include "hardware/dma.h"

#define DIVISOR_CLOCK 960
#define FSAMP 50000
#define FREQUENCIA_MAXIMA_CAPTURA 500
#define MIC_CANAL_ADC 2
#define MIC_GPIO 28
#define N_AMOSTRAS 4000

extern kiss_fftr_cfg cfg_fftr;
extern struct repeating_timer timer_mic;
struct corda_violao {
  int frequencia_lida;
  int frequencia_desejada;
};
extern struct corda_violao cordas[];

extern void configurar_mic();
extern void pegar_amostra(uint8_t *capture_buf);
// extern bool main_fft(struct repeating_timer *t);
// extern void main_fft();
