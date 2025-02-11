#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "hardware/interp.h"
#include "hardware/spi.h"
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "src/constants.h"
#include <stdio.h>

int64_t alarm_callback(alarm_id_t id, void *user_data) {
  // Put your timeout handler code in here
  return 0;
}

int main() {
  stdio_init_all();

  // Initialise the Wi-Fi chip
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  // SPI initialisation. This example will use SPI at 1MHz.
  spi_init(SPI_PORT, 1000 * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);
  // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);
  // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

  // Interpolator example code
  interp_config cfg = interp_default_config();
  // Now use the various interpolator library functions for your use case
  // e.g. interp_config_clamp(&cfg, true);
  //      interp_config_shift(&cfg, 2);
  // Then set the config
  interp_set_config(interp0, 0, &cfg);
  // For examples of interpolator use see https://github.com/raspberrypi/pico-examples/tree/master/interp

  // Timer example code - This example fires off the callback after 2000ms
  add_alarm_in_ms(2000, alarm_callback, NULL, false);
  // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

  printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
  printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
  // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks

  // Enable wifi station
  cyw43_arch_enable_sta_mode();

  printf("Connecting to Wi-Fi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms("JCDA", "josemario1515", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    printf("failed to connect.\n");
    return 1;
  } else {
    printf("Connected.\n");
    // Read the ip address in a human readable way
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
  }

  // Set up our UART
  uart_init(UART_ID, BAUD_RATE);
  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Use some the various UART functions to send out data
  // In a default system, printf will also output via the default UART

  // Send out a string, with CR/LF conversions
  uart_puts(UART_ID, " Hello, UART!\n");

  // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

  while (true) {
    printf("Hello, world!\n");
    sleep_ms(1000);
  }
}
