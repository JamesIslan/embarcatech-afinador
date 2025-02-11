#define LED_PIN 12    // Pino do LED
#define BUTTON1_PIN 5 // Pino do botão 1
#define BUTTON2_PIN 6 // Pino do botão 2

extern void criar_resposta_http();
extern void monitor_buttons();
extern void start_http_server(void);
// extern static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
