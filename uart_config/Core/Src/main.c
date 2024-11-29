#include "RccConfig.h"
#include "Delay.h"


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

void UART2_GetChar(void){

	uint8_t k;
	if((USART2->SR & (1<<5))){ //Check if data is loaded
		k = USART2->DR; //Get data if loaded
		if (k == 'l' ){ //turn on LED if k == 'l'
			GPIOA->ODR |= (1 << 5);

		} else{ //turn off otherwise
			 GPIOA->ODR &= ~(1 << 5);

		}
	}

}

void LEDConfig(void) {
	//2. Set the Pin as OUTPUT
	GPIOA->MODER |= (1<<10); //Set PA5 to output mode (green LED)

	//3. Configure the OUTPUT MODE
	GPIOA->OTYPER &= ~(1<<5); //Set PA5 to push-pull.
	GPIOA->OSPEEDR &= ~(3 << 10); //Set PA5 to high speed.

}


int main (void){
	SysClockConfig();
	TIM5Config();
	Usart2Config();
	LEDConfig();

	while(1){
		UART2_GetChar();
		UART2_SendString("hello\r\n");
		Delay_ms(1000);
	}
}
