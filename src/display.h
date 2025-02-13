#include "inc/ssd1306.h"

// #define I2C_PORT i2c0
#define I2C_SDA 14
#define I2C_SCL 15
extern ssd1306_t ssd_bm;

extern void configurar_display();
extern void configurar_display_texto();
extern void configurar_display_bitmap();
// extern void escrever_string_display();
extern void escrever_string_display(char *text[], uint8_t *ssd, struct render_area *frame_area, size_t line_count);
extern void exibir_leitura_mic(int frequencia_lida, int frequencia_desejada);
// extern bool joystick_callback(struct temporizador_display *t);
extern void iniciar_display();