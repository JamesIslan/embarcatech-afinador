# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

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
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(embarcatech-afinador C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(embarcatech-afinador
        embarcatech-afinador.c
        src/bitmaps.c
        src/botao.c
        src/display.c
        src/joystick.c
        src/mic.c
        src/wifi.c
        src/inc/ssd1306_i2c.c
        src/fft/kiss_fft.c
        src/fft/kiss_fftr.c
        )

# add_library(kiss_fftr kiss_fftr.c) # FFT
# add_library(kiss_fft kiss_fft.c) # FFT

pico_set_program_name(embarcatech-afinador "embarcatech-afinador")
pico_set_program_version(embarcatech-afinador "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(embarcatech-afinador 1)
pico_enable_stdio_usb(embarcatech-afinador 1)

# Add the standard library to the build
target_link_libraries(embarcatech-afinador
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(embarcatech-afinador PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# pico_add_extra_outputs(adc_fft) # FFT

# Add any user requested libraries
target_link_libraries(embarcatech-afinador
        hardware_adc
        hardware_clocks
        hardware_dma
        hardware_i2c
        hardware_interp
        hardware_timer
        pico_cyw43_arch_lwip_threadsafe_background
        )

pico_add_extra_outputs(embarcatech-afinador)

