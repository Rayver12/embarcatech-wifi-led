# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
cmake_minimum_required(VERSION 3.29)

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

project(webserver_testes C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(webserver_testes webserver_testes.c inc/ssd1306_i2c.c )

pico_set_program_name(webserver_testes "webserver_testes")
pico_set_program_version(webserver_testes "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(webserver_testes 0)
pico_enable_stdio_usb(webserver_testes 1)

# Add the standard library to the build
target_link_libraries(webserver_testes
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(webserver_testes PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(webserver_testes 
        pico_cyw43_arch_lwip_threadsafe_background hardware_pwm hardware_i2c
        pico_cyw43_arch)

pico_add_extra_outputs(webserver_testes)

