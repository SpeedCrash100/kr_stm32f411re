#include "maincontoller.h"

#include <stdlib.h>

#include "adc.h"
#include "display.h"
#include "keypad.h"
#include "pwm.h"
#include "stm32f4xx_hal.h"
#include "uart.h"

/// @ingroup MC
/// @{

#define UART_PACKET_SIZE 10240  /// Размер пакета  UART

/// Глобальный контроллер
struct {
  MainStates state;  /// Текущее состояние

  STM32ADC* adc;     /// АЦП
  Display* display;  /// Дисплей
  PWM* pwm;          /// Генератор ШИМ
  UART* uart;        /// UART
  Keypad* keypad;    /// Клавиатура

  Boolean uart_working;  /// Работает ли UART
  Boolean uart_half;     /// Получена ли половина пакета
  Boolean uart_full;     /// Получен ли пакет полностью

} mainContoller;

Boolean Init_RCC();

MainStates MainController_WaitingState(uint8_t* uartPackage,
                                       int32_t sizeOfUart);
MainStates MainController_StartedState(uint8_t* uartPackage, int32_t sizeOfUart,
                                       int32_t bufUsage);

MainStates MainController_StoppingState(int32_t bufUsage);

Boolean MainContoller_Init() {
  /// Настройка частоты
  if (!Init_RCC()) return FALSE;

  /// Инициализация STM32ADC
  mainContoller.adc = ADC_Init();
  if (mainContoller.adc == NULL) return FALSE;

  /// Инициализация Display
  mainContoller.display = Display_Init();
  if (mainContoller.display == NULL) return FALSE;

  /// Инициализация PWM
  mainContoller.pwm = PWM_Init();
  if (!mainContoller.pwm) return FALSE;

  /// Инициализация UART
  mainContoller.uart = UART_Init();
  if (!mainContoller.uart) return FALSE;

  /// Инициализация Keypad
  mainContoller.keypad = Keypad_Init();
  if (!mainContoller.keypad) return FALSE;

  mainContoller.state = Stopped;
  mainContoller.uart_working = FALSE;
  mainContoller.uart_half = FALSE;
  mainContoller.uart_full = FALSE;

  return TRUE;
}

/**
 * @brief UartHalf функция, вызываемая при приеме половины пакета данных
 * @details устанавливает флаг uart_half = TRUE
 */
void UartHalf() {
  if (mainContoller.uart_working) {
    mainContoller.uart_half = TRUE;
  }
}

/**
 * @brief UartFull функция, вызываемая при приеме пакета данных
 * @details сообщает контролеру, что на следующей итерации необходимо обработать
 * вторую часть пакета
 *
 * @param errored провален ли прием данных
 */
void UartFull(Boolean errored) {
  if (!mainContoller.uart_working) return;

  if (errored) {
    mainContoller.uart_working = FALSE;
    mainContoller.uart_full = FALSE;
    mainContoller.uart_half = FALSE;

    while (1) {
      __NOP();
    }
  }

  mainContoller.uart_full = TRUE;
  mainContoller.uart_working = FALSE;
}

void MainContoller_Loop() {
  uint16_t result = 0;
  int32_t bufUsage = 0;

  uint8_t uartPackage[UART_PACKET_SIZE];
  int uartPackageSize = sizeof(uartPackage);

  while (TRUE) {
    result = ADC_Get(mainContoller.adc) * 100 / 8192;
    PWM_GetBufferUsage(mainContoller.pwm, &bufUsage);

    int32_t realF = PWM_SetFreq(mainContoller.pwm, 2500 + 150 * result);

    Display_SetFreq(mainContoller.display, realF);
    Display_SetBufferUsage(mainContoller.display, bufUsage);
    Display_SetState(mainContoller.display, mainContoller.state);

    switch (mainContoller.state) {
      case Stopped:
        if (Keypad_GetState(mainContoller.keypad, KeyStartStop) == Clicked) {
          mainContoller.state = Waiting;
        }
        break;
      case Waiting:
        mainContoller.state =
            MainController_WaitingState(uartPackage, uartPackageSize);
        break;
      case Started:
        mainContoller.state =
            MainController_StartedState(uartPackage, uartPackageSize, bufUsage);
        break;
      case Stopping:
        mainContoller.state = MainController_StoppingState(bufUsage);
        break;
    }
  }
}

/**
 * @brief Init_RCC настраивает частоту ядра процессора
 * @return TRUE если успешно
 *
 * @remark Частота устанавливается на 50 МГц. Тактирование от внешнего источника
 */
Boolean Init_RCC() {
  // Init HAL
  HAL_StatusTypeDef status = HAL_Init();
  if (status != HAL_OK) {
    return FALSE;
  }

  __HAL_RCC_GPIOH_CLK_ENABLE();  // For HSE

  // Init clocks
  RCC_ClkInitTypeDef clkInitStruct = {0};
  RCC_OscInitTypeDef oscInitStruct = {0};

  oscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  oscInitStruct.HSEState = RCC_HSE_ON;
  oscInitStruct.PLL.PLLState = RCC_PLL_ON;
  oscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  oscInitStruct.PLL.PLLM = 8;
  oscInitStruct.PLL.PLLN = 100;
  oscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  oscInitStruct.PLL.PLLQ = 4;  // NC
  status = HAL_RCC_OscConfig(&oscInitStruct);
  if (status != HAL_OK) {
    return FALSE;
  }

  clkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                             RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  clkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&clkInitStruct, FLASH_LATENCY_1);
  if (status != HAL_OK) {
    return FALSE;
  }

  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_5);

  SystemCoreClockUpdate();

  return TRUE;
}

/**
 * @brief MainController_WaitingState функция состояния ожидания
 * @details На каждом такте запускает прием по UART если не запущено и проверяет
 * принята ли первая половина пакета; если да, то переход в состояние Started
 *
 * @param uartPackage место хранения буфера для UART
 * @param sizeOfUart размер UART пакета
 * @return новое состояние контроллера
 */
MainStates MainController_WaitingState(uint8_t* uartPackage,
                                       int32_t sizeOfUart) {
  if (!mainContoller.uart_working) {
    if (UART_Acquire(mainContoller.uart)) {
      if (UART_StartReceive(mainContoller.uart, UartHalf, UartFull, uartPackage,
                            sizeOfUart)) {
        mainContoller.uart_working = TRUE;
        return Waiting;
      }
    }
  }

  if (mainContoller.uart_half) {
    mainContoller.uart_half = FALSE;
    Display_SetState(mainContoller.display, Started);
    for (int i = 0; i < sizeOfUart / 2; i++) {
      PWM_AddWidth(mainContoller.pwm, uartPackage[i]);
    }

    PWM_Start(mainContoller.pwm);
    return Started;
  }

  return Waiting;
}

/**
 * @brief MainController_StartedState функция запущеного состояния
 * @param uartPackage место хранения буфера для UART
 * @param sizeOfUart размер UART пакета
 * @param bufUsage текущая наполненость буфера
 * @return новое состояние контроллера
 */
MainStates MainController_StartedState(uint8_t* uartPackage, int32_t sizeOfUart,
                                       int32_t bufUsage) {
  if (mainContoller.uart_half) {
    mainContoller.uart_half = FALSE;
    Display_SetState(mainContoller.display, Started);
    for (int i = 0; i < sizeOfUart / 2; i++) {
      PWM_AddWidth(mainContoller.pwm, uartPackage[i]);
    }
  }

  if (mainContoller.uart_full) {
    mainContoller.uart_full = FALSE;
    // Start new receive while processing second part
    UART_Free(mainContoller.uart);
    if (UART_Acquire(mainContoller.uart)) {
      if (UART_StartReceive(mainContoller.uart, UartHalf, UartFull, uartPackage,
                            sizeOfUart)) {
        mainContoller.uart_working = TRUE;
      }
    }

    for (int i = sizeOfUart / 2; i < sizeOfUart; i++) {
      PWM_AddWidth(mainContoller.pwm, uartPackage[i]);
    }
    return Started;
  }

  if (Keypad_GetState(mainContoller.keypad, KeyStartStop) == Clicked) {
    return Stopping;
  }

  if (bufUsage == 0) {
    return Stopping;
  }

  return Started;
}

/**
 * @brief MainController_StoppingState функция остановки генерации
 * @details ожидает воспроизведения остатков буфера
 * @param bufUsage текущая наполненость буфера
 * @return новое состояние контроллера
 */
MainStates MainController_StoppingState(int32_t bufUsage) {
  mainContoller.uart_working = FALSE;
  UART_Free(mainContoller.uart);

  if (bufUsage == 0) {
    PWM_Stop(mainContoller.pwm);
    return Stopped;
  }
  return Stopping;
}

/**
 * @brief HAL_MspInit инициализация микропроцессора
 */
void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief SysTick_Handler прерывание по SysTick
 * @details позволяет определить время с момента инициализации
 */
void SysTick_Handler(void) { HAL_IncTick(); }

/// @}
