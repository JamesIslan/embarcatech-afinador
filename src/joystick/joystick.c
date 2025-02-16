#include "joystick.h"
#include "hardware/adc.h"

void setup_joystick() {
  adc_init();
  adc_gpio_init(JOYSTICK_PINO_VRX); // Define JOYSTICK_PINO_VRX como input ADC
}