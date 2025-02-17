/*
 * RccConfig.c
 *
 *  Created on: Nov 27, 2024
 *      Author: allen
 */

/*
 * SysClkConfig.h
 *
 *  Created on: Nov 27, 2024
 *      Author: Brandon Allen
 */

#ifndef INC_SYSCLKCONFIG_H_
#define INC_SYSCLKCONFIG_H_



#endif /* INC_SYSCLKCONFIG_H_ */

/*
 * SysClkConfig.c
 *
 *  Created on: Nov 27, 2024
 *      Author: Brandon Allen
 */

#include "stm32f401xe.h"

#define PLL_M 4 // Divide PLL input by 4. 8/4 = 2MHz input
#define PLL_N 84 // Multiply frequency by 84. 2 * 84 = 168Mhz
#define PLL_P 0 // Divide frquency by 2. 168/2 = 84Mhz

void SysClockConfig(){
	/**********************>>>>> STEPS FOLLOWED <<<<<***********************
				1. ENABLE HSE and wait for the HSE to become Ready
				2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
				3. Configure the FLASH PREFETCH and the LATENCY Related Settings
				4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
				5. Configure the MAIN PLL
				6. Enable the PLL and wait for it to become ready
				7. Select the Clock Source and wait for it to be set
	************************************************************************/

		//Using STM32CubeMX to inform clock setup.

		//1. ENABLE HSE and wait for the HSE to become Ready
		RCC->CR |= (1 << 16);
		while(!(RCC->CR & (1<<17))){}

		//2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR
		RCC->APB1ENR |= (1<<28); //Enable power
		PWR->CR |= (3 << 14); //keep voltage regulator in scale 1 (reset)

		//3. Configure the FLASH PREFETCH and the LATENCY Related Settings
		FLASH->ACR |= (1 << 10); //Enable data cache
		FLASH->ACR |= (1 << 9); //Enable instruction cache
		FLASH->ACR |= (1 << 8); //Enable prefetch cache
		FLASH->ACR |= (2 << 0); //Set flash latency to two wait states (3 CPU cycles)

		//4. Configure the PRESCALARS HCLK, PCLK1, PCLK2
		RCC->CFGR = (0 << 4) | (4 << 10) | (0 << 13); //AHB not divided | APB1 divided by 2 | APB2 not divided

		//5. Configure the MAIN PLL
		//PLL input division factor of 4 | PLL output multiplication factor of 84 | PLL output division factor of 2 | HSE selected as PLL src.
		RCC->PLLCFGR = (PLL_M << 0) | (PLL_N << 6) | (PLL_P << 16) | (1 << 22);

		//6. Enable the PLL and wait for it to become ready
		RCC->CR |= (1 << 24); // enable PLL
		while(!(RCC->CR & (1<<25))){}; //wait for PLL to be ready

		//7. Select the Clock Source and wait for it to be set
		RCC->CFGR |= (2<<0); // Set PLL as clock source
		while(!(RCC->CFGR & (2 << 2))){}; // Wait for switch from default clk src (HSI) to PLL.

}


