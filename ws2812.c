#include "ws2812.h"
#include "pico/stdlib.h"


// Inicializa o pino para saída de dados no WS2812
void ws2812_init(uint pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}

// Simples função para exibir números na matriz WS2812 (exemplo fictício)
void ws2812_display_number(uint pin, int number)
{
    // Lógica para exibir o número na matriz 5x5 (personalize conforme necessário)
    for (int i = 0; i < number; i++)
    {
        gpio_put(pin, 1);
        sleep_ms(100);
        gpio_put(pin, 0);
        sleep_ms(100);
    }
}


