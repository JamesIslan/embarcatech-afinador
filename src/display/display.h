#include "../inc/ssd1306.h"

#define I2C_SDA 14
#define I2C_SCL 15
extern ssd1306_t ssd_bm;

extern void configurar_display();
extern void exibir_leitura_mic(int frequencia_lida, int frequencia_desejada);
extern void exibir_bitmap_display(uint8_t text[]);
extern void gerenciar_afinacao();
extern void iniciar_display();