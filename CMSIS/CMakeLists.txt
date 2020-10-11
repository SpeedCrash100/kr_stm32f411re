# This file is not part of CMSIS. It's created for build system by SpeedCrash100.


set(CMSIS_SOURCES
    Device/ST/STM32F4xx/Source/system_stm32f4xx.c
)

set(CMSIS_INCLUDE_DIRS
    Device/ST/STM32F4xx/Include
    Inc
)

add_library(cmsis STATIC ${CMSIS_SOURCES})
target_include_directories(cmsis PUBLIC ${CMSIS_INCLUDE_DIRS})

