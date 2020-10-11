set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING ON)

### Searching parameters ###
set(tools_path "C:/Dev/ARM/gnu-toolchain" CACHE PATH "Path to build tools") # path to build tools
set(prefix "arm-none-eabi-" CACHE STRING "Prefix that prepend to utils")

### Searching tools ###
find_program(
    CMAKE_C_COMPILER_FIND 
    NAMES
        ${prefix}gcc
    PATHS
        ${tools_path}
    PATH_SUFFIXES
        bin
)

if(NOT CMAKE_C_COMPILER_FIND)
    message(FATAL_ERROR "Cannot find C compiler: ${prefix}gcc")
endif(NOT CMAKE_C_COMPILER_FIND)
set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER_FIND})
message("Finded C compiler: ${CMAKE_C_COMPILER}")

find_program(
    CMAKE_CXX_COMPILER_FIND 
    NAMES
        ${prefix}g++
    PATHS
        ${tools_path}
    PATH_SUFFIXES
        bin
)

if(NOT CMAKE_CXX_COMPILER_FIND)
    message(FATAL_ERROR "Cannot find CXX compiler: ${prefix}g++")
endif(NOT CMAKE_CXX_COMPILER_FIND)
set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER_FIND})
message("Finded CXX compiler: ${CMAKE_CXX_COMPILER}")

set(CMAKE_ASM_COMPILER "${CMAKE_C_COMPILER}")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")
message(STATUS "Using asm compiler: ${CMAKE_ASM_COMPILER}")

find_program(
    CMAKE_OBJCOPY
    NAMES
        ${prefix}objcopy
    PATHS
        ${tools_path}
    PATH_SUFFIXES
        bin
)
if(NOT CMAKE_OBJCOPY)
    message(FATAL_ERROR "Cannot find: ${prefix}objcopy")
endif(NOT CMAKE_OBJCOPY)
message("Finded objcopy: ${CMAKE_OBJCOPY}")

find_program(
    CMAKE_SIZE
    NAMES
        ${prefix}size
    PATHS
        ${tools_path}
    PATH_SUFFIXES
        bin
)
if(NOT CMAKE_SIZE)
    message(FATAL_ERROR "Cannot find: ${prefix}size")
endif(NOT CMAKE_SIZE)
message("Finded size: ${CMAKE_SIZE}")

### Configuring flags for compilers ###
set(CPU_FLAGS -mcpu=cortex-m4)
set(FPU_FLAGS -mfpu=fpv4-sp-d16)
set(FLOAT_ABI_FLAGS -mfloat-abi=hard)

set(MCU_FLAGS "${CPU_FLAGS} -mthumb ${FPU_FLAGS} ${FLOAT_ABI_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS_Debug "-DDEBUG -g -gdwarf-2")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS_Debug "-DDEBUG -g -gdwarf-2")

### Configuring linker flags ###

set(LIBS "-lc -lm -lnosys")
set(LDSCRIPT "${CMAKE_SOURCE_DIR}/STM32F411RETx_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nosys.specs -T${LDSCRIPT} ${LIBS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.map,--cref -Wl,--gc-sections")
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)


### Commands def ###
set(BIN_GENERATE_FLAGS "-O binary -S")
set(HEX_GENERATE_FLAGS "-O ihex")
