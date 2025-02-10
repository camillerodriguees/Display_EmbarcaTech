# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Inicializa o SDK do Pico
include(pico_sdk_import.cmake)

project(DisplayC C CXX ASM)

# Inicializa o SDK do Raspberry Pi Pico
pico_sdk_init()

# Adiciona o executável DisplayC
add_executable(DisplayC
    DisplayC.c
    ws2812.c
    inc/ssd1306.c
)

# Define o nome e versão do programa
pico_set_program_name(DisplayC "DisplayC")
pico_set_program_version(DisplayC "0.1")

# Configura o I/O padrão (UART e USB)
pico_enable_stdio_uart(DisplayC 0)
pico_enable_stdio_usb(DisplayC 1)

# Processa o código PIO para gerar ws2812.pio.h automaticamente
pico_generate_pio_header(DisplayC ${CMAKE_CURRENT_SOURCE_DIR}/ws2812.pio)

# Adiciona bibliotecas necessárias para o projeto
target_link_libraries(DisplayC
    pico_stdlib
    hardware_i2c
    hardware_pio  # Necessário para o controle dos WS2812
)

# Adiciona diretórios de cabeçalhos ao projeto
target_include_directories(DisplayC PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    inc
)

# Gera saídas extras (arquivo UF2, etc.)
pico_add_extra_outputs(DisplayC)
