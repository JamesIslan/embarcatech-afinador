#include "../fft/kiss_fftr.h"
#include "hardware/dma.h"

#define MIC_CANAL_ADC 2
#define MIC_GPIO 28

#define DIVISOR_CLOCK 960
#define FSAMP 50000
#define FREQUENCIA_MAXIMA_CAPTURA 500
#define N_AMOSTRAS 6500

extern volatile float max_freq;
extern kiss_fftr_cfg cfg_fftr;
extern struct repeating_timer timer_mic;
struct corda_violao {
  float frequencia_lida;
  float frequencia_desejada;
};
extern struct corda_violao cordas[];

extern void configurar_mic();
extern void pegar_amostra(uint16_t *capture_buf);