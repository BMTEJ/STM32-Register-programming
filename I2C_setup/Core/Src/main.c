#include "RccConfig.h"
#include "Delay.h"


void I2CConfig(){
	//Set up GPIOB for pins PB10 (I2C SCL) and PB3 (I2C SDA)

	RCC->AHB1ENR |= (1<<1); //Enable GIPOB clock.
	GPIOB->AFR[0] |= (4 << 12); //Set alternate function for PB3 for I2S
	GPIOB->AFR[1] |= (4 << 8); //Set alternate function for PB10 for I2C
	GPIOB->MODER |= (2 << 6); //Set PB3 mode to alternate func
	GPIOB->MODER |= (2 << 20); //Set PB10 mode to alternate func
	GPIOB->OTYPER &= ~(1 << 3); //Set PB3 to open drain
	GPIOB->OTYPER &= ~(1 << 10); //Set PB10 to open drain
	GPIOB->OSPEEDR |= (2 << 6); // Set output speed of PB3 to high
	GPIOB->OSPEEDR |= (2 << 20); // Set output speed of PB10 to high
	GPIOB->PUPDR |= (1 << 6); //Set PB3 to pull-up
	GPIOB->PUPDR |= (1 << 20); //Set PB10 to pull-up.

	//Set up I2C2

	RCC->APB1ENR |= (1 << 22); //Enable I2C(2)

	I2C2->CR1 |= (1<<15); //Reset I2C2
	i2C2->CR1 &= ~(1<<15); //Reset I2C2

	I2C2->CR2 |= (42 << 0); //Set frequency to 42Mhz

	I2C2->CCR &= ~(1 << 15); //Set to standard mode
	I2C2->CCR &= ~(1 << 14); //Set duty to 0. Not using fm mode.
	I2C2->CCR |= (210 << 0); // Set CCR value.
	I2C2->TRISE = 43; //Set rise time
	I2C2->CR1 &= (1<<0); //Enable peripheral

}


int main (void){


	while(1){

	}


}
