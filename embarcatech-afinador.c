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
  return 0;
}

int main() {
  stdio_init_all();

  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  spi_init(SPI_PORT, 1000 * 1000);
  gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
  gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

  gpio_set_dir(PIN_CS, GPIO_OUT);
  gpio_put(PIN_CS, 1);
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  interp_config cfg = interp_default_config();
  interp_set_config(interp0, 0, &cfg);
  add_alarm_in_ms(2000, alarm_callback, NULL, false);
  printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
  printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
  cyw43_arch_enable_sta_mode();

  printf("Connecting to Wi-Fi...\n");
  if (cyw43_arch_wifi_connect_timeout_ms("JCDA", "josemario1515", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
    printf("failed to connect.\n");
    return 1;
  } else {
    printf("Connected.\n");
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
  }

  uart_init(UART_ID, BAUD_RATE);
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
  uart_puts(UART_ID, " Hello, UART!\n");

  while (true) {
    printf("Hello, world!\n");
    sleep_ms(1000);
  }
}
