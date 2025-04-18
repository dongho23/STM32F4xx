/*

  main.c - driver code for STM32F4xx ARM processors

  Part of grblHAL

  Copyright (c) 2019-2025 Terje Io
  Some parts (C) COPYRIGHT STMicroelectronics - code created by IDE

  grblHAL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  grblHAL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with grblHAL. If not, see <http://www.gnu.org/licenses/>.

*/

#include "main.h"
#include "driver.h"
#include "grbl/grbllib.h"

static void SystemClock_Config (void);
static void MX_GPIO_Init (void);

int main (void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
#ifdef UF2_BOOTLOADER
    HAL_RCC_DeInit();
    HAL_DeInit();
    extern uint8_t _FLASH_VectorTable;
    __disable_irq();
    SCB->VTOR = (uint32_t)&_FLASH_VectorTable;
    __DSB();
    __enable_irq();
#endif
    
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    if(!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }

    grbl_enter();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config (void)
{
  __HAL_RCC_PWR_CLK_ENABLE();

#ifdef STM32F412Vx

  #if defined(BOARD_LONGBOARD32) || defined(BOARD_LONGBOARD32_EXT)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .LSIState = RCC_LSI_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = 16,
        .PLL.PLLN = 256,
        .PLL.PLLP = RCC_PLLP_DIV4,
        .PLL.PLLQ = 8,
        .PLL.PLLR = 2
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2

    #define FLASH_LATENCY FLASH_LATENCY_3

  #endif

#elif defined(STM32F446xx)

  #if defined(NUCLEO_F446)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
#if RTC_ENABLE
        .OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE,
        .LSEState       = RCC_LSE_ON,
#else
        .OscillatorType = RCC_OSCILLATORTYPE_HSI,
#endif
        .HSIState = RCC_HSI_ON,
        .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSI,
        .PLL.PLLM = 16,
        .PLL.PLLN = 360,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 2,
        .PLL.PLLR = 2
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2
    #define FLASH_LATENCY FLASH_LATENCY_5

  #elif defined(NUCLEO144_F446)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
#if RTC_ENABLE
        .OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE,
        .LSEState       = RCC_LSE_ON,
#else
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
#endif
        .HSEState = RCC_HSE_BYPASS,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = 4,
        .PLL.PLLN = 180,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 7,
        .PLL.PLLR = 2
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV4
    #define FLASH_LATENCY FLASH_LATENCY_5

  #elif defined(BOARD_FLEXI_HAL)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = 15,
        .PLL.PLLN = 216,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 8,
        .PLL.PLLR = 2
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2
    #define FLASH_LATENCY FLASH_LATENCY_5

    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

  #elif defined(BOARD_FYSETC_S6)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = 12,
        .PLL.PLLN = 336,
        .PLL.PLLP = 2,
        .PLL.PLLQ = 7,
        .PLL.PLLR = 2
    };

    #define APB1CLKDIV RCC_HCLK_DIV2
    #define APB2CLKDIV RCC_HCLK_DIV1
    #define FLASH_LATENCY FLASH_LATENCY_5

  #else

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
  #if HSE_VALUE == 8000000
        .PLL.PLLM = 7,
        .PLL.PLLN = 294,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 7,
        .PLL.PLLR = 7
  #else
        .PLL.PLLM = (uint32_t)HSE_VALUE / 1000000UL,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 2,
        .PLL.PLLR = 2
  #endif
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2
    #define FLASH_LATENCY FLASH_LATENCY_5

  #endif

#elif defined(STM32F429xx)

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = (uint32_t)HSE_VALUE / 1000000UL,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 7
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2
    #define FLASH_LATENCY FLASH_LATENCY_5

#elif defined(STM32F411xE)

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  #ifdef NUCLEO_F411

    RCC_OscInitTypeDef RCC_OscInitStruct = {
#if RTC_ENABLE
        .OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE,
        .LSEState       = RCC_LSE_ON,
#else
        .OscillatorType = RCC_OSCILLATORTYPE_HSI,
#endif
        .HSIState = RCC_HSI_ON,
        .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSI,
        .PLL.PLLM = 16,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV4,
        .PLL.PLLQ = 4
    };

    #define FLASH_LATENCY FLASH_LATENCY_2

  #else // BlackPill

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = (uint32_t)HSE_VALUE / 1000000UL,
        .PLL.PLLN = 192,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 4
    };

    #define FLASH_LATENCY FLASH_LATENCY_3

  #endif

#elif defined(STM32F407xx)

   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitTypeDef RCC_OscInitStruct = {
#if RTC_ENABLE
        .OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE,
        .LSEState       = RCC_LSE_ON,
#else
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
#endif
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = (uint32_t)HSE_VALUE / 1000000UL,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV2,
        .PLL.PLLQ = 7
    };

    #define APB1CLKDIV RCC_HCLK_DIV4
    #define APB2CLKDIV RCC_HCLK_DIV2
    #define FLASH_LATENCY FLASH_LATENCY_5

#else // STM32F401

   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  #ifdef NUCLEO_F401

    RCC_OscInitTypeDef RCC_OscInitStruct = {
#if RTC_ENABLE
        .OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE,
        .LSEState       = RCC_LSE_ON,
#else
        .OscillatorType = RCC_OSCILLATORTYPE_HSI,
#endif
        .HSIState = RCC_HSI_ON,
        .HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSI,
        .PLL.PLLM = 16,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV4,
        .PLL.PLLQ = 4
    };

  #else

    RCC_OscInitTypeDef RCC_OscInitStruct = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSE,
        .HSEState = RCC_HSE_ON,
        .PLL.PLLState = RCC_PLL_ON,
        .PLL.PLLSource = RCC_PLLSOURCE_HSE,
        .PLL.PLLM = (uint32_t)HSE_VALUE / 1000000UL,
        .PLL.PLLN = 336,
        .PLL.PLLP = RCC_PLLP_DIV4,
        .PLL.PLLQ = 7
    };

  #endif

    #define FLASH_LATENCY FLASH_LATENCY_2

#endif

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

#if defined(NUCLEO144_F446)

    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
      Error_Handler();
    }

#endif

#ifndef APB1CLKDIV
#define APB1CLKDIV RCC_HCLK_DIV2
#endif
#ifndef APB2CLKDIV
#define APB2CLKDIV RCC_HCLK_DIV1
#endif

    /** Initializes the CPU, AHB and APB busses clocks */
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {
        .ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
        .AHBCLKDivider = RCC_SYSCLK_DIV1,
        .APB1CLKDivider = APB1CLKDIV,
        .APB2CLKDivider = APB2CLKDIV
    };

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY) != HAL_OK) {
        Error_Handler();
    }

#if USB_SERIAL_CDC && (defined(STM32F446xx) || defined(STM32F412Vx))

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {
  #ifdef NUCLEO144_F446
         .PeriphClockSelection = RCC_PERIPHCLK_CLK48,
        .PLLSAI.PLLSAIM = 8,
        .PLLSAI.PLLSAIN = 192,
        .PLLSAI.PLLSAIQ = 2,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivQ = 1,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP
  #elif defined(BOARD_FLEXI_HAL)
        .PeriphClockSelection = RCC_PERIPHCLK_CLK48,
        .PLLSAI.PLLSAIM = 25,
        .PLLSAI.PLLSAIN = 192,
        .PLLSAI.PLLSAIQ = 2,
        .PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4,
        .PLLSAIDivQ = 1,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP
  #elif defined(BOARD_LONGBOARD32)
        .PeriphClockSelection = RCC_PERIPHCLK_PLLI2S|RCC_PERIPHCLK_CLK48|RCC_PERIPHCLK_SDIO,
        .PLLI2S.PLLI2SN = 192,
        .PLLI2S.PLLI2SM = 25,
        .PLLI2S.PLLI2SR = 2,
        .PLLI2S.PLLI2SQ = 4,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLI2SQ,
        .SdioClockSelection = RCC_SDIOCLKSOURCE_CLK48,
        .PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC
  #else
        .PeriphClockSelection = RCC_PERIPHCLK_CLK48,
        .Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ
  #endif
    };

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

#endif
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
#ifdef GPIOF
  __HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
  __HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
  __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
