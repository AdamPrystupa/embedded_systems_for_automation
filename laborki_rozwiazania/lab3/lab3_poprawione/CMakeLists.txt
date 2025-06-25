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
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Lab3_zad1 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add all source files
file(GLOB SOURCES 
    "src/*.c"
)

# Add executable with all source files
add_executable(Lab3_zad1 
    ${SOURCES}
)

pico_set_program_name(Lab3_zad1 "Lab3_zad1")
pico_set_program_version(Lab3_zad1 "0.1")

# Enable UART for terminal functionality
pico_enable_stdio_uart(Lab3_zad1 1)
pico_enable_stdio_usb(Lab3_zad1 0)

# Add the standard library to the build
target_link_libraries(Lab3_zad1
    pico_stdlib
    hardware_uart
    hardware_gpio
    hardware_watchdog
    pico_cyw43_arch_none
)

# Add include directories
target_include_directories(Lab3_zad1 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Add any additional libraries needed
target_link_libraries(Lab3_zad1 
    pico_cyw43_arch_none
)

pico_add_extra_outputs(Lab3_zad1)