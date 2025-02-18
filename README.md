# Controle de LED com Pico W e OLED

Este projeto foi desenvolvido para a Raspberry Pi Pico W e demonstra o controle de LEDs utilizando PWM, monitoramento de botões físicos e exibição de status em um display OLED. Além disso, um servidor HTTP é implementado para possibilitar o controle remoto dos LEDs via interface web.

## Sumário

- [Descrição](#descrição)
- [Funcionalidades](#funcionalidades)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)
- [Pré-requisitos](#pré-requisitos)
- [Instalação e Compilação](#instalação-e-compilação)
- [Uso](#uso)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Destaques do Código](#destaques-do-código)
- [Contribuições](#contribuições)
- [Licença](#licença)
- [Contato](#contato)

---

## Descrição

## Este projeto tem como objetivo controlar o brilho de LEDs conectados à Raspberry Pi Pico W, utilizando tanto botões físicos quanto uma interface web. O display OLED exibe mensagens de status (como o progresso da conexão Wi-Fi, endereço IP e eventos dos botões) e o servidor HTTP permite ligar ou desligar os LEDs remotamente.

## Funcionalidades

- **Conexão Wi-Fi:** Configuração e conexão automática à rede definida.
- **Servidor HTTP:** Permite controlar os LEDs (ligar/desligar) através de endpoints HTTP.
- **Controle de LED via PWM:** Ajuste do brilho dos LEDs por meio de sinais PWM.
- **Display OLED:** Exibição de mensagens e status do sistema.
- **Monitoramento de Botões:** Aumenta ou diminui o brilho dos LEDs com base na interação com os botões físicos.

---

## Tecnologias Utilizadas

- **Raspberry Pi Pico W**
- **Pico SDK:** Uso das bibliotecas `pico/stdlib.h` e `pico/cyw43_arch.h`
- **LWIP:** Para comunicação TCP/IP
- **SSD1306:** Biblioteca para controle do display OLED via I2C
- **Hardware PWM:** Controle do brilho dos LEDs
- **Linguagem C**

---

## Pré-requisitos

- Raspberry Pi Pico W
- Ambiente de desenvolvimento configurado para o Pico SDK (consulte a [documentação oficial](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf))
- Display OLED compatível com SSD1306 (conectado via I2C)
- Conexão com a internet para testar o servidor HTTP

---

## Instalação e Compilação

1. **Configurar o Ambiente Pico SDK:**

   Siga as instruções da documentação oficial para instalar e configurar o Pico SDK.

2. **Clonar o Repositório:**

```bash
   git clone https://github.com/Rayver12/embarcatech-wifi-led.git
   cd embarcatech-wifi-led
```

3. **Ajustar Credenciais de Wi-Fi:**

No início do código, modifique os defines `WIFI_SSID` e `WIFI_PASS` com o nome e a senha da sua rede:

```C
#define WIFI_SSID "Teste"           // Nome da rede Wi-Fi
#define WIFI_PASS "12345"           // Senha da rede Wi-Fi
```

4. **Compilar o Projeto:**

   Crie um diretório de build, configure o CMake e compile:

```bash
    mkdir build && cd build
    cmake ..
    make
```

5. **Upload para a Pico W:**

   Conecte a Pico W ao computador e copie o arquivo `.uf2` gerado para a unidade de armazenamento que aparecerá.

---

## Uso

Após o upload e inicialização, o sistema realiza as seguintes ações:

- Inicializa o display OLED e exibe mensagens de status.
- Conecta-se à rede Wi-Fi usando as credenciais fornecidas.
- Inicia um servidor HTTP na porta 80.
- Monitora dois botões físicos para ajustar o brilho dos LEDs:
  - **Botão** 1: Aumenta o brilho.
  - **Botão** 2: Diminui o brilho.
- Permite controlar os LEDs via requisições HTTP:
  - Acesse `http://<ip_da_pico>/led/on` para ligar o LED (brilho máximo).
  - Acesse `http://<ip_da_pico>/led/off` para desligar o LED (brilho mínimo).

---

## Estrutura do Projeto

    embarcatech-wifi-led/
    ├── README.md             # Este arquivo
    ├── webserver_testes.c    # Código fonte principal
    ├── inc/
    │   └── ssd1306.h         # Biblioteca para controle do display OLED
    └── (outros arquivos e pastas conforme a organização do projeto)

---

## Destaques do Código

1. **Configuração de Wi-Fi e Inicialização do OLED**

```C
#define WIFI_SSID "Teste"           // Nome da rede Wi-Fi
#define WIFI_PASS "12345"           // Senha da rede Wi-Fi

// Inicializa o OLED e configura os pinos I2C
void init_oled() {
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
}
```

Destaque: Inicializa o display OLED e configura a comunicação I2C para que as mensagens possam ser exibidas.

2. **Criação da Resposta HTTP com Interface Web**

```html
void create_http_response() {
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
```

Destaque: A função `create_http_response()` monta dinamicamente uma página HTML com botões para controle e informações sobre o sistema, integrando dados como o brilho atual e o status dos botões.

3. **Configuração do PWM e Controle do LED**

```C
void configure_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_num, MAX_BRIGHTNESS);
    pwm_set_gpio_level(pin, brightness);
    pwm_set_enabled(slice_num, true);
}

void update_led_brightness() {
    pwm_set_gpio_level(LED_PIN_B, brightness);
    pwm_set_gpio_level(LED_PIN_R, brightness);
    pwm_set_gpio_level(LED_PIN_G, brightness);
}
```

Destaque: Estas funções configuram o PWM nos pinos designados e atualizam o nível de brilho dos LEDs, permitindo o controle fino do hardware.

4. **Monitoramento dos Botões Físicos**

```C
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
```

Destaque: Essa função detecta o estado dos botões físicos, ajustando o brilho dos LEDs e atualizando as mensagens de status exibidas tanto no OLED quanto na interface web.

---

## Contribuições

Contribuições são sempre bem-vindas! Se você deseja colaborar:

1. Faça um fork deste repositório.
2. Crie uma branch para a sua feature:

```bash
git checkout -b feature/nome-da-feature
```

3. Realize suas alterações e faça commit:

```bash
git commit -m "Descrição da feature implementada"
```

4. Envie sua branch para o repositório remoto:

```bash
git push origin feature/nome-da-feature
```

5. Abra um Pull Request detalhando as mudanças.

---

## Licença

Distribuído sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.

---

## Contato

- **Nome:** Rafael Avelino dos Santos
- **Email:** rafaelavelino0908@gmail.com
- **GitHub:** [Rayver12](https://github.com/Rayver12)

---
