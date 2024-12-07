/*
 * UART2Config.c
 *
 *  Created on: Nov 30, 2024
 *      Author: allen
 */

#include "UART2Config.h"

void Usart2Config(){
	//Enable clock for USART2
	RCC->APB1ENR |= (1<<17);
	//Enable GPIOA to configure PA2 and PA3 for USART2
	RCC->AHB1ENR |= (1 << 0);
	//Set alternate function of PA2 and PA3 for USART2
	GPIOA->AFR[0] |= (7 << 8);
	GPIOA->AFR[0] |= (7 << 12);

	//Set MODE for PA2 and PA3 for alternate function
	GPIOA->MODER |= (2 << 4);
	GPIOA->MODER |= (2 << 6);

	//Set speed for PA2 and PA3;
	GPIOA->OSPEEDR |= (2 << 4);
	GPIOA->OSPEEDR |= (2 << 6);

	USART2->CR1 |= (1<<13); //Enable USART2
	USART2->CR1 &= ~(1<<12); //Select 8 data bits

	//Baud rate set up, USARTDIV = 22.7866 for 115200 baudrate and over8 = 0
	USART2->BRR = (7 << 0) | (22 << 4); //Set fraction to 7. Mantissa to 22

	 //Enable transmitting and receiving
	USART2->CR1 |= (3 << 2);

}

void UART2_SendChar(char c){

	USART2->DR = c; //load data into data register
	while (!(USART2->SR & (1<<6))); //Wait for data to send.

}

void UART2_SendString(char* string){

	while (*string) UART2_SendChar(*string++); //Send one character at a time via UART.
}
