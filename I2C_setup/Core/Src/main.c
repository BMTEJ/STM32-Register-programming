#include "RccConfig.h"
#include "Delay.h"
#include "UART2Config.h"
#include <stdlib.h>


void I2CConfig(){
	//Set up GPIOB for pins PB10 (I2C SCL) and PB3 (I2C SDA)

	RCC->AHB1ENR |= (1<<1); //Enable GIPOB clock.
	RCC->APB1ENR |= (1 << 21); //Enable I2C1.


	GPIOB->MODER |= (2 << 16) | (2 << 18); //Set PB8 mode to alternate func. Set PB9 mode to alternate func
	GPIOB->OTYPER |= (1 << 8) | (1 << 9); //Set PB8 to open drain. Set PB9 to open drain
	GPIOB->OSPEEDR |= (3 << 16) | (3 << 18); // Set output speed of PB8 to low. Set output speed of PB9 to low.
	GPIOB->PUPDR |= (1 << 16) | (1 << 18); //Set PB9 to pull-up. Set PB9 to pull-up.
	GPIOB->AFR[1] |= (4 << 0); //Set alternate function for PB8 for I2C
	GPIOB->AFR[1] |= (4 << 4); //Set alternate function for PB9 for I2C

	//Set up I2C1

	I2C1->CR1 |= (1<<15); //Reset I2C1
	I2C1->CR1 &= ~(1<<15); //Reset I2C1

	I2C1->CR2 |= (42 << 0); //Set frequency to 42Mhz

	I2C1->CCR &= ~(1 << 15); //Set to standard mode
	I2C1->CCR &= ~(1 << 14); //Set duty to 0. Not using fm mode.
	I2C1->CCR |= (210 << 0); // Set CCR value.
	I2C1->TRISE = 43; //Set rise time
	I2C1->CR1 |= (1<<0); //Enable peripheral

}

void I2C_Start(){
	I2C1->CR1 |= (1 << 8); //Start communication
	while(!(I2C1->SR1 & (1 << 0))){
		UART2_SendString("start not sent \r\n");
	}
	UART2_SendString("start sent \r\n");
}

void I2C_Write(uint8_t c){
	while(!(I2C1->SR1 & (1 << 7))); //Wait for data register to be empty
	I2C1->DR = c; //Send data
	while(!(I2C1->SR1 & (1 << 2))); //Wait for data register to empty and byte transfer finished to be set.

}

void I2C_Address(uint8_t addr){
	I2C1->DR = addr; //send address
	UART2_SendString("acknowledged \r\n");
	uint8_t read = I2C1->SR1 | I2C1->SR2; // Read SR1 and SR2 to clear address bit.

}

void I2C_Stop(){
	I2C1->CR1 |= (1 << 9); //Stop transmission.
}

void I2C_WriteMulti(uint8_t* data, uint8_t size){
	while(!(I2C1->SR1 & (1 << 7))); //wait for data register to empty.

	while(size){
		while(!(I2C1->SR1 & (1 << 7))); //wait for data register to empty.
		I2C1->DR = *data++; //send data and advance pointer to next byte.
		size --;
	}
	while(!(I2C1->SR1 & (1 << 2))); //Wait for byte transmission to finish.
}

void I2C_Scan() {
    UART2_SendString("Starting I2C address scan...\r\n");

    for (uint8_t addr = 1; addr < 128; addr++) { // 7-bit I2C addresses
        // Generate START condition
        I2C1->CR1 |= (1 << 8); // START
        while (!(I2C1->SR1 & (1 << 0))); // Wait for START to be sent

        // Send the address (write mode)
        I2C1->DR = addr << 1; // Shift 7-bit address and add R/W bit (0 for write)

        // Wait for ACK or timeout
        uint32_t timeout = 100000; // Initialize timeout counter
        while (!(I2C1->SR1 & (1 << 1)) && --timeout); // Wait for ADDR bit (ACK)

        if (timeout > 0) {
            // ACK received
            char buffer[20];
            sprintf(buffer, "ACK at 0x%02X\r\n", addr); // Format address as hex string
            UART2_SendString(buffer); // Send the formatted string

            // Clear ADDR flag
            volatile uint8_t temp = I2C1->SR1 | I2C1->SR2;
        } else {
            // Timeout occurred, no ACK
            UART2_SendString("No ACK received for ");
            char buffer[10];
            sprintf(buffer, "0x%02X\r\n", addr);
            UART2_SendString(buffer);

            // Ensure the bus is released
            I2C1->CR1 |= (1 << 9); // STOP
        }

        // Short delay before moving to the next address
        for (volatile int delay = 0; delay < 1000; delay++);
    }

    UART2_SendString("I2C address scan complete.\r\n");
    while(1);
}


int main(void) {
    RCC->APB1RSTR |= (1 << 21);
    RCC->APB1RSTR &= ~(1 << 21);
    SysClockConfig();
    I2CConfig();
    Usart2Config();
    TIM5Config();
    UART2_SendString("starting light showa\r\n");


    while (1) {
    	for(int i = 0; i < 8; i++){
    		I2C_Start();
    		I2C_Address(0x4E);
    		I2C_Write(1 << i);
    		I2C_Stop();
    		Delay_ms(300);
    	}
    }
}
