#include "inc/ssd1306.h"

// #define I2C_PORT i2c0
#define I2C_SDA 14
#define I2C_SCL 15
extern ssd1306_t ssd_bm;

extern void configurar_display();
// extern bool joystick_callback(struct temporizador_display *t);
extern void iniciar_display();