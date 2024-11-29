/********
 * RCC Config to set up system clock and enable flash
 * Clock speed: 84Mhz
 * APB1: 42Mhz, Prescaler 2
 * APB2: 84Mhz, Prescaler 1
 * AHB1: 84Mhz, Prescaler 1
 * PLL source: HSE
 * clock source: PLL
 * PLL_M: 4
 * PLL_N: 84
 * PLL_P: 2
 */

#include "stm32f401xe.h"
void SysClockConfig();
