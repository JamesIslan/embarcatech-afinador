#include <stdbool.h>

#define PINO_BOTAO 5

extern volatile bool modo_menu;
extern void configurar_botao();
extern void configurar_interrupcao_botao(bool estado);