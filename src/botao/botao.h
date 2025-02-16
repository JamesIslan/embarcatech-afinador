#include <stdbool.h>

#define PINO_BOTAO 5

extern volatile bool modo_menu; // Declaração da variável para poder ser utilizada em outros lugares
extern void configurar_botao();
extern void configurar_interrupcao_botao(bool estado);