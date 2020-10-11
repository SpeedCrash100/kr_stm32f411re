cmake_minimum_required(VERSION 3.11)

include(cmake/toolchain.cmake)

project(
    kr_stm32f411re
    VERSION 0.0.1
    LANGUAGES C ASM
)

add_definitions(
    -DSTM32F411xE
)

add_subdirectory(Startup)
add_subdirectory(CMSIS)
add_subdirectory(STM32F4xx_HAL_Driver)

file(GLOB_RECURSE SOURCES "Src/*.c")

add_executable(kr_stm32f411re.elf ${SOURCES})
target_include_directories(kr_stm32f411re.elf PRIVATE Inc)
target_link_libraries(kr_stm32f411re.elf PRIVATE startuplib hal)


