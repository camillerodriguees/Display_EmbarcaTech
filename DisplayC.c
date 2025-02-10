#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDR 0x3C

#define LED_PIN_R 11
#define LED_PIN_G 12
#define LED_PIN_B 13
#define WS2812_PIN 7

#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define NUM_PIXELS 25
#define BRIGHTNESS 50
#define DEBOUNCE_DELAY 200  // Tempo de debounce em ms

bool led_green_state = false;
bool led_blue_state = false;
uint32_t last_press_time_A = 0;
uint32_t last_press_time_B = 0;

ssd1306_t ssd;

// Mapeamento dos números (0-9) para matriz 5x5
const bool number_to_matrix[10][NUM_PIXELS] = {
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 0
    {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0}, // 1
    {1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 2
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 3
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // 4
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 5
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 6
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 7
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}  // 9
};

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void display_number_matrix(uint8_t num) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(number_to_matrix[num][i] ? urgb_u32(0, BRIGHTNESS, 0) : 0);
    }
}

void display_message(const char *message) {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, message, 10, 20);
    ssd1306_send_data(&ssd);
}

void handle_uart_input(char c) {
    printf("Recebido: '%c'\n", c);

    char message[20];  // Buffer para armazenar a string formatada
    sprintf(message, "Núm: %c", c);  // Formata a string corretamente

    display_message(message);  // Envia a string formatada para o display

    if (c >= '0' && c <= '9') {
        display_number_matrix(c - '0');
    }
}

void handle_button_press(uint gpio, uint32_t events) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    if (gpio == BUTTON_A_PIN && (current_time - last_press_time_A) > DEBOUNCE_DELAY) {
        last_press_time_A = current_time;
        led_green_state = !led_green_state;
        gpio_put(LED_PIN_R, led_green_state);
        printf("Botão A pressionado. LED Verde %s\n", led_green_state ? "Ligado" : "Desligado");
        display_message(led_green_state ? "LED A ON" : "LED A OFF");
    } 
    else if (gpio == BUTTON_B_PIN && (current_time - last_press_time_B) > DEBOUNCE_DELAY) {
        last_press_time_B = current_time;
        led_blue_state = !led_blue_state;
        gpio_put(LED_PIN_G, led_blue_state);
        printf("Botão B pressionado. LED Azul %s\n", led_blue_state ? "Ligado" : "Desligado");
        display_message(led_blue_state ? "LED B ON" : "LED B OFF");
    }
}


int main() {
    stdio_init_all();

     gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);  // LED Vermelho desligado

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);  // LED Verde desligado

    gpio_init(LED_PIN_B);
    gpio_set_dir(LED_PIN_B, GPIO_OUT);
    gpio_put(LED_PIN_B, 0);  // LED Azul desligado

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &handle_button_press);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &handle_button_press);

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);

    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    display_message("Clique no botão");

    printf("Clique no botão.\n");

    while (true) {
        if (stdio_usb_connected()) {
            char c;
            if (scanf("%c", &c) == 1) {
                handle_uart_input(c);
            }
        }
    }

    return 0;
}
