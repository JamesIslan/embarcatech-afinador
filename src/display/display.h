#include "lib/inc/ssd1306.h"

#define DISPLAY_PINO_SDA 14
#define DISPLAY_PINO_SCL 15
extern ssd1306_t ssd_bm;
extern volatile int display_menu_index;

extern void configurar_display();
extern void exibir_bitmap_display(uint8_t text[]);
extern void gerenciar_afinacao();
extern void iniciar_menu();