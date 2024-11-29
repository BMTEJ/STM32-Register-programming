#include "Delay.h"
#include "RccConfig.h"

void TIM5Config(){
	//Enable TIM5
	RCC->APB1ENR |= (1 << 3);

	//Set prescaler to achieve 1 us/cycle and max count before reset
	TIM5->PSC = 84 - 1;
	TIM5->ARR = 0xffffffff;

	//Clear UIF flag
	TIM5->SR &= ~(1 << 0);

	//Enable counter and wait for it to enable
	TIM5->CR1 |= (1<<0);
	while(!(TIM5->SR & (1<<0)));

}

void Delay_us(uint32_t time){
	// Reset counter
	TIM5->CNT = 0;
	// Wait until counter reaches time
	while (TIM5->CNT < time);
}


void Delay_ms(uint32_t time){

	//Wait until counter reaches time
	for (uint32_t i = 0; i < time; i ++){
		Delay_us(1000); //wait 1ms by delaying for 1000us
	}
}

