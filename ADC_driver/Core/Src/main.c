#include "RccConfig.h"

/**
 * Static variables
 */
uint16_t vals[3] = {0, 0, 0};
float device_temp;
float external_temp;
uint16_t test_output;

float device_calc(uint16_t value){
	return ( (value - 760)/2.5) + 25;
}

float external_calc(uint16_t value){
//	float vref = 3.3; // ADC reference voltage
//	float vout = ( ((float) value) / 4096.0) * vref; // Convert ADC value to voltage
	return (float) (value - 500) / 10; // Convert voltage to temperature
}
void ADC_Init(){

	//Enable the ADC clock from APB2 and GPIOA clock from AHB1
	RCC->APB2ENR |= (1 << 8);
	RCC->AHB1ENR |= (1<<0);

	//Set prescaler to 4 (max frequency 36, frequency = (APB2 = 84MHz) / (psc = 4)
	ADC->CCR |= (1 << 16);

	//Set scan mode and resolution
	ADC1->CR1 |= (1 << 8); //Scan mode enabled
	ADC1->CR1 &= ~(3 << 24); //12 bit resolution

	//Setting continuous conversion, EOC, and data alignment
	ADC1->CR2 |= (1 << 1); //Setting continuous conversion
	ADC1->CR2 |= (1 << 10); //Overrun bit set at the end of each conversion.
	ADC1->CR2 &= ~(1 << 11); //Align data to the right

	//Configure sampling times for channels 1, 4, and 16 (latter is for temperature sensor).
	ADC1->SMPR2 &= ~((3<<3) | (3<<12)); // Set channel 1 to sampling time of 3 and channel 4 to sampling time of 3.
	ADC1->SMPR1 &= ~(3 << 18); //Min samp_time for temp_sens = 10us, 10us/47,62ns = 210 cycles required, so setting to 480.

	//Set number of channels in conversion sequence (2 in this case)
	ADC1->SQR1 |= (2 << 20); //Number of channels in sequence.

	//Set respective GPIO PINs to Analog Mode.
	GPIOA->MODER |= ((3 << 2) | (3 << 8)); //Set PA1 and PA4 to analog.
}

void ADC_Enable(void){
	//Enable ADC
	ADC1->CR2 |= (1 << 0);
	//Enable built-in temp sensor
	ADC->CCR |= (1 << 23);

	//Wait for ADC to stabilize
	uint32_t delay = 1000;
	while(delay--);
}

void ADC_Start(int channel){
	ADC1->SQR3 = 0;
	ADC1->SQR3 |= (channel << 0); // mAKE

	ADC1->SR = 0; //Clear status register

	ADC1->CR2 |= (1 << 30); //Start conversion
}

void ADC_WaitForConversion(void){
	while (!(ADC1->SR & (1 <<1))); //Wait for EOC flag to set.
}

uint16_t ADC_GetVal(void){
	return ADC1->DR; //Return value
}

void ADC_Disable(void){
	ADC1->CR2 &= ~(1<<0); // Disable ADC
}

void ADC_Sequential(){
	ADC1->SQR3 = 0;
	ADC1->SQR3 |= (1<<0); //Channel 1 first
	ADC1->SQR3 |= (4<<5); //Channel 2 second
	ADC1->SQR3 |= (16<<10); //Channel 16 (embedded temp sensor 3rd)

	ADC1->SR = 0; //Clear SR
	ADC1->CR2 |= (1 << 30); //Start conversion

	//Poll for each conversion
	for(int i = 0; i < 3; ){
		if (ADC1->SR & (1 <<1)){ //EOC set
			vals[i] = ADC1->DR;
			i++;
		}
	}
	test_output = vals[0];
	external_temp = external_calc(vals[1]);
	device_temp = device_calc(vals[2]);
}


int main(){
	SysClockConfig();
	ADC_Init();
	ADC_Enable();

	//Manual conversion method (scan mode made redundant)
//	while(1){
//		ADC_Start(1);
//		ADC_WaitForConversion();
//		test_output = ADC_GetVal();
//
//
//		ADC_Start(4);
//		ADC_WaitForConversion();
//		external_temp = external_calc(ADC_GetVal());
//
//		ADC_Start(16);
//		ADC_WaitForConversion();
//		device_temp = device_calc(ADC_GetVal());
//	}

	//Sequential order used
	while(1){
		ADC_Sequential();
	}

}


