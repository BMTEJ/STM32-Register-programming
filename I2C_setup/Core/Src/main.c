#include "RccConfig.h"
#include "Delay.h"
#include <stdlib.h>


void I2CConfig(){
	//Set up GPIOB for pins PB10 (I2C SCL) and PB3 (I2C SDA)

	RCC->AHB1ENR |= (1<<1); //Enable GIPOB clock.
	RCC->APB1ENR |= (1 << 22); //Enable I2C(2)


	GPIOB->MODER |= (2 << 6); //Set PB3 mode to alternate func
	GPIOB->MODER |= (2 << 20); //Set PB10 mode to alternate func
	GPIOB->OTYPER |= (1 << 3); //Set PB3 to open drain
	GPIOB->OTYPER |= (1 << 10); //Set PB10 to open drain
	GPIOB->OSPEEDR |= (2 << 6); // Set output speed of PB3 to high
	GPIOB->OSPEEDR |= (2 << 20); // Set output speed of PB10 to high
	GPIOB->PUPDR |= (1 << 6); //Set PB3 to pull-up
	GPIOB->PUPDR |= (1 << 20); //Set PB10 to pull-up.
	GPIOB->AFR[0] |= (4 << 12); //Set alternate function for PB3 for I2S
	GPIOB->AFR[1] |= (4 << 8); //Set alternate function for PB10 for I2C

	//Set up I2C2

	I2C2->CR1 |= (1<<15); //Reset I2C2
	I2C2->CR1 &= ~(1<<15); //Reset I2C2

	I2C2->CR2 |= (42 << 0); //Set frequency to 42Mhz

	I2C2->CCR &= ~(1 << 15); //Set to standard mode
	I2C2->CCR &= ~(1 << 14); //Set duty to 0. Not using fm mode.
	I2C2->CCR |= (210 << 0); // Set CCR value.
	I2C2->TRISE = 43; //Set rise time
	I2C2->CR1 |= (1<<0); //Enable peripheral

}

void I2C_Start(){
	I2C2->CR1 |= (1 << 8); //Set I2C to master mode
	while(!(I2C2->SR1 & (1 << 0))); //Wait for start bit to go high
}

void I2C_Write(uint8_t c){
	while(!(I2C2->SR1 & (1 << 7))); //Wait for data register to be empty
	I2C2->DR = c; //Send data
	while(!(I2C2->SR1 & (1 << 2))); //Wait for data register to empty and byte transfer finished to be set.
}

void I2C_Address(uint8_t addr){
	I2C2->DR = addr; //send address
	while(!(I2C2->SR1 & (1 << 1))); //Wait for address sent bit to be set.
	uint8_t read = I2C2->SR1 | I2C2->SR2; // Read SR1 and SR2 to clear address bit.

}

void I2C_Stop(){
	I2C2->CR1 |= (1 << 9); //Stop transmission.
}

void I2C_WriteMulti(uint8_t* data, uint8_t size){
	while(!(I2C2->SR1 & (1 << 7))); //wait for data register to empty.

	while(size){
		while(!(I2C2->SR1 & (1 << 7))); //wait for data register to empty.
		I2C2->DR = *data++; //send data and advance pointer to next byte.
		size --;
	}
	while(!(I2C2->SR1 & (1 << 2))); //Wait for byte transmission to finish.
}



int main(void) {
    SysClockConfig();
    I2CConfig();
    TIM5Config();

    I2C_Start();
    I2C_Address(0x27);
    I2C_Write(0x00);
    I2C_Stop();

    while (1) {

    }
}
