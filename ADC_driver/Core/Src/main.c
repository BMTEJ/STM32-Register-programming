#include "RccConfig.h"


void ADC_Init(){

	//Enable the ADC clock from APB2 and GPIOA clock from AHB1
	RCC->APB2ENR |= (1 << 8);
	RCC->AHB1ENR |= (1<<0);

	//Set prescaler to 4 (max frequency 36, frequency = (APB2 = 84MHz) / (psc = 4)
	ADC->CCR |= (1 << 16);

	//Set scan mode and resolution
	ADC->CR1 |= (1 << 8); //Scan mode enabled
	ADC->CR1 &= ~(3 << 24); //12 bit resolution

	//Setting continuous conversion, EOC, and data alignment
	ADC->CR2 |= (1 << 1); //Setting continuous conversion
	ADC->CR2 |= (1 << 10); //Overrun bit set at the end of each conversion.
	ADC->CR2 &= ~(1 << 11); //Align data to the right

	//Configure sampling times for channels 1, 4, and 16 (latter is for temperature sensor).
	ADC->SMPR2 &= ~((3<<3) | (3<<12)); // Set channel 1 to sampling time of 3 and channel 4 to sampling time of 3.
	ADC->SMPR1 &= ~(3 << 18); //Min samp_time for temp_sens = 10us, 10us/47,62ns = 210 cycles required, so setting to 480.

	//Set number of channels in conversion sequence (2 in this case)
	ADC->SQR1 |= (2 << 20); //Number of channels in sequence.

	//Set respective GPIO PINs to Analog Mode.
	GPIOA->MODER |= ((3 << 2) | (3 << 8)); //Set PA1 and PA4 to analog.
}

void ADC_Enable(void){
	//Enable ADC
	ADC->CR2 |= (1 << 0);
	//Enable built-in temp sensor
	ADC->CCR |= (1 << 23);

	//Wait for ADC to stabilize
	uint32_t delay = 1000;
	while(delay--);
}

void ADC_Start(int channel){
	ADC->SQR3 = 0;
	ADC->SQR3 |= (channel << 0); // mAKE

	ADC->SR = 0; //Clear status register

	ADC->CR2 |= (1 << 30); //Start conversion
}

void ADC_WaitForConversion(void){
	while (!(ADC->SR & (1 <<1))); //Wait for EOC flag to set.
}

uint16_t ADC_GetVal(void){
	return ADC->DR; //Return value
}

void ADC_Disable(void){
	ADC->CR &= ~(1<<0); // Disable ADC
}


uint16_t vals[2] = {0,0};

int main(){
	SysClockConfig();
	ADC_Init();
	ADC_Enable();

	while(1){
		ADC_Start(1);
		ADC_WaitForConv();
		ADC_VAL[0] = ADC_GetVal();

		ADC_Start(4);
		ADC_WaitForConv();
		ADC_VAL[1] = ADC_GetVal();
	}

}


