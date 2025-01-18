#include "Delay.h"
#include "RccConfig.h"

void TIM4Config(){
	//Enable TIM5
	RCC->APB1ENR |= (1 << 2);

	//Set prescaler to achieve 1 us/cycle and max count before reset
	TIM4->PSC = 84 - 1; //84MHz/84 = 1MHz => t = 1/MHz = 1us
	TIM4->ARR = 0xffff;

	//Clear UIF flag
	TIM4->SR &= ~(1 << 0);

	//Enable counter and wait for enable bit to set
	TIM4->CR1 |= (1<<0);
	while(!(TIM4->SR & (1<<0)));

}

void Delay_us(uint32_t time){
	// Reset counter
	TIM4->CNT = 0;
	// Wait until counter reaches time
	while (TIM4->CNT < time);
}

void Delay_ms(uint32_t time){

	//Wait until counter reaches time
	for (uint32_t i = 0; i < time; i ++){
		Delay_us(1000); //wait 1ms by delaying for 1000us
	}
}

