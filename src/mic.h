#include "hardware/dma.h"

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN 28

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200                                   // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f / 5.f) // Intervalos de volume do microfone.

struct corda_violao {
  int frequencia_lida;
  int frequencia_desejada;
};
extern struct corda_violao cordas[];
extern uint dma_channel;
extern dma_channel_config dma_cfg;

extern void sample_mic();
extern float mic_power();
extern uint8_t get_intensity(float v);
extern void configurar_mic();