# This file is not part of HAL driver fro stm32f4xx. It's created for build system by SpeedCrash100.

set(HAL_INCLUDE_DIRS
    Inc
)

file(GLOB HAL_SOURCES "Src/*.c")

add_library(hal STATIC ${HAL_SOURCES})
target_include_directories(hal PUBLIC ${HAL_INCLUDE_DIRS})
target_link_libraries(hal PUBLIC cmsis)