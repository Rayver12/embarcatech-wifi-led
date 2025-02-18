#define WIFI_SSID "ED-LINK FIBRA // Joao" // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "JOAO2FILHO8"           // Substitua pela senha da sua rede Wi-Fi
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "inc/ssd1306.h"
#include "hardware/i2c.h"

#define LED_PIN_B 12
#define LED_PIN_R 13
#define LED_PIN_G 11  // Pino do LED
#define BUTTON1_PIN 5 // Pino do botão 1
#define BUTTON2_PIN 6 // Pino do botão 2

#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define BRIGHTNESS_STEP 10

// Pinos do I2C para o OLED
#define I2C_SDA 14
#define I2C_SCL 15

// Função para inicializar o OLED
void init_oled()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
}

// Função para exibir mensagem no OLED
void display_message(char *line1, char *line2, char *line3)
{
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    ssd1306_draw_string(ssd, 5, 8, line1);
    ssd1306_draw_string(ssd, 5, 24, line2);
    ssd1306_draw_string(ssd, 5, 40, line3);

    render_on_display(ssd, &frame_area);
}

int brightness = 0; // Brilho inicial (0%)

int percentual()
{
    int valor = (brightness * 100) / 255;
    return valor;
}

// Estado dos botões (inicialmente sem mensagens)
char button1_message[50] = "Nenhum evento no botão 1";
char button2_message[50] = "Nenhum evento no botão 2";

char status_icon1[10] = "";
char status_icon2[10] = "";

// Buffer para resposta HTTP
char http_response[4096];

// Função para criar a resposta HTTP
void create_http_response()
{
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
             "<!DOCTYPE html>"
             "<html>"
             "<head>"
             "<meta charset=\"UTF-8\">"
             "<title>Controle Pico W</title>"
             "<style>"
             "body {"
             "  font-family: 'Segoe UI', sans-serif;"
             "  background: linear-gradient(135deg, #1a1a1a, #0d0d0d);"
             "  color: #fff;"
             "  margin: 0;"
             "  padding: 20px;"
             "  min-height: 100vh;"
             "  display: flex;"
             "  align-items: center;"
             "  justify-content: center;"
             "}"
             ".container {"
             "  max-width: 600px;"
             "  width: 100%%;"
             "  background: rgba(255,255,255,0.1);"
             "  padding: 30px;"
             "  border-radius: 20px;"
             "  backdrop-filter: blur(10px);"
             "  box-shadow: 0 8px 32px rgba(0,0,0,0.3);"
             "  border: 1px solid rgba(255,255,255,0.1);"
             "}"
             "h1 {"
             "  text-align: center;"
             "  color: #00ff88;"
             "  font-size: 2.5rem;"
             "  margin-bottom: 20px;"
             "  text-shadow: 0 0 10px rgba(0,255,136,0.5);"
             "}"
             ".btn-group {"
             "  display: flex;"
             "  gap: 15px;"
             "  justify-content: center;"
             "  margin: 30px 0;"
             "}"
             "a {"
             "  display: inline-flex;"
             "  align-items: center;"
             "  gap: 10px;"
             "  padding: 15px 30px;"
             "  background: linear-gradient(135deg, #00ff88, #00cc66);"
             "  color: #1a1a1a;"
             "  text-decoration: none;"
             "  border-radius: 12px;"
             "  transition: all 0.3s ease;"
             "  font-weight: bold;"
             "  box-shadow: 0 4px 15px rgba(0,255,136,0.3);"
             "}"
             "a:hover {"
             "  transform: translateY(-3px);"
             "  box-shadow: 0 6px 20px rgba(0,255,136,0.5);"
             "}"
             ".status {"
             "  background: rgba(0,0,0,0.2);"
             "  padding: 20px;"
             "  border-radius: 12px;"
             "  margin: 25px 0;"
             "}"
             ".status h2 {"
             "  color: #00ff88;"
             "  margin-top: 0;"
             "  font-size: 1.5rem;"
             "}"
             ".status p {"
             "  display: flex;"
             "  align-items: center;"
             "  gap: 10px;"
             "  margin: 10px 0;"
             "}"
             ".status-icon {"
             "  width: 12px;"
             "  height: 12px;"
             "  border-radius: 50%%;"
             "  background: #444;"
             "}"
             ".status-icon.active {"
             "  background: #00ff88;"
             "  box-shadow: 0 0 8px rgba(0,255,136,0.5);"
             "}"
             ".brightness-bar {"
             "  width: 100%%;"
             "  height: 10px;"
             "  background: #444;"
             "  border-radius: 5px;"
             "  margin-top: 10px;"
             "  overflow: hidden;"
             "}"
             ".brightness-progress {"
             "  height: 100%%;"
             "  background: linear-gradient(90deg, #00cc66, #00ff88);"
             "  border-radius: 5px;"
             "  width: %d%%;"
             "}"
             "</style>"
             "</head>"
             "<body>"
             "<div class=\"container\">"
             "  <h1>🚀 Controle Pico W</h1>"
             "  <div class=\"btn-group\">"
             "    <a href=\"/led/on\">🔆 Ligar LED</a>"
             "    <a href=\"/led/off\">🌑 Desligar LED</a>"
             "  </div>"
             "  <div class=\"status\">"
             "    <h2>📊 Estado do Sistema</h2>"
             "    <p><span class=\"status-icon %s\"></span>%s</p>"
             "    <p><span class=\"status-icon %s\"></span>%s</p>"
             "  </div>"
             "  <div class=\"status\">"
             "    <h2>💡 Brilho do LED</h2>"
             "    <p>Brilho atual: %d%%</p>"
             "    <div class=\"brightness-bar\">"
             "      <div class=\"brightness-progress\"></div>"
             "    </div>"
             "  </div>"
             "  <div class=\"btn-group\">"
             "    <a href=\"/update\">🔄 Atualizar Estado</a>"
             "  </div>"
             "</div>"
             "</body>"
             "</html>\r\n",
             percentual(), status_icon1, button1_message, status_icon2, button2_message, percentual());
}

// Função para configurar o PWM
void configure_pwm(uint pin)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, MAX_BRIGHTNESS);
    pwm_set_gpio_level(pin, brightness);
    pwm_set_enabled(slice_num, true);
}

// Atualiza o brilho dos LEDs
void update_led_brightness()
{
    pwm_set_gpio_level(LED_PIN_B, brightness);
    pwm_set_gpio_level(LED_PIN_R, brightness);
    pwm_set_gpio_level(LED_PIN_G, brightness);
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (p == NULL)
    {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *request = (char *)p->payload;

    if (strstr(request, "GET /led/on"))
    {
        brightness = MAX_BRIGHTNESS;
        update_led_brightness();
        printf("Led ligado: %d\n", brightness);
    }
    else if (strstr(request, "GET /led/off"))
    {
        brightness = MIN_BRIGHTNESS;
        update_led_brightness();
        printf("Led desligado: %d\n", brightness);
    }

    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
    pbuf_free(p);

    return ERR_OK;
}

// Callback de conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, http_callback);
    return ERR_OK;
}

// Função de setup do servidor TCP
static void start_http_server(void)
{
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb)
    {
        printf("Erro ao criar PCB\n");
        return;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK)
    {
        printf("Erro ao ligar o servidor na porta 80\n");
        return;
    }

    pcb = tcp_listen(pcb);
    tcp_accept(pcb, connection_callback);
    printf("Servidor HTTP rodando na porta 80...\n");
}

// Função para monitorar o estado dos botões
void monitor_buttons()
{
    static bool button1_last_state = false;
    static bool button2_last_state = false;

    bool button1_state = !gpio_get(BUTTON1_PIN);
    bool button2_state = !gpio_get(BUTTON2_PIN);

    if (button1_state && !button1_last_state)
    {
        button1_last_state = button1_state;
        brightness = brightness + BRIGHTNESS_STEP > MAX_BRIGHTNESS ? MAX_BRIGHTNESS : brightness + BRIGHTNESS_STEP;
        update_led_brightness();
        snprintf(button1_message, sizeof(button1_message), "Botão 1 pressionado: brilho aumentado");
        snprintf(status_icon1, sizeof(status_icon1), "active");
        printf("Brilho aumentado: %d\n", brightness);
    }
    else if (!button1_state)
    {
        button1_last_state = false;
        snprintf(button1_message, sizeof(button1_message), "Botão 1 não pressionado!");
        snprintf(status_icon1, sizeof(status_icon1), "");
    }

    if (button2_state && !button2_last_state)
    {
        button2_last_state = button2_state;
        brightness = brightness - BRIGHTNESS_STEP < MIN_BRIGHTNESS ? MIN_BRIGHTNESS : brightness - BRIGHTNESS_STEP;
        update_led_brightness();
        snprintf(button2_message, sizeof(button2_message), "Botão 2 pressionado: brilho reduzido");
        snprintf(status_icon2, sizeof(status_icon2), "active");
        printf("Brilho reduzido: %d\n", brightness);
    }
    else if (!button2_state)
    {
        button2_last_state = false;
        snprintf(button2_message, sizeof(button2_message), "Botão 2 não pressionado!");
        snprintf(status_icon2, sizeof(status_icon2), "");
    }
}

int main()
{
    stdio_init_all();

    // Inicializa o OLED
    init_oled();
    display_message("OLED", "INICIADO", NULL);

    sleep_ms(1000);
    printf("Iniciando servidor HTTP\n");

    if (cyw43_arch_init())
    {
        printf("Erro ao inicializar o Wi-Fi\n");
        display_message("Erro:", "Wi-Fi falhou", NULL);
        return 1;
    }

    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");
    display_message("Conectando", "ao Wi-Fi", NULL);

    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
    {
        printf("Falha ao conectar ao Wi-Fi\nTentando novamente!\n");
        display_message("Falha", "Tentando", "Novamente");
    }

    printf("Wi-Fi conectado!\n");
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    char ip_str[20];
    snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    printf("Endereço IP: %s\n", ip_str);
    display_message("IP Address:", ip_str, NULL);

    configure_pwm(LED_PIN_B);
    configure_pwm(LED_PIN_R);
    configure_pwm(LED_PIN_G);

    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON1_PIN);

    gpio_init(BUTTON2_PIN);
    gpio_set_dir(BUTTON2_PIN, GPIO_IN);
    gpio_pull_up(BUTTON2_PIN);

    start_http_server();

    while (true)
    {
        cyw43_arch_poll();
        monitor_buttons();
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}