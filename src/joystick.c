#include "joystick.h"
#include "hardware/adc.h"

void setup_joystick() {
  adc_init();
  adc_gpio_init(VRX_PIN); // Set VRX_PIN to ADC input
}